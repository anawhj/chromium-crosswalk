// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/password_manager/save_password_infobar_delegate.h"

#include "base/metrics/histogram.h"
#include "chrome/browser/infobars/infobar_service.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/sync/profile_sync_service.h"
#include "chrome/browser/sync/profile_sync_service_factory.h"
#include "chrome/grit/chromium_strings.h"
#include "chrome/grit/generated_resources.h"
#include "components/infobars/core/infobar.h"
#include "components/password_manager/core/browser/password_bubble_experiment.h"
#include "components/password_manager/core/browser/password_manager_client.h"
#include "content/public/browser/web_contents.h"
#include "grit/theme_resources.h"
#include "ui/base/l10n/l10n_util.h"

#if defined(OS_ANDROID)
#include "chrome/browser/ui/android/infobars/save_password_infobar.h"
#endif

#if defined(OS_MACOSX)
#import "chrome/browser/ui/cocoa/infobars/save_password_infobar_controller.h"
#endif

namespace {

int GetCancelButtonText(password_manager::CredentialSourceType source_type) {
  return source_type ==
      password_manager::CredentialSourceType::CREDENTIAL_SOURCE_API
      ? IDS_PASSWORD_MANAGER_SAVE_PASSWORD_SMART_LOCK_NO_THANKS_BUTTON
      : IDS_PASSWORD_MANAGER_BLACKLIST_BUTTON;
}

}  // namespace

// static
void SavePasswordInfoBarDelegate::Create(
    content::WebContents* web_contents,
    scoped_ptr<password_manager::PasswordFormManager> form_to_save,
    const std::string& uma_histogram_suffix,
    password_manager::CredentialSourceType source_type) {
  InfoBarService* infobar_service =
      InfoBarService::FromWebContents(web_contents);
  Profile* profile =
      Profile::FromBrowserContext(web_contents->GetBrowserContext());
  const ProfileSyncService* sync_service =
      ProfileSyncServiceFactory::GetForProfile(profile);
  SavePasswordInfoBarDelegate* infobar_delegate =
      new SavePasswordInfoBarDelegate(
          form_to_save.Pass(), uma_histogram_suffix, source_type,
          password_bubble_experiment::IsSmartLockBrandingEnabled(sync_service));
#if defined(OS_ANDROID)
  // For Android in case of smart lock we need different appearance of infobar.
  scoped_ptr<infobars::InfoBar> infobar =
      make_scoped_ptr(new SavePasswordInfoBar(
          scoped_ptr<SavePasswordInfoBarDelegate>(infobar_delegate)));
#elif defined(OS_MACOSX)
  scoped_ptr<infobars::InfoBar> infobar(
      CreateSavePasswordInfoBar(make_scoped_ptr(infobar_delegate)));
#else
  // For desktop we'll keep using the ConfirmInfobar.
  scoped_ptr<infobars::InfoBar> infobar(infobar_service->CreateConfirmInfoBar(
      scoped_ptr<ConfirmInfoBarDelegate>(infobar_delegate)));
#endif
  infobar_service->AddInfoBar(infobar.Pass());
}

SavePasswordInfoBarDelegate::~SavePasswordInfoBarDelegate() {
  UMA_HISTOGRAM_ENUMERATION("PasswordManager.InfoBarResponse",
                            infobar_response_,
                            password_manager::metrics_util::NUM_RESPONSE_TYPES);

  password_manager::metrics_util::LogUIDismissalReason(infobar_response_);

  // The shortest period for which the prompt needs to live, so that we don't
  // consider it killed prematurely, as might happen, e.g., if a pre-rendered
  // page gets swapped in (and the current WebContents is destroyed).
  const base::TimeDelta kMinimumPromptDisplayTime =
      base::TimeDelta::FromSeconds(1);

  if (!uma_histogram_suffix_.empty()) {
    password_manager::metrics_util::LogUMAHistogramEnumeration(
        "PasswordManager.SavePasswordPromptResponse_" + uma_histogram_suffix_,
        infobar_response_,
        password_manager::metrics_util::NUM_RESPONSE_TYPES);
    password_manager::metrics_util::LogUMAHistogramBoolean(
        "PasswordManager.SavePasswordPromptDisappearedQuickly_" +
            uma_histogram_suffix_,
        timer_.Elapsed() < kMinimumPromptDisplayTime);
  }
}

SavePasswordInfoBarDelegate::SavePasswordInfoBarDelegate(
    scoped_ptr<password_manager::PasswordFormManager> form_to_save,
    const std::string& uma_histogram_suffix,
    password_manager::CredentialSourceType source_type,
    bool is_smartlock_branding_enabled)
    : ConfirmInfoBarDelegate(),
      form_to_save_(form_to_save.Pass()),
      infobar_response_(password_manager::metrics_util::NO_RESPONSE),
      uma_histogram_suffix_(uma_histogram_suffix),
      source_type_(source_type) {
  if (!uma_histogram_suffix_.empty()) {
    password_manager::metrics_util::LogUMAHistogramBoolean(
        "PasswordManager.SavePasswordPromptDisplayed_" + uma_histogram_suffix_,
        true);
  }
  title_link_range_ = gfx::Range();
  if (is_smartlock_branding_enabled) {
    size_t offset = 0;
    base::string16 title_link =
        l10n_util::GetStringUTF16(IDS_PASSWORD_MANAGER_SMART_LOCK);
    title_ = l10n_util::GetStringFUTF16(IDS_SAVE_PASSWORD, title_link, &offset);
    title_link_range_ = gfx::Range(offset, offset + title_link.length());
  } else {
    title_ = l10n_util::GetStringFUTF16(
        IDS_SAVE_PASSWORD,
        l10n_util::GetStringUTF16(IDS_SAVE_PASSWORD_TITLE_BRAND));
  }
}

bool SavePasswordInfoBarDelegate::ShouldShowMoreButton() {
  return source_type_ ==
         password_manager::CredentialSourceType::CREDENTIAL_SOURCE_API;
}

infobars::InfoBarDelegate::Type
SavePasswordInfoBarDelegate::GetInfoBarType() const {
  return PAGE_ACTION_TYPE;
}

infobars::InfoBarDelegate::InfoBarAutomationType
SavePasswordInfoBarDelegate::GetInfoBarAutomationType() const {
  return PASSWORD_INFOBAR;
}

int SavePasswordInfoBarDelegate::GetIconID() const {
  return IDR_INFOBAR_SAVE_PASSWORD;
}

bool SavePasswordInfoBarDelegate::ShouldExpire(
    const NavigationDetails& details) const {
  return !details.is_redirect &&
         infobars::InfoBarDelegate::ShouldExpire(details);
}

void SavePasswordInfoBarDelegate::InfoBarDismissed() {
  DCHECK(form_to_save_.get());
  infobar_response_ = password_manager::metrics_util::INFOBAR_DISMISSED;
}

base::string16 SavePasswordInfoBarDelegate::GetMessageText() const {
  return title_;
}

base::string16 SavePasswordInfoBarDelegate::GetButtonLabel(
    InfoBarButton button) const {
  return l10n_util::GetStringUTF16((button == BUTTON_OK)
                                       ? IDS_PASSWORD_MANAGER_SAVE_BUTTON
                                       : GetCancelButtonText(source_type_));
}

bool SavePasswordInfoBarDelegate::Accept() {
  DCHECK(form_to_save_.get());
  form_to_save_->Save();
  infobar_response_ = password_manager::metrics_util::REMEMBER_PASSWORD;
  return true;
}

bool SavePasswordInfoBarDelegate::Cancel() {
  DCHECK(form_to_save_.get());
  if (source_type_ ==
      password_manager::CredentialSourceType::CREDENTIAL_SOURCE_API) {
    InfoBarDismissed();
  } else {
    form_to_save_->PermanentlyBlacklist();
    infobar_response_ = password_manager::metrics_util::NEVER_REMEMBER_PASSWORD;
  }
  return true;
}

bool SavePasswordInfoBarDelegate::LinkClicked(
    WindowOpenDisposition disposition) {
  InfoBarService::WebContentsFromInfoBar(infobar())
      ->OpenURL(content::OpenURLParams(
          GURL(l10n_util::GetStringUTF16(
              IDS_PASSWORD_MANAGER_SMART_LOCK_ARTICLE)),
          content::Referrer(),
          (disposition == CURRENT_TAB) ? NEW_FOREGROUND_TAB : disposition,
          ui::PAGE_TRANSITION_LINK, false));
  return true;
}
