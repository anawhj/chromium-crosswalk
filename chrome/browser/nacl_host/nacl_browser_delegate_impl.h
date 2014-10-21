// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_NACL_HOST_NACL_BROWSER_DELEGATE_IMPL_H_
#define CHROME_BROWSER_NACL_HOST_NACL_BROWSER_DELEGATE_IMPL_H_

#include <set>
#include <string>

#include "base/compiler_specific.h"
#include "components/nacl/browser/nacl_browser_delegate.h"
#include "extensions/common/url_pattern.h"

namespace extensions {
class InfoMap;
}

class ProfileManager;

class NaClBrowserDelegateImpl : public NaClBrowserDelegate {
 public:
  explicit NaClBrowserDelegateImpl(ProfileManager* profile_manager);
  ~NaClBrowserDelegateImpl() override;

  void ShowMissingArchInfobar(int render_process_id,
                              int render_view_id) override;
  bool DialogsAreSuppressed() override;
  bool GetCacheDirectory(base::FilePath* cache_dir) override;
  bool GetPluginDirectory(base::FilePath* plugin_dir) override;
  bool GetPnaclDirectory(base::FilePath* pnacl_dir) override;
  bool GetUserDirectory(base::FilePath* user_dir) override;
  std::string GetVersionString() const override;
  ppapi::host::HostFactory* CreatePpapiHostFactory(
      content::BrowserPpapiHost* ppapi_host) override;
  bool MapUrlToLocalFilePath(const GURL& url,
                             bool is_blocking,
                             const base::FilePath& profile_directory,
                             base::FilePath* file_path) override;
  void SetDebugPatterns(std::string debug_patterns) override;
  bool URLMatchesDebugPatterns(const GURL& manifest_url) override;
  content::BrowserPpapiHost::OnKeepaliveCallback GetOnKeepaliveCallback()
      override;
  bool IsNonSfiModeAllowed(const base::FilePath& profile_directory,
                           const GURL& manifest_url) override;

 private:
#if defined(ENABLE_EXTENSIONS)
  scoped_refptr<extensions::InfoMap> GetExtensionInfoMap(
      const base::FilePath& profile_directory);
#endif

  ProfileManager* profile_manager_;
  std::vector<URLPattern> debug_patterns_;
  bool inverse_debug_patterns_;
  std::set<std::string> allowed_nonsfi_origins_;
  DISALLOW_COPY_AND_ASSIGN(NaClBrowserDelegateImpl);
};


#endif  // CHROME_BROWSER_NACL_HOST_NACL_BROWSER_DELEGATE_IMPL_H_
