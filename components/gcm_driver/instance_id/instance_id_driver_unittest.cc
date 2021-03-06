// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/gcm_driver/instance_id/instance_id_driver.h"

#include <cmath>
#include "base/bind.h"
#include "base/message_loop/message_loop.h"
#include "base/run_loop.h"
#include "base/strings/string_util.h"
#include "components/gcm_driver/instance_id/fake_gcm_driver_for_instance_id.h"
#include "components/gcm_driver/instance_id/instance_id.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace instance_id {

namespace {

const char kTestAppID1[] = "TestApp1";
const char kTestAppID2[] = "TestApp2";

bool VerifyInstanceID(const std::string& str) {
  // Checks the length.
  if (str.length() != static_cast<size_t>(
          std::ceil(InstanceID::kInstanceIDByteLength * 8 / 6.0)))
    return false;

  // Checks if it is URL-safe base64 encoded.
  for (auto ch : str) {
    if (!IsAsciiAlpha(ch) && !IsAsciiDigit(ch) && ch != '_' && ch != '-')
      return false;
  }
  return true;
}

}  // namespace

class InstanceIDDriverTest : public testing::Test {
 public:
  InstanceIDDriverTest();
  ~InstanceIDDriverTest() override;

  // testing::Test:
  void SetUp() override;

  void WaitForAsyncOperation();

  // Recreates InstanceIDDriver to simulate restart.
  void RecreateInstanceIDDriver();

  // Sync wrappers for async version.
  std::string GetID(InstanceID* instance_id);
  base::Time GetCreationTime(InstanceID* instance_id);
  InstanceID::Result DeleteID(InstanceID* instance_id);

  InstanceIDDriver* driver() const { return driver_.get(); }

 private:
  void GetIDCompleted(const std::string& id);
  void GetCreationTimeCompleted(const base::Time& creation_time);
  void DeleteIDCompleted(InstanceID::Result result);

  base::MessageLoopForUI message_loop_;
  scoped_ptr<FakeGCMDriverForInstanceID> gcm_driver_;
  scoped_ptr<InstanceIDDriver> driver_;

  std::string id_;
  base::Time creation_time_;
  InstanceID::Result result_;

  bool async_operation_completed_;
  base::Closure async_operation_completed_callback_;

  DISALLOW_COPY_AND_ASSIGN(InstanceIDDriverTest);
};

InstanceIDDriverTest::InstanceIDDriverTest()
    : result_(InstanceID::UNKNOWN_ERROR),
      async_operation_completed_(false) {
}

InstanceIDDriverTest::~InstanceIDDriverTest() {
}

void InstanceIDDriverTest::SetUp() {
  gcm_driver_.reset(new FakeGCMDriverForInstanceID);
  RecreateInstanceIDDriver();
}

void InstanceIDDriverTest::RecreateInstanceIDDriver() {
  driver_.reset(new InstanceIDDriver(gcm_driver_.get()));
}

void InstanceIDDriverTest::WaitForAsyncOperation() {
  // No need to wait if async operation is not needed.
  if (async_operation_completed_)
    return;
  base::RunLoop run_loop;
  async_operation_completed_callback_ = run_loop.QuitClosure();
  run_loop.Run();
}

std::string InstanceIDDriverTest::GetID(InstanceID* instance_id) {
  async_operation_completed_ = false;
  id_.clear();
  instance_id->GetID(base::Bind(&InstanceIDDriverTest::GetIDCompleted,
                     base::Unretained(this)));
  WaitForAsyncOperation();
  return id_;
}

base::Time InstanceIDDriverTest::GetCreationTime(InstanceID* instance_id) {
  async_operation_completed_ = false;
  creation_time_ = base::Time();
  instance_id->GetCreationTime(
      base::Bind(&InstanceIDDriverTest::GetCreationTimeCompleted,
                 base::Unretained(this)));
  WaitForAsyncOperation();
  return creation_time_;
}

InstanceID::Result InstanceIDDriverTest::DeleteID(InstanceID* instance_id) {
  async_operation_completed_ = false;
  result_ = InstanceID::UNKNOWN_ERROR;;
  instance_id->DeleteID(base::Bind(&InstanceIDDriverTest::DeleteIDCompleted,
                        base::Unretained(this)));
  WaitForAsyncOperation();
  return result_;
}

void InstanceIDDriverTest::GetIDCompleted(const std::string& id) {
  async_operation_completed_ = true;
  id_ = id;
  if (!async_operation_completed_callback_.is_null())
    async_operation_completed_callback_.Run();
}

void InstanceIDDriverTest::GetCreationTimeCompleted(
    const base::Time& creation_time) {
  async_operation_completed_ = true;
  creation_time_ = creation_time;
  if (!async_operation_completed_callback_.is_null())
    async_operation_completed_callback_.Run();
}

void InstanceIDDriverTest::DeleteIDCompleted(InstanceID::Result result) {
  async_operation_completed_ = true;
  result_ = result;
  if (!async_operation_completed_callback_.is_null())
    async_operation_completed_callback_.Run();
}

TEST_F(InstanceIDDriverTest, NewID) {
  // Creation time should not be set when the ID is not created.
  InstanceID* instance_id1 = driver()->GetInstanceID(kTestAppID1);
  EXPECT_TRUE(GetCreationTime(instance_id1).is_null());

  // New ID is generated for the first time.
  std::string id1 = GetID(instance_id1);
  EXPECT_TRUE(VerifyInstanceID(id1));
  base::Time creation_time = GetCreationTime(instance_id1);
  EXPECT_FALSE(creation_time.is_null());

  // Same ID is returned for the same app.
  EXPECT_EQ(id1, GetID(instance_id1));
  EXPECT_EQ(creation_time, GetCreationTime(instance_id1));

  // New ID is generated for another app.
  InstanceID* instance_id2 = driver()->GetInstanceID(kTestAppID2);
  std::string id2 = GetID(instance_id2);
  EXPECT_TRUE(VerifyInstanceID(id2));
  EXPECT_NE(id1, id2);
  EXPECT_FALSE(GetCreationTime(instance_id2).is_null());
}

TEST_F(InstanceIDDriverTest, PersistID) {
  InstanceID* instance_id = driver()->GetInstanceID(kTestAppID1);

  // Create the ID for the first time. The ID and creation time should be saved
  // to the store.
  std::string id = GetID(instance_id);
  EXPECT_FALSE(id.empty());
  base::Time creation_time = GetCreationTime(instance_id);
  EXPECT_FALSE(creation_time.is_null());

  // Simulate restart by recreating InstanceIDDriver. Same ID and creation time
  // should be expected.
  RecreateInstanceIDDriver();
  instance_id = driver()->GetInstanceID(kTestAppID1);
  EXPECT_EQ(creation_time, GetCreationTime(instance_id));
  EXPECT_EQ(id, GetID(instance_id));

  // Delete the ID. The ID and creation time should be removed from the store.
  EXPECT_EQ(InstanceID::SUCCESS, DeleteID(instance_id));
  EXPECT_TRUE(GetCreationTime(instance_id).is_null());

  // Simulate restart by recreating InstanceIDDriver. Different ID should be
  // expected.
  // Note that we do not check for different creation time since the test might
  // be run at a very fast server.
  RecreateInstanceIDDriver();
  instance_id = driver()->GetInstanceID(kTestAppID1);
  EXPECT_NE(id, GetID(instance_id));
}

TEST_F(InstanceIDDriverTest, DeleteID) {
  InstanceID* instance_id = driver()->GetInstanceID(kTestAppID1);
  std::string id1 = GetID(instance_id);
  EXPECT_FALSE(id1.empty());
  EXPECT_FALSE(GetCreationTime(instance_id).is_null());

  // New ID will be generated from GetID after calling DeleteID.
  EXPECT_EQ(InstanceID::SUCCESS, DeleteID(instance_id));
  EXPECT_TRUE(GetCreationTime(instance_id).is_null());

  std::string id2 = GetID(instance_id);
  EXPECT_FALSE(id2.empty());
  EXPECT_NE(id1, id2);
  EXPECT_FALSE(GetCreationTime(instance_id).is_null());
}

}  // instance_id
