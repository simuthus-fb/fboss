/*
 *  Copyright (c) 2004-present, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */
#pragma once

#include "fboss/agent/if/gen-cpp2/ctrl_clients.h"
#include "fboss/qsfp_service/if/gen-cpp2/qsfp_clients.h"

#include <folly/IPAddress.h>
#include <folly/io/async/EventBase.h>
#include <thrift/lib/cpp2/async/HeaderClientChannel.h>
#include <memory>
#include <optional>

DECLARE_string(wedge_agent_host);
DECLARE_int32(wedge_agent_port);
DECLARE_string(qsfp_service_host);
DECLARE_int32(qsfp_service_port);

namespace facebook::fboss::utils {

auto constexpr kConnTimeout = 1000;
auto constexpr kRecvTimeout = 45000;
auto constexpr kSendTimeout = 5000;

template <typename ClientT>
std::unique_ptr<apache::thrift::Client<ClientT>> createPlaintextClient(
    const folly::SocketAddress& dstAddr,
    const std::optional<folly::SocketAddress>& srcAddr = std::nullopt,
    folly::EventBase* eb = nullptr) {
  folly::EventBase* socketEb =
      eb ? eb : folly::EventBaseManager::get()->getEventBase();

  auto socket = folly::AsyncSocket::UniquePtr(new folly::AsyncSocket(socketEb));
  socket->setSendTimeout(kSendTimeout);
  socket->connect(
      nullptr,
      dstAddr,
      kConnTimeout,
      folly::emptySocketOptionMap,
      srcAddr ? *srcAddr : folly::AsyncSocketTransport::anyAddress());
  auto channel =
      apache::thrift::RocketClientChannel::newChannel(std::move(socket));
  channel->setTimeout(kRecvTimeout);
  return std::make_unique<apache::thrift::Client<ClientT>>(std::move(channel));
}

// Prefers encrypted client, creates plaintext client if certs are unavailble
template <typename ClientT>
std::unique_ptr<apache::thrift::Client<ClientT>> tryCreateEncryptedClient(
    const folly::SocketAddress& dstAddr,
    const std::optional<folly::SocketAddress>& srcAddr = std::nullopt,
    folly::EventBase* eb = nullptr);

std::unique_ptr<apache::thrift::Client<facebook::fboss::FbossCtrl>>
createWedgeAgentClient(
    const std::optional<folly::SocketAddress>& dstAddr = std::nullopt,
    folly::EventBase* eb = nullptr);

std::unique_ptr<apache::thrift::Client<facebook::fboss::QsfpService>>
createQsfpServiceClient(
    const std::optional<folly::SocketAddress>& dstAddr = std::nullopt,
    folly::EventBase* eb = nullptr);
} // namespace facebook::fboss::utils
