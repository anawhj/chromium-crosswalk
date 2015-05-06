// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/nacl/browser/nacl_host_message_filter.h"

#include "base/sys_info.h"
#include "components/nacl/browser/nacl_browser.h"
#include "components/nacl/browser/nacl_file_host.h"
#include "components/nacl/browser/nacl_process_host.h"
#include "components/nacl/browser/pnacl_host.h"
#include "components/nacl/common/nacl_host_messages.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/plugin_service.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/browser/web_contents.h"
#include "ipc/ipc_message_attachment_set.h"
#include "ipc/ipc_platform_file.h"
#include "net/url_request/url_request_context.h"
#include "net/url_request/url_request_context_getter.h"
#include "ppapi/shared_impl/ppapi_permissions.h"
#include "url/gurl.h"

namespace nacl {

namespace {

// The maximum number of resource file handles NaClProcessMsg_Start message
// can have. Currently IPC::MessageAttachmentSet::kMaxDescriptorsPerMessage
// is 7 and NaCl sends 5 handles for other purposes, hence 2.
// TODO(yusukes): Remove |kMaxPreOpenResourceFiles|. Instead, send an arbitrary
// number of FDs to the NaCl loader process with separate IPC messages.
const size_t kMaxPreOpenResourceFiles = 2;

#if defined(OS_POSIX)
static_assert(kMaxPreOpenResourceFiles ==
              IPC::MessageAttachmentSet::kMaxDescriptorsPerMessage - 5,
              "kMaxPreOpenResourceFiles is not up to date");
#endif

ppapi::PpapiPermissions GetNaClPermissions(
    uint32 permission_bits,
    content::BrowserContext* browser_context,
    const GURL& document_url) {
  // Only allow NaCl plugins to request certain permissions. We don't want
  // a compromised renderer to be able to start a nacl plugin with e.g. Flash
  // permissions which may expand the surface area of the sandbox.
  uint32 masked_bits = permission_bits & ppapi::PERMISSION_DEV;
  if (content::PluginService::GetInstance()->PpapiDevChannelSupported(
          browser_context, document_url))
    masked_bits |= ppapi::PERMISSION_DEV_CHANNEL;
  return ppapi::PpapiPermissions::GetForCommandLine(masked_bits);
}


ppapi::PpapiPermissions GetPpapiPermissions(uint32 permission_bits,
                                            int render_process_id,
                                            int render_view_id) {
  // We get the URL from WebContents from the RenderViewHost, since we don't
  // have a BrowserPpapiHost yet.
  content::RenderProcessHost* host =
      content::RenderProcessHost::FromID(render_process_id);
  content::RenderViewHost* view_host =
      content::RenderViewHost::FromID(render_process_id, render_view_id);
  if (!view_host)
    return ppapi::PpapiPermissions();
  GURL document_url;
  content::WebContents* contents =
      content::WebContents::FromRenderViewHost(view_host);
  if (contents)
    document_url = contents->GetLastCommittedURL();
  return GetNaClPermissions(permission_bits,
                            host->GetBrowserContext(),
                            document_url);
}

}  // namespace

NaClHostMessageFilter::NaClHostMessageFilter(
    int render_process_id,
    bool is_off_the_record,
    const base::FilePath& profile_directory,
    net::URLRequestContextGetter* request_context)
    : BrowserMessageFilter(NaClHostMsgStart),
      render_process_id_(render_process_id),
      off_the_record_(is_off_the_record),
      profile_directory_(profile_directory),
      request_context_(request_context),
      weak_ptr_factory_(this) {
}

NaClHostMessageFilter::~NaClHostMessageFilter() {
}

void NaClHostMessageFilter::OnChannelClosing() {
  pnacl::PnaclHost::GetInstance()->RendererClosing(render_process_id_);
}

bool NaClHostMessageFilter::OnMessageReceived(const IPC::Message& message) {
  bool handled = true;
  IPC_BEGIN_MESSAGE_MAP(NaClHostMessageFilter, message)
#if !defined(DISABLE_NACL)
    IPC_MESSAGE_HANDLER_DELAY_REPLY(NaClHostMsg_LaunchNaCl, OnLaunchNaCl)
    IPC_MESSAGE_HANDLER_DELAY_REPLY(NaClHostMsg_GetReadonlyPnaclFD,
                                    OnGetReadonlyPnaclFd)
    IPC_MESSAGE_HANDLER_DELAY_REPLY(NaClHostMsg_NaClCreateTemporaryFile,
                                    OnNaClCreateTemporaryFile)
    IPC_MESSAGE_HANDLER(NaClHostMsg_NexeTempFileRequest,
                        OnGetNexeFd)
    IPC_MESSAGE_HANDLER(NaClHostMsg_ReportTranslationFinished,
                        OnTranslationFinished)
    IPC_MESSAGE_HANDLER(NaClHostMsg_MissingArchError,
                        OnMissingArchError)
    IPC_MESSAGE_HANDLER_DELAY_REPLY(NaClHostMsg_OpenNaClExecutable,
                                    OnOpenNaClExecutable)
    IPC_MESSAGE_HANDLER(NaClHostMsg_NaClGetNumProcessors,
                        OnNaClGetNumProcessors)
    IPC_MESSAGE_HANDLER(NaClHostMsg_NaClDebugEnabledForURL,
                        OnNaClDebugEnabledForURL)
#endif
    IPC_MESSAGE_UNHANDLED(handled = false)
  IPC_END_MESSAGE_MAP()

  return handled;
}

net::HostResolver* NaClHostMessageFilter::GetHostResolver() {
  return request_context_->GetURLRequestContext()->host_resolver();
}

void NaClHostMessageFilter::OnLaunchNaCl(
    const nacl::NaClLaunchParams& launch_params,
    IPC::Message* reply_msg) {
  // If we're running llc or ld for the PNaCl translator, we don't need to look
  // up permissions, and we don't have the right browser state to look up some
  // of the whitelisting parameters anyway.
  if (launch_params.process_type == kPNaClTranslatorProcessType) {
    uint32 perms = launch_params.permission_bits & ppapi::PERMISSION_DEV;
    LaunchNaClContinuationOnIOThread(
        launch_params,
        reply_msg,
        std::vector<NaClResourcePrefetchResult>(),
        ppapi::PpapiPermissions(perms));
    return;
  }
  content::BrowserThread::PostTask(
      content::BrowserThread::UI,
      FROM_HERE,
      base::Bind(&NaClHostMessageFilter::LaunchNaClContinuation,
                 this,
                 launch_params,
                 reply_msg));
}

void NaClHostMessageFilter::LaunchNaClContinuation(
    const nacl::NaClLaunchParams& launch_params,
    IPC::Message* reply_msg) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);

  ppapi::PpapiPermissions permissions =
      GetPpapiPermissions(launch_params.permission_bits,
                          render_process_id_,
                          launch_params.render_view_id);

  content::RenderViewHost* rvh = content::RenderViewHost::FromID(
      render_process_id(), launch_params.render_view_id);
  if (!rvh) {
    BadMessageReceived();  // Kill the renderer.
    return;
  }

  nacl::NaClLaunchParams safe_launch_params(launch_params);
  safe_launch_params.resource_prefetch_request_list.clear();

  // TODO(yusukes): Fix NaClProcessHost::~NaClProcessHost() and remove the
  // ifdef.
#if !defined(OS_WIN)
  const std::vector<NaClResourcePrefetchRequest>& original_request_list =
      launch_params.resource_prefetch_request_list;
  content::SiteInstance* site_instance = rvh->GetSiteInstance();
  for (size_t i = 0; i < original_request_list.size(); ++i) {
    GURL gurl(original_request_list[i].resource_url);
    // Important security check: Do the same check as OpenNaClExecutable()
    // in nacl_file_host.cc.
    if (!content::SiteInstance::IsSameWebSite(
            site_instance->GetBrowserContext(),
            site_instance->GetSiteURL(),
            gurl)) {
      continue;
    }
    safe_launch_params.resource_prefetch_request_list.push_back(
        original_request_list[i]);
  }
#endif

  // Process a list of resource file URLs in
  // |launch_params.resource_files_to_prefetch|.
  content::BrowserThread::PostBlockingPoolTask(
      FROM_HERE,
      base::Bind(&NaClHostMessageFilter::BatchOpenResourceFiles,
                 this,
                 safe_launch_params,
                 reply_msg,
                 permissions));
}

void NaClHostMessageFilter::BatchOpenResourceFiles(
    const nacl::NaClLaunchParams& launch_params,
    IPC::Message* reply_msg,
    ppapi::PpapiPermissions permissions) {
  std::vector<NaClResourcePrefetchResult> prefetched_resource_files;
  const std::vector<NaClResourcePrefetchRequest>& request_list =
      launch_params.resource_prefetch_request_list;
  for (size_t i = 0; i < request_list.size(); ++i) {
    GURL gurl(request_list[i].resource_url);
    base::FilePath file_path_metadata;
    if (!nacl::NaClBrowser::GetDelegate()->MapUrlToLocalFilePath(
            gurl,
            true,  // use_blocking_api
            profile_directory_,
            &file_path_metadata)) {
      continue;
    }
    base::File file = nacl::OpenNaClReadExecImpl(
        file_path_metadata, true /* is_executable */);
    if (!file.IsValid())
      continue;

    prefetched_resource_files.push_back(NaClResourcePrefetchResult(
        IPC::TakeFileHandleForProcess(file.Pass(), PeerHandle()),
        file_path_metadata,
        request_list[i].file_key));

    if (prefetched_resource_files.size() >= kMaxPreOpenResourceFiles)
      break;
  }

  content::BrowserThread::PostTask(
      content::BrowserThread::IO,
      FROM_HERE,
      base::Bind(&NaClHostMessageFilter::LaunchNaClContinuationOnIOThread,
                 this,
                 launch_params,
                 reply_msg,
                 prefetched_resource_files,
                 permissions));
}

void NaClHostMessageFilter::LaunchNaClContinuationOnIOThread(
    const nacl::NaClLaunchParams& launch_params,
    IPC::Message* reply_msg,
    const std::vector<NaClResourcePrefetchResult>& prefetched_resource_files,
    ppapi::PpapiPermissions permissions) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::IO);

  NaClFileToken nexe_token = {
      launch_params.nexe_token_lo,  // lo
      launch_params.nexe_token_hi   // hi
  };

  base::PlatformFile nexe_file;
#if defined(OS_WIN)
  // Duplicate the nexe file handle from the renderer process into the browser
  // process.
  if (!::DuplicateHandle(PeerHandle(),
                         launch_params.nexe_file,
                         base::GetCurrentProcessHandle(),
                         &nexe_file,
                         0,  // Unused, given DUPLICATE_SAME_ACCESS.
                         FALSE,
                         DUPLICATE_CLOSE_SOURCE | DUPLICATE_SAME_ACCESS)) {
    NaClHostMsg_LaunchNaCl::WriteReplyParams(
        reply_msg,
        NaClLaunchResult(),
        std::string("Failed to duplicate nexe file handle"));
    Send(reply_msg);
    return;
  }
#elif defined(OS_POSIX)
  nexe_file =
      IPC::PlatformFileForTransitToPlatformFile(launch_params.nexe_file);
#else
#error Unsupported platform.
#endif

  NaClProcessHost* host = new NaClProcessHost(
      GURL(launch_params.manifest_url),
      base::File(nexe_file),
      nexe_token,
      prefetched_resource_files,
      permissions,
      launch_params.render_view_id,
      launch_params.permission_bits,
      launch_params.uses_nonsfi_mode,
      off_the_record_,
      launch_params.process_type,
      profile_directory_);
  GURL manifest_url(launch_params.manifest_url);
  base::FilePath manifest_path;
  // We're calling MapUrlToLocalFilePath with the non-blocking API
  // because we're running in the I/O thread. Ideally we'd use the other path,
  // which would cover more cases.
  nacl::NaClBrowser::GetDelegate()->MapUrlToLocalFilePath(
      manifest_url,
      false /* use_blocking_api */,
      profile_directory_,
      &manifest_path);
  host->Launch(this, reply_msg, manifest_path);
}

void NaClHostMessageFilter::OnGetReadonlyPnaclFd(
    const std::string& filename, bool is_executable, IPC::Message* reply_msg) {
  // This posts a task to another thread, but the renderer will
  // block until the reply is sent.
  nacl_file_host::GetReadonlyPnaclFd(this, filename, is_executable, reply_msg);

  // This is the first message we receive from the renderer once it knows we
  // want to use PNaCl, so start the translation cache initialization here.
  pnacl::PnaclHost::GetInstance()->Init();
}

// Return the temporary file via a reply to the
// NaClHostMsg_NaClCreateTemporaryFile sync message.
void NaClHostMessageFilter::SyncReturnTemporaryFile(
    IPC::Message* reply_msg,
    base::File file) {
  if (file.IsValid()) {
    NaClHostMsg_NaClCreateTemporaryFile::WriteReplyParams(
        reply_msg,
        IPC::TakeFileHandleForProcess(file.Pass(), PeerHandle()));
  } else {
    reply_msg->set_reply_error();
  }
  Send(reply_msg);
}

void NaClHostMessageFilter::OnNaClCreateTemporaryFile(
    IPC::Message* reply_msg) {
  pnacl::PnaclHost::GetInstance()->CreateTemporaryFile(
      base::Bind(&NaClHostMessageFilter::SyncReturnTemporaryFile,
                 this,
                 reply_msg));
}

void NaClHostMessageFilter::AsyncReturnTemporaryFile(
    int pp_instance,
    const base::File& file,
    bool is_hit) {
  IPC::PlatformFileForTransit fd = IPC::InvalidPlatformFileForTransit();
  if (file.IsValid()) {
    // Don't close our copy of the handle, because PnaclHost will use it
    // when the translation finishes.
    fd = IPC::GetFileHandleForProcess(file.GetPlatformFile(), PeerHandle(),
                                      false);
  }
  Send(new NaClViewMsg_NexeTempFileReply(pp_instance, is_hit, fd));
}

void NaClHostMessageFilter::OnNaClGetNumProcessors(int* num_processors) {
  *num_processors = base::SysInfo::NumberOfProcessors();
}

void NaClHostMessageFilter::OnGetNexeFd(
    int render_view_id,
    int pp_instance,
    const nacl::PnaclCacheInfo& cache_info) {
  if (!cache_info.pexe_url.is_valid()) {
    LOG(ERROR) << "Bad URL received from GetNexeFd: " <<
        cache_info.pexe_url.possibly_invalid_spec();
    BadMessageReceived();
    return;
  }

  pnacl::PnaclHost::GetInstance()->GetNexeFd(
      render_process_id_,
      render_view_id,
      pp_instance,
      off_the_record_,
      cache_info,
      base::Bind(&NaClHostMessageFilter::AsyncReturnTemporaryFile,
                 this,
                 pp_instance));
}

void NaClHostMessageFilter::OnTranslationFinished(int instance, bool success) {
  pnacl::PnaclHost::GetInstance()->TranslationFinished(
      render_process_id_, instance, success);
}

void NaClHostMessageFilter::OnMissingArchError(int render_view_id) {
  nacl::NaClBrowser::GetDelegate()->
      ShowMissingArchInfobar(render_process_id_, render_view_id);
}

void NaClHostMessageFilter::OnOpenNaClExecutable(
    int render_view_id,
    const GURL& file_url,
    bool enable_validation_caching,
    IPC::Message* reply_msg) {
  nacl_file_host::OpenNaClExecutable(this,
                                     render_view_id,
                                     file_url,
                                     enable_validation_caching,
                                     reply_msg);
}

void NaClHostMessageFilter::OnNaClDebugEnabledForURL(const GURL& nmf_url,
                                                     bool* should_debug) {
  *should_debug =
      nacl::NaClBrowser::GetDelegate()->URLMatchesDebugPatterns(nmf_url);
}

}  // namespace nacl
