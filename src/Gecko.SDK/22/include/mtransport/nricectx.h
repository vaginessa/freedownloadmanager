/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

// Original author: ekr@rtfm.com

// Some of this code is cut-and-pasted from nICEr. Copyright is:

/*
Copyright (c) 2007, Adobe Systems, Incorporated
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

* Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.

* Neither the name of Adobe Systems, Network Resonance nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

// Original author: ekr@rtfm.com

// This is a wrapper around the nICEr ICE stack
#ifndef nricectx_h__
#define nricectx_h__

#include <vector>

#include "sigslot.h"

#include "prnetdb.h"

#include "mozilla/RefPtr.h"
#include "mozilla/Scoped.h"
#include "nsAutoPtr.h"
#include "nsIEventTarget.h"
#include "nsITimer.h"

#include "m_cpp_utils.h"

typedef struct nr_ice_ctx_ nr_ice_ctx;
typedef struct nr_ice_peer_ctx_ nr_ice_peer_ctx;
typedef struct nr_ice_media_stream_ nr_ice_media_stream;
typedef struct nr_ice_handler_ nr_ice_handler;
typedef struct nr_ice_handler_vtbl_ nr_ice_handler_vtbl;
typedef struct nr_ice_cand_pair_ nr_ice_cand_pair;
typedef struct nr_ice_stun_server_ nr_ice_stun_server;
typedef struct nr_resolver_ nr_resolver;

namespace mozilla {

typedef void* NR_SOCKET;

class NrIceMediaStream;

struct NrIceStunServer {
 public:
  NrIceStunServer(const PRNetAddr& addr) : has_addr_(true) {
    memcpy(&addr_, &addr, sizeof(addr));
  }

   // The main function to use. Will take either an address or a hostname.
  static NrIceStunServer* Create(const std::string& addr, uint16_t port) {
    ScopedDeletePtr<NrIceStunServer> server(
        new NrIceStunServer());

    nsresult rv = server->Init(addr, port);
    if (NS_FAILED(rv))
      return nullptr;

    return server.forget();
  }

  nsresult ToNicerStruct(nr_ice_stun_server *server) const;

 private:
  NrIceStunServer() : addr_() {}

  nsresult Init(const std::string& addr, uint16_t port) {
    PRStatus status = PR_StringToNetAddr(addr.c_str(), &addr_);
    if (status == PR_SUCCESS) {
      // Parseable as an address
      addr_.inet.port = PR_htons(port);
      port_ = port;
      has_addr_ = true;
      return NS_OK;
    }
    else if (addr.size() < 256) {
      // Apparently this is a hostname.
      host_ = addr;
      port_ = port;
      has_addr_ = false;
      return NS_OK;
    }

    return NS_ERROR_FAILURE;
  }

  bool has_addr_;
  std::string host_;
  uint16_t port_;
  PRNetAddr addr_;
};

struct NrIceTurnServer {
  PRNetAddr addr;
  std::string username;
  std::string password;
};

class NrIceCtx {
 public:
  enum State { ICE_CTX_INIT,
               ICE_CTX_GATHERING,
               ICE_CTX_GATHERED,
               ICE_CTX_CHECKING,
               ICE_CTX_OPEN,
               ICE_CTX_FAILED
  };

  enum Controlling { ICE_CONTROLLING,
                     ICE_CONTROLLED
  };

  static RefPtr<NrIceCtx> Create(const std::string& name,
                                 bool offerer,
                                 bool set_interface_priorities = true);
  virtual ~NrIceCtx();

  nr_ice_ctx *ctx() { return ctx_; }
  nr_ice_peer_ctx *peer() { return peer_; }

  // Testing only.
  void destroy_peer_ctx();

  // Create a media stream
  RefPtr<NrIceMediaStream> CreateStream(const std::string& name,
                                                 int components);

  // The name of the ctx
  const std::string& name() const { return name_; }

  // Current state
  State state() const { return state_; }

  // Get the global attributes
  std::vector<std::string> GetGlobalAttributes();

  // Set the other side's global attributes
  nsresult ParseGlobalAttributes(std::vector<std::string> attrs);

  // Set whether we are controlling or not.
  nsresult SetControlling(Controlling controlling);

  // Set the STUN servers. Must be called before StartGathering
  // (if at all).
  nsresult SetStunServers(const std::vector<NrIceStunServer>& stun_servers);

  // Provide the resolution provider. Must be called before
  // StartGathering.
  nsresult SetResolver(nr_resolver *resolver);

  // Start ICE gathering
  nsresult StartGathering();

  // Start checking
  nsresult StartChecks();

  // Finalize the ICE negotiation. I.e., there will be no
  // more forking.
  nsresult Finalize();

  // Signals to indicate events. API users can (and should)
  // register for these.
  // TODO(ekr@rtfm.com): refactor this to be state change instead
  // so we don't need to keep adding signals?
  sigslot::signal1<NrIceCtx *> SignalGatheringCompleted;  // Done gathering
  sigslot::signal1<NrIceCtx *> SignalCompleted;  // Done handshaking
  sigslot::signal1<NrIceCtx *> SignalFailed;  // Failure.

  // The thread to direct method calls to
  nsCOMPtr<nsIEventTarget> thread() { return sts_target_; }

  NS_INLINE_DECL_THREADSAFE_REFCOUNTING(NrIceCtx)

 private:
  NrIceCtx(const std::string& name,
           bool offerer)
  : state_(ICE_CTX_INIT),
    name_(name),
    offerer_(offerer),
    streams_(),
    ctx_(nullptr),
    peer_(nullptr),
    ice_handler_vtbl_(nullptr),
    ice_handler_(nullptr)
  {
    // XXX: offerer_ will be used eventually;  placate clang in the meantime.
    (void)offerer_;
  }

  DISALLOW_COPY_ASSIGN(NrIceCtx);

  // Callbacks for nICEr
  static void initialized_cb(NR_SOCKET s, int h, void *arg);  // ICE initialized

  // Handler implementation
  static int select_pair(void *obj,nr_ice_media_stream *stream,
                         int component_id, nr_ice_cand_pair **potentials,
                         int potential_ct);
  static int stream_ready(void *obj, nr_ice_media_stream *stream);
  static int stream_failed(void *obj, nr_ice_media_stream *stream);
  static int ice_completed(void *obj, nr_ice_peer_ctx *pctx);
  static int msg_recvd(void *obj, nr_ice_peer_ctx *pctx,
                       nr_ice_media_stream *stream, int component_id,
                       unsigned char *msg, int len);

  // Iterate through all media streams and emit the candidates
  // Note that we don't do trickle ICE yet
  void EmitAllCandidates();

  // Find a media stream by stream ptr. Gross
  RefPtr<NrIceMediaStream> FindStream(nr_ice_media_stream *stream);

  // Set the state
  void SetState(State state);

  State state_;
  const std::string name_;
  bool offerer_;
  std::vector<RefPtr<NrIceMediaStream> > streams_;
  nr_ice_ctx *ctx_;
  nr_ice_peer_ctx *peer_;
  nr_ice_handler_vtbl* ice_handler_vtbl_;  // Must be pointer
  nr_ice_handler* ice_handler_;  // Must be pointer
  nsCOMPtr<nsIEventTarget> sts_target_; // The thread to run on
};


}  // close namespace
#endif
