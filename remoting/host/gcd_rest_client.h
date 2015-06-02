// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef REMOTING_HOST_GCD_REST_CLIENT_H_
#define REMOTING_HOST_GCD_REST_CLIENT_H_

#include <queue>

#include "base/basictypes.h"
#include "base/callback.h"
#include "base/memory/scoped_ptr.h"
#include "base/memory/weak_ptr.h"
#include "base/time/clock.h"
#include "net/url_request/url_fetcher_delegate.h"
#include "net/url_request/url_fetcher_factory.h"
#include "remoting/base/url_request_context_getter.h"
#include "remoting/host/oauth_token_getter.h"

namespace base {
class DictionaryValue;
}  // namespace base

namespace remoting {

// A client for calls to the GCD REST API.
class GcdRestClient : public net::URLFetcherDelegate {
 public:
  // Result of a GCD call.
  enum Status {
    SUCCESS,
    NETWORK_ERROR,
    NO_SUCH_HOST,
    OTHER_ERROR,
  };

  typedef base::Callback<void(Status status)> PatchStateCallback;

  // Note: |token_getter|, |url_fetcher_factory|, and |clock| must
  // outlive this object.
  GcdRestClient(const std::string& gcd_base_url,
                const std::string& gcd_device_id,
                const scoped_refptr<net::URLRequestContextGetter>&
                    url_request_context_getter,
                OAuthTokenGetter* token_getter);

  ~GcdRestClient() override;

  // Sends a 'patchState' request to the GCD API.  Constructs and
  // sends an appropriate JSON message M where |patch_details| becomes
  // the value of M.patches[0].patch.
  void PatchState(scoped_ptr<base::DictionaryValue> patch_details,
                  const GcdRestClient::PatchStateCallback& callback);

  void SetClockForTest(scoped_ptr<base::Clock> clock) { clock_ = clock.Pass(); }

 private:
  struct PatchStateRequest;

  void StartNextRequest();
  void OnTokenReceived(OAuthTokenGetter::Status status,
                       const std::string& user_email,
                       const std::string& access_token);
  void FinishCurrentRequest(Status result);

  // URLFetcherDelegate interface.
  void OnURLFetchComplete(const net::URLFetcher* source) override;

  std::string gcd_base_url_;
  std::string gcd_device_id_;
  scoped_refptr<net::URLRequestContextGetter> url_request_context_getter_;
  OAuthTokenGetter* token_getter_;
  scoped_ptr<base::Clock> clock_;
  scoped_ptr<PatchStateRequest> current_request_;

  // Ideally this queue would contain instances of scoped_ptr, but the
  // Mac C++ compiler doesn't like that.
  std::queue<PatchStateRequest*> pending_requests_;

  base::WeakPtrFactory<GcdRestClient> weak_factory_;

  DISALLOW_COPY_AND_ASSIGN(GcdRestClient);
};

}  // namespace remoting

#endif  // REMOTING_HOST_GCD_REST_CLIENT_H_
