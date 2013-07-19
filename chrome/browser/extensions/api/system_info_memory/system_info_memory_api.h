// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_EXTENSIONS_API_SYSTEM_INFO_MEMORY_SYSTEM_INFO_MEMORY_API_H_
#define CHROME_BROWSER_EXTENSIONS_API_SYSTEM_INFO_MEMORY_SYSTEM_INFO_MEMORY_API_H_

#include "chrome/browser/extensions/extension_function.h"
#include "chrome/common/extensions/api/system_info_memory.h"

namespace extensions {

class SystemInfoMemoryGetFunction : public AsyncExtensionFunction {
 public:
  DECLARE_EXTENSION_FUNCTION("systemInfo.memory.get", SYSTEMINFO_MEMORY_GET)
  SystemInfoMemoryGetFunction();

 private:
  virtual ~SystemInfoMemoryGetFunction();
  virtual bool RunImpl() OVERRIDE;
  void OnGetMemoryInfoCompleted(bool success);
};

}  // namespace extensions

#endif  // CHROME_BROWSER_EXTENSIONS_API_SYSTEM_INFO_MEMORY_SYSTEM_INFO_MEMORY_API_H__
