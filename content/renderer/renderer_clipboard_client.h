// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_RENDERER_RENDERER_CLIPBOARD_CLIENT_H_
#define CONTENT_RENDERER_RENDERER_CLIPBOARD_CLIENT_H_

#include "base/compiler_specific.h"
#include "content/renderer/clipboard_client.h"

namespace content {

// An implementation of ClipboardClient that gets and sends data over IPC.
class RendererClipboardClient : public ClipboardClient {
 public:
  RendererClipboardClient();
  virtual ~RendererClipboardClient();

  virtual ui::Clipboard* GetClipboard() override;
  virtual uint64 GetSequenceNumber(ui::ClipboardType type) override;
  virtual bool IsFormatAvailable(ClipboardFormat format,
                                 ui::ClipboardType type) override;
  virtual void Clear(ui::ClipboardType type) override;
  virtual void ReadAvailableTypes(ui::ClipboardType type,
                                  std::vector<base::string16>* types,
                                  bool* contains_filenames) override;
  virtual void ReadText(ui::ClipboardType type,
                        base::string16* result) override;
  virtual void ReadHTML(ui::ClipboardType type,
                        base::string16* markup,
                        GURL* url,
                        uint32* fragment_start,
                        uint32* fragment_end) override;
  virtual void ReadRTF(ui::ClipboardType type, std::string* result) override;
  virtual void ReadImage(ui::ClipboardType type, std::string* data) override;
  virtual void ReadCustomData(ui::ClipboardType clipboard_type,
                              const base::string16& type,
                              base::string16* data) override;
  virtual WriteContext* CreateWriteContext() override;
};

}  // namespace content

#endif  // CONTENT_RENDERER_RENDERER_CLIPBOARD_CLIENT_H_
