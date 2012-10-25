// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gl/gl_egl_api_implementation.h"

namespace gfx {

RealEGLApi g_real_egl;

void InitializeGLBindingsEGL() {
  g_driver_egl.InitializeBindings();
  g_real_egl.Initialize(&g_driver_egl);
  g_current_egl_context = &g_real_egl;
}

void InitializeGLExtensionBindingsEGL(GLContext* context) {
  g_driver_egl.InitializeExtensionBindings(context);
}

void InitializeDebugGLBindingsEGL() {
  g_driver_egl.InitializeDebugBindings();
}

void ClearGLBindingsEGL() {
  g_driver_egl.ClearBindings();
}

EGLApi::EGLApi() {
}

EGLApi::~EGLApi() {
}

RealEGLApi::RealEGLApi() {
}

void RealEGLApi::Initialize(DriverEGL* driver) {
  driver_ = driver;
}

}  // namespace gfx


