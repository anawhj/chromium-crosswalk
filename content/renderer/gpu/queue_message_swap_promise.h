// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_RENDERER_QUEUE_MESSAGE_SWAP_PROMISE_H_
#define CONTENT_RENDERER_QUEUE_MESSAGE_SWAP_PROMISE_H_

#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "cc/base/swap_promise.h"

namespace IPC {
class SyncMessageFilter;
}

namespace content {

class FrameSwapMessageQueue;

class QueueMessageSwapPromise : public cc::SwapPromise {
 public:
  QueueMessageSwapPromise(scoped_refptr<IPC::SyncMessageFilter> message_sender,
                          scoped_refptr<FrameSwapMessageQueue> message_queue,
                          int source_frame_number);

  virtual ~QueueMessageSwapPromise();

  virtual void DidSwap(cc::CompositorFrameMetadata* metadata) override;

  virtual void DidNotSwap(DidNotSwapReason reason) override;

  virtual int64 TraceId() const override;

 private:
  void PromiseCompleted();

  scoped_refptr<IPC::SyncMessageFilter> message_sender_;
  scoped_refptr<content::FrameSwapMessageQueue> message_queue_;
  int source_frame_number_;
#if DCHECK_IS_ON
  bool completed_;
#endif
};

}  // namespace content

#endif  // CONTENT_RENDERER_QUEUE_MESSAGE_SWAP_PROMISE_H_
