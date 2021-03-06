// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/view_manager/view_manager_app.h"
#include "mojo/application/application_runner_chromium.h"
#include "third_party/mojo/src/mojo/public/c/system/main.h"

MojoResult MojoMain(MojoHandle shell_handle) {
  mojo::ApplicationRunnerChromium runner(new view_manager::ViewManagerApp);
  return runner.Run(shell_handle);
}
