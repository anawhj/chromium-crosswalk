// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_RENDERER_P2P_IPC_SOCKET_FACTORY_H_
#define CONTENT_RENDERER_P2P_IPC_SOCKET_FACTORY_H_

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "content/common/content_export.h"
#include "third_party/libjingle/source/talk/p2p/base/packetsocketfactory.h"

namespace content {

class P2PSocketDispatcher;

// IpcPacketSocketFactory implements rtc::PacketSocketFactory
// interface for libjingle using IPC-based P2P sockets. The class must
// be used on a thread that is a libjingle thread (implements
// rtc::Thread) and also has associated base::MessageLoop. Each
// socket created by the factory must be used on the thread it was
// created on.
class IpcPacketSocketFactory : public rtc::PacketSocketFactory {
 public:
  CONTENT_EXPORT explicit IpcPacketSocketFactory(
      P2PSocketDispatcher* socket_dispatcher);
  virtual ~IpcPacketSocketFactory();

  virtual rtc::AsyncPacketSocket* CreateUdpSocket(
      const rtc::SocketAddress& local_address,
      int min_port, int max_port) override;
  virtual rtc::AsyncPacketSocket* CreateServerTcpSocket(
      const rtc::SocketAddress& local_address,
      int min_port,
      int max_port,
      int opts) override;
  virtual rtc::AsyncPacketSocket* CreateClientTcpSocket(
      const rtc::SocketAddress& local_address,
      const rtc::SocketAddress& remote_address,
      const rtc::ProxyInfo& proxy_info,
      const std::string& user_agent,
      int opts) override;
  virtual rtc::AsyncResolverInterface* CreateAsyncResolver() override;

 private:
  P2PSocketDispatcher* socket_dispatcher_;

  DISALLOW_COPY_AND_ASSIGN(IpcPacketSocketFactory);
};

}  // namespace content

#endif  // CONTENT_RENDERER_P2P_IPC_SOCKET_FACTORY_H_
