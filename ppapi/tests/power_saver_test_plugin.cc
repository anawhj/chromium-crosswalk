// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <algorithm>

#include "ppapi/cpp/graphics_2d.h"
#include "ppapi/cpp/image_data.h"
#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/module.h"
#include "ppapi/tests/test_utils.h"

// Windows defines 'PostMessage', so we have to undef it.
#ifdef PostMessage
#undef PostMessage
#endif

// This is a simple C++ Pepper plugin that enables Plugin Power Saver tests.
class PowerSaverTestInstance : public pp::Instance {
 public:
  explicit PowerSaverTestInstance(PP_Instance instance)
      : pp::Instance(instance), callback_factory_(this) {}
  ~PowerSaverTestInstance() override {}

  bool Init(uint32_t argc, const char* argn[], const char* argv[]) {
    GetTestingInterface()->SubscribeToPowerSaverNotifications(pp_instance());
    return true;
  }

  void HandleMessage(const pp::Var& message_data) override {
    if (message_data.is_string() &&
        message_data.AsString() == "getPowerSaverStatus") {
      GetTestingInterface()->PostPowerSaverStatus(pp_instance());
    }
  }

  // Broadcast our peripheral status after the initial view data. This is for
  // tests that await initial plugin creation.
  void DidChangeView(const pp::View& view) override {
    view_ = view;
    device_context_ = pp::Graphics2D(this, view_.GetRect().size(), true);
    if (!BindGraphics(device_context_))
      return;

    Paint();
  }

  void OnFlush(int32_t) { Paint(); }

 private:
  void Paint() {
    pp::ImageData image(this, PP_IMAGEDATAFORMAT_BGRA_PREMUL,
                        view_.GetRect().size(), true);
    if (image.is_null())
      return;

    // Draw black and white stripes to present an "interesting" keyframe.
    for (int y = 0; y < view_.GetRect().size().height(); ++y) {
      for (int x = 0; x < view_.GetRect().size().width(); ++x) {
        uint32_t color = x % 2 ? 0xFF0000FF : 0xFFFFFFFF;
        *image.GetAddr32(pp::Point(x, y)) = color;
      }
    }

    device_context_.ReplaceContents(&image);
    device_context_.Flush(
        callback_factory_.NewCallback(&PowerSaverTestInstance::OnFlush));
  }

  pp::View view_;
  pp::Graphics2D device_context_;

  pp::CompletionCallbackFactory<PowerSaverTestInstance> callback_factory_;
};

class PowerSaverTestModule : public pp::Module {
 public:
  PowerSaverTestModule() : pp::Module() {}
  virtual ~PowerSaverTestModule() {}

  virtual pp::Instance* CreateInstance(PP_Instance instance) {
    return new PowerSaverTestInstance(instance);
  }
};

namespace pp {

Module* CreateModule() {
  return new PowerSaverTestModule();
}

}  // namespace pp
