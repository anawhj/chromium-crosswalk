// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/password_manager/core/browser/password_syncable_service.h"

#include "base/auto_reset.h"
#include "base/location.h"
#include "base/memory/scoped_vector.h"
#include "base/metrics/histogram.h"
#include "base/strings/utf_string_conversions.h"
#include "components/autofill/core/common/password_form.h"
#include "components/password_manager/core/browser/password_store_sync.h"
#include "net/base/escape.h"
#include "sync/api/sync_error_factory.h"

namespace password_manager {

// Converts the |password| into a SyncData object.
syncer::SyncData SyncDataFromPassword(const autofill::PasswordForm& password);

// Extracts the |PasswordForm| data from sync's protobuf format.
autofill::PasswordForm PasswordFromSpecifics(
    const sync_pb::PasswordSpecificsData& password);

// Returns the unique tag that will serve as the sync identifier for the
// |password| entry.
std::string MakePasswordSyncTag(const sync_pb::PasswordSpecificsData& password);
std::string MakePasswordSyncTag(const autofill::PasswordForm& password);

namespace {

// Returns true iff |password_specifics| and |password_specifics| are equal
// memberwise.
bool AreLocalAndSyncPasswordsEqual(
    const sync_pb::PasswordSpecificsData& password_specifics,
    const autofill::PasswordForm& password_form) {
  return (password_form.scheme == password_specifics.scheme() &&
          password_form.signon_realm == password_specifics.signon_realm() &&
          password_form.origin.spec() == password_specifics.origin() &&
          password_form.action.spec() == password_specifics.action() &&
          base::UTF16ToUTF8(password_form.username_element) ==
              password_specifics.username_element() &&
          base::UTF16ToUTF8(password_form.password_element) ==
              password_specifics.password_element() &&
          base::UTF16ToUTF8(password_form.username_value) ==
              password_specifics.username_value() &&
          base::UTF16ToUTF8(password_form.password_value) ==
              password_specifics.password_value() &&
          password_form.ssl_valid == password_specifics.ssl_valid() &&
          password_form.preferred == password_specifics.preferred() &&
          password_form.date_created.ToInternalValue() ==
              password_specifics.date_created() &&
          password_form.blacklisted_by_user ==
              password_specifics.blacklisted() &&
          password_form.type == password_specifics.type() &&
          password_form.times_used == password_specifics.times_used() &&
          base::UTF16ToUTF8(password_form.display_name) ==
              password_specifics.display_name() &&
          password_form.avatar_url.spec() == password_specifics.avatar_url() &&
          password_form.federation_url.spec() ==
              password_specifics.federation_url());
}

syncer::SyncChange::SyncChangeType GetSyncChangeType(
    PasswordStoreChange::Type type) {
  switch (type) {
    case PasswordStoreChange::ADD:
      return syncer::SyncChange::ACTION_ADD;
    case PasswordStoreChange::UPDATE:
      return syncer::SyncChange::ACTION_UPDATE;
    case PasswordStoreChange::REMOVE:
      return syncer::SyncChange::ACTION_DELETE;
  }
  NOTREACHED();
  return syncer::SyncChange::ACTION_INVALID;
}

// Creates a PasswordForm from |specifics| and |sync_time|, appends it to
// |entries|.
void AppendPasswordFromSpecifics(
    const sync_pb::PasswordSpecificsData& specifics,
    base::Time sync_time,
    ScopedVector<autofill::PasswordForm>* entries) {
  entries->push_back(
      new autofill::PasswordForm(PasswordFromSpecifics(specifics)));
  entries->back()->date_synced = sync_time;
}

bool IsEmptyPasswordForm(const autofill::PasswordForm& form) {
  return (form.username_value.empty() &&
          form.password_value.empty() &&
          !form.blacklisted_by_user);
}

bool IsEmptyPasswordSpecificsData(
    const sync_pb::PasswordSpecificsData& specifics) {
  return (specifics.username_value().empty() &&
          specifics.password_value().empty() &&
          !specifics.blacklisted());
}

}  // namespace

struct PasswordSyncableService::SyncEntries {
  ScopedVector<autofill::PasswordForm>* EntriesForChangeType(
      syncer::SyncChange::SyncChangeType type) {
    switch (type) {
      case syncer::SyncChange::ACTION_ADD:
        return &new_entries;
      case syncer::SyncChange::ACTION_UPDATE:
        return &updated_entries;
      case syncer::SyncChange::ACTION_DELETE:
        return &deleted_entries;
      case syncer::SyncChange::ACTION_INVALID:
        return NULL;
    }
    NOTREACHED();
    return NULL;
  }

  // List that contains the entries that are known only to sync.
  ScopedVector<autofill::PasswordForm> new_entries;

  // List that contains the entries that are known to both sync and the local
  // database but have updates in sync. They need to be updated in the local
  // database.
  ScopedVector<autofill::PasswordForm> updated_entries;

  // The list of entries to be deleted from the local database.
  ScopedVector<autofill::PasswordForm> deleted_entries;
};

PasswordSyncableService::PasswordSyncableService(
    PasswordStoreSync* password_store)
    : password_store_(password_store), is_processing_sync_changes_(false) {
}

PasswordSyncableService::~PasswordSyncableService() {
}

syncer::SyncMergeResult PasswordSyncableService::MergeDataAndStartSyncing(
    syncer::ModelType type,
    const syncer::SyncDataList& initial_sync_data,
    scoped_ptr<syncer::SyncChangeProcessor> sync_processor,
    scoped_ptr<syncer::SyncErrorFactory> sync_error_factory) {
  DCHECK(CalledOnValidThread());
  DCHECK_EQ(syncer::PASSWORDS, type);
  base::AutoReset<bool> processing_changes(&is_processing_sync_changes_, true);
  syncer::SyncMergeResult merge_result(type);

  // We add all the db entries as |new_local_entries| initially. During model
  // association entries that match a sync entry will be removed and this list
  // will only contain entries that are not in sync.
  ScopedVector<autofill::PasswordForm> password_entries;
  PasswordEntryMap new_local_entries;
  if (!ReadFromPasswordStore(&password_entries, &new_local_entries)) {
    merge_result.set_error(sync_error_factory->CreateAndUploadError(
        FROM_HERE, "Failed to get passwords from store."));
    return merge_result;
  }

  if (password_entries.size() != new_local_entries.size()) {
    merge_result.set_error(sync_error_factory->CreateAndUploadError(
        FROM_HERE,
        "There are passwords with identical sync tags in the database."));
    return merge_result;
  }

  // Save |sync_processor_| only if reading the PasswordStore succeeded. In case
  // of failure Sync shouldn't receive any updates from the PasswordStore.
  sync_error_factory_ = sync_error_factory.Pass();
  sync_processor_ = sync_processor.Pass();

  merge_result.set_num_items_before_association(new_local_entries.size());

  SyncEntries sync_entries;
  // Changes from password db that need to be propagated to sync.
  syncer::SyncChangeList updated_db_entries;
  for (syncer::SyncDataList::const_iterator sync_iter =
           initial_sync_data.begin();
       sync_iter != initial_sync_data.end(); ++sync_iter) {
    CreateOrUpdateEntry(*sync_iter,
                        &new_local_entries,
                        &sync_entries,
                        &updated_db_entries);
  }

  for (PasswordEntryMap::iterator it = new_local_entries.begin();
       it != new_local_entries.end(); ++it) {
    if (IsEmptyPasswordForm(*it->second)) {
      // http://crbug.com/404012. Remove the empty password from the local db.
      // This code can be removed once all users have their password store
      // cleaned up. This should happen in M43 and can be verified using crash
      // reports.
      sync_entries.deleted_entries.push_back(
          new autofill::PasswordForm(*it->second));
    } else {
      updated_db_entries.push_back(
          syncer::SyncChange(FROM_HERE,
                             syncer::SyncChange::ACTION_ADD,
                             SyncDataFromPassword(*it->second)));
    }
  }

  WriteToPasswordStore(sync_entries);
  merge_result.set_num_items_after_association(
      merge_result.num_items_before_association() +
      sync_entries.new_entries.size());
  merge_result.set_num_items_added(sync_entries.new_entries.size());
  merge_result.set_num_items_modified(sync_entries.updated_entries.size());
  merge_result.set_num_items_deleted(sync_entries.deleted_entries.size());
  merge_result.set_error(
      sync_processor_->ProcessSyncChanges(FROM_HERE, updated_db_entries));
  return merge_result;
}

void PasswordSyncableService::StopSyncing(syncer::ModelType type) {
  DCHECK(CalledOnValidThread());
  DCHECK_EQ(syncer::PASSWORDS, type);

  sync_processor_.reset();
  sync_error_factory_.reset();
}

syncer::SyncDataList PasswordSyncableService::GetAllSyncData(
    syncer::ModelType type) const {
  DCHECK(CalledOnValidThread());
  DCHECK_EQ(syncer::PASSWORDS, type);
  ScopedVector<autofill::PasswordForm> password_entries;
  ReadFromPasswordStore(&password_entries, NULL);

  syncer::SyncDataList sync_data;
  for (PasswordForms::iterator it = password_entries.begin();
       it != password_entries.end(); ++it) {
    sync_data.push_back(SyncDataFromPassword(**it));
  }
  return sync_data;
}

syncer::SyncError PasswordSyncableService::ProcessSyncChanges(
    const tracked_objects::Location& from_here,
    const syncer::SyncChangeList& change_list) {
  DCHECK(CalledOnValidThread());
  base::AutoReset<bool> processing_changes(&is_processing_sync_changes_, true);
  SyncEntries sync_entries;
  base::Time time_now = base::Time::Now();

  for (syncer::SyncChangeList::const_iterator it = change_list.begin();
       it != change_list.end(); ++it) {
    const sync_pb::EntitySpecifics& specifics = it->sync_data().GetSpecifics();
    ScopedVector<autofill::PasswordForm>* entries =
        sync_entries.EntriesForChangeType(it->change_type());
    if (!entries) {
      return sync_error_factory_->CreateAndUploadError(
          FROM_HERE, "Failed to process sync changes for passwords datatype.");
    }
    AppendPasswordFromSpecifics(
        specifics.password().client_only_encrypted_data(), time_now, entries);
  }
  WriteToPasswordStore(sync_entries);
  return syncer::SyncError();
}

void PasswordSyncableService::ActOnPasswordStoreChanges(
    const PasswordStoreChangeList& local_changes) {
  DCHECK(CalledOnValidThread());

  if (!sync_processor_) {
    if (!flare_.is_null()) {
      flare_.Run(syncer::PASSWORDS);
      flare_.Reset();
    }
    return;
  }

  // ActOnPasswordStoreChanges() can be called from ProcessSyncChanges(). Do
  // nothing in this case.
  if (is_processing_sync_changes_)
    return;
  syncer::SyncChangeList sync_changes;
  for (PasswordStoreChangeList::const_iterator it = local_changes.begin();
       it != local_changes.end(); ++it) {
    syncer::SyncData data = (it->type() == PasswordStoreChange::REMOVE ?
        syncer::SyncData::CreateLocalDelete(MakePasswordSyncTag(it->form()),
                                            syncer::PASSWORDS) :
        SyncDataFromPassword(it->form()));
    sync_changes.push_back(
        syncer::SyncChange(FROM_HERE, GetSyncChangeType(it->type()), data));
  }
  sync_processor_->ProcessSyncChanges(FROM_HERE, sync_changes);
}

void PasswordSyncableService::InjectStartSyncFlare(
    const syncer::SyncableService::StartSyncFlare& flare) {
  DCHECK(CalledOnValidThread());
  flare_ = flare;
}

bool PasswordSyncableService::ReadFromPasswordStore(
    ScopedVector<autofill::PasswordForm>* password_entries,
    PasswordEntryMap* passwords_entry_map) const {
  DCHECK(password_entries);
  if (!password_store_->FillAutofillableLogins(&password_entries->get()) ||
      !password_store_->FillBlacklistLogins(&password_entries->get())) {
    // Password store often fails to load passwords. Track failures with UMA.
    // (http://crbug.com/249000)
    UMA_HISTOGRAM_ENUMERATION("Sync.LocalDataFailedToLoad",
                              ModelTypeToHistogramInt(syncer::PASSWORDS),
                              syncer::MODEL_TYPE_COUNT);
    return false;
  }

  if (!passwords_entry_map)
    return true;

  PasswordEntryMap& entry_map = *passwords_entry_map;
  for (PasswordForms::iterator it = password_entries->begin();
       it != password_entries->end(); ++it) {
    autofill::PasswordForm* password_form = *it;
    entry_map[MakePasswordSyncTag(*password_form)] = password_form;
  }

  return true;
}

void PasswordSyncableService::WriteToPasswordStore(const SyncEntries& entries) {
  PasswordStoreChangeList changes;
  WriteEntriesToDatabase(&PasswordStoreSync::AddLoginImpl,
                         entries.new_entries.get(),
                         &changes);
  WriteEntriesToDatabase(&PasswordStoreSync::UpdateLoginImpl,
                         entries.updated_entries.get(),
                         &changes);
  WriteEntriesToDatabase(&PasswordStoreSync::RemoveLoginImpl,
                         entries.deleted_entries.get(),
                         &changes);

  // We have to notify password store observers of the change by hand since
  // we use internal password store interfaces to make changes synchronously.
  password_store_->NotifyLoginsChanged(changes);
}

void PasswordSyncableService::CreateOrUpdateEntry(
    const syncer::SyncData& data,
    PasswordEntryMap* unmatched_data_from_password_db,
    SyncEntries* sync_entries,
    syncer::SyncChangeList* updated_db_entries) {
  const sync_pb::EntitySpecifics& specifics = data.GetSpecifics();
  const sync_pb::PasswordSpecificsData& password_specifics(
      specifics.password().client_only_encrypted_data());
  std::string tag = MakePasswordSyncTag(password_specifics);

  // Check whether the data from sync is already in the password store.
  PasswordEntryMap::iterator existing_local_entry_iter =
      unmatched_data_from_password_db->find(tag);
  base::Time time_now = base::Time::Now();
  if (existing_local_entry_iter == unmatched_data_from_password_db->end()) {
    if (IsEmptyPasswordSpecificsData(password_specifics)) {
      // http://crbug.com/404012. Remove the empty password from the Sync
      // server. This code can be removed once all users have their password
      // store cleaned up. This should happen in M43 and can be verified using
      // crash reports.
      updated_db_entries->push_back(
          syncer::SyncChange(FROM_HERE,
                             syncer::SyncChange::ACTION_DELETE,
                             data));
    } else {
      // The sync data is not in the password store, so we need to create it in
      // the password store. Add the entry to the new_entries list.
      AppendPasswordFromSpecifics(password_specifics, time_now,
                                  &sync_entries->new_entries);
    }
  } else {
    // The entry is in password store. If the entries are not identical, then
    // the entries need to be merged.
    // If the passwords differ, take the one that was created more recently.
    const autofill::PasswordForm& password_form =
        *existing_local_entry_iter->second;
    if (!AreLocalAndSyncPasswordsEqual(password_specifics, password_form)) {
      if (base::Time::FromInternalValue(password_specifics.date_created()) <
          password_form.date_created) {
        updated_db_entries->push_back(
            syncer::SyncChange(FROM_HERE,
                               syncer::SyncChange::ACTION_UPDATE,
                               SyncDataFromPassword(password_form)));
      } else {
        AppendPasswordFromSpecifics(password_specifics, time_now,
                                    &sync_entries->updated_entries);
      }
    } else if (IsEmptyPasswordForm(password_form)) {
      // http://crbug.com/404012. Remove empty passwords from both the Sync
      // server and the local db. This code can be removed once all users have
      // their password store cleaned up. This should happen in M43 and can be
      // verified using crash reports.
      updated_db_entries->push_back(
          syncer::SyncChange(FROM_HERE,
                             syncer::SyncChange::ACTION_DELETE,
                             SyncDataFromPassword(password_form)));
      AppendPasswordFromSpecifics(password_specifics, time_now,
                                  &sync_entries->deleted_entries);
    }
    // Remove the entry from the entry map to indicate a match has been found.
    // Entries that remain in the map at the end of associating all sync entries
    // will be treated as additions that need to be propagated to sync.
    unmatched_data_from_password_db->erase(existing_local_entry_iter);
  }
}

void PasswordSyncableService::WriteEntriesToDatabase(
    DatabaseOperation operation,
    const PasswordForms& entries,
    PasswordStoreChangeList* all_changes) {
  for (PasswordForms::const_iterator it = entries.begin(); it != entries.end();
       ++it) {
    PasswordStoreChangeList new_changes = (password_store_->*operation)(**it);
    all_changes->insert(all_changes->end(),
                        new_changes.begin(),
                        new_changes.end());
  }
}

syncer::SyncData SyncDataFromPassword(
    const autofill::PasswordForm& password_form) {
  sync_pb::EntitySpecifics password_data;
  sync_pb::PasswordSpecificsData* password_specifics =
      password_data.mutable_password()->mutable_client_only_encrypted_data();
#define CopyField(field) password_specifics->set_##field(password_form.field)
#define CopyStringField(field) \
  password_specifics->set_##field(base::UTF16ToUTF8(password_form.field))
  CopyField(scheme);
  CopyField(signon_realm);
  password_specifics->set_origin(password_form.origin.spec());
  password_specifics->set_action(password_form.action.spec());
  CopyStringField(username_element);
  CopyStringField(password_element);
  CopyStringField(username_value);
  CopyStringField(password_value);
  CopyField(ssl_valid);
  CopyField(preferred);
  password_specifics->set_date_created(
      password_form.date_created.ToInternalValue());
  password_specifics->set_blacklisted(password_form.blacklisted_by_user);
  CopyField(type);
  CopyField(times_used);
  CopyStringField(display_name);
  password_specifics->set_avatar_url(password_form.avatar_url.spec());
  password_specifics->set_federation_url(password_form.federation_url.spec());
#undef CopyStringField
#undef CopyField

  std::string tag = MakePasswordSyncTag(*password_specifics);
  return syncer::SyncData::CreateLocalData(tag, tag, password_data);
}

autofill::PasswordForm PasswordFromSpecifics(
    const sync_pb::PasswordSpecificsData& password) {
  autofill::PasswordForm new_password;
  new_password.scheme =
      static_cast<autofill::PasswordForm::Scheme>(password.scheme());
  new_password.signon_realm = password.signon_realm();
  new_password.origin = GURL(password.origin());
  new_password.action = GURL(password.action());
  new_password.username_element =
      base::UTF8ToUTF16(password.username_element());
  new_password.password_element =
      base::UTF8ToUTF16(password.password_element());
  new_password.username_value = base::UTF8ToUTF16(password.username_value());
  new_password.password_value = base::UTF8ToUTF16(password.password_value());
  new_password.ssl_valid = password.ssl_valid();
  new_password.preferred = password.preferred();
  new_password.date_created =
      base::Time::FromInternalValue(password.date_created());
  new_password.blacklisted_by_user = password.blacklisted();
  new_password.type =
      static_cast<autofill::PasswordForm::Type>(password.type());
  new_password.times_used = password.times_used();
  new_password.display_name = base::UTF8ToUTF16(password.display_name());
  new_password.avatar_url = GURL(password.avatar_url());
  new_password.federation_url = GURL(password.federation_url());
  return new_password;
}

std::string MakePasswordSyncTag(
    const sync_pb::PasswordSpecificsData& password) {
  return MakePasswordSyncTag(PasswordFromSpecifics(password));
}

std::string MakePasswordSyncTag(const autofill::PasswordForm& password) {
  return (net::EscapePath(password.origin.spec()) + "|" +
          net::EscapePath(base::UTF16ToUTF8(password.username_element)) + "|" +
          net::EscapePath(base::UTF16ToUTF8(password.username_value)) + "|" +
          net::EscapePath(base::UTF16ToUTF8(password.password_element)) + "|" +
          net::EscapePath(password.signon_realm));
}

}  // namespace password_manager
