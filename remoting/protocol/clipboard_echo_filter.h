// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef REMOTING_PROTOCOL_CLIPBOARD_ECHO_FILTER_H_
#define REMOTING_PROTOCOL_CLIPBOARD_ECHO_FILTER_H_

#include <map>
#include <string>

#include "base/compiler_specific.h"
#include "remoting/protocol/clipboard_stub.h"

namespace remoting {
namespace protocol {

// ClipboardEchoFilter stops the host sending a clipboard item to the
// client, if that item was the latest item received from the client.
class ClipboardEchoFilter {
 public:
  ClipboardEchoFilter();
  ~ClipboardEchoFilter();

  // Sets the ClipboardStub that sends events to the client.
  void set_client_stub(ClipboardStub* client_stub);

  // Sets the ClipboardStub that sends events to the host.
  void set_host_stub(ClipboardStub* host_stub);

  // Gets the ClipboardStub that sends events through this filter and on to the
  // client.
  ClipboardStub* client_filter();

  // Gets the ClipboardStub that sends events through this filter and on to the
  // host.
  ClipboardStub* host_filter();

 private:
  class ClientFilter : public ClipboardStub {
   public:
    ClientFilter(ClipboardEchoFilter* filter);
    virtual void InjectClipboardEvent(const ClipboardEvent& event) OVERRIDE;

   private:
    ClipboardEchoFilter* filter_;
  };

  class HostFilter : public ClipboardStub {
   public:
    HostFilter(ClipboardEchoFilter* filter);
    virtual void InjectClipboardEvent(const ClipboardEvent& event) OVERRIDE;

   private:
    ClipboardEchoFilter* filter_;
  };

  void InjectClipboardEventToHost(const ClipboardEvent& event);
  void InjectClipboardEventToClient(const ClipboardEvent& event);

  ClipboardStub* host_stub_;
  ClipboardStub* client_stub_;
  ClientFilter client_filter_;
  HostFilter host_filter_;

  // The latest item received from the client.
  std::string client_latest_mime_type_;
  std::string client_latest_data_;

  DISALLOW_COPY_AND_ASSIGN(ClipboardEchoFilter);
};

}  // namespace protocol
}  // namespace remoting

#endif  // REMOTING_PROTOCOL_CLIPBOARD_ECHO_FILTER_H_
