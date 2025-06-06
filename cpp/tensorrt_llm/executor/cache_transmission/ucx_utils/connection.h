/*
 * SPDX-FileCopyrightText: Copyright (c) 2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "ucxx/api.h"
#include "ucxx/utils/sockaddr.h"
#include "ucxx/utils/ucx.h"
#include <cstdint>
#if __linux__
#include <arpa/inet.h>
#include <ifaddrs.h>
#endif
#include "tensorrt_llm/common/cudaUtils.h"
#include "tensorrt_llm/common/envUtils.h"
#include "tensorrt_llm/executor/cacheCommunicator.h"
#include <memory>

namespace tensorrt_llm::executor::kv_cache
{

class UcxConnectionManager;

class UcxConnection : public Connection
{
public:
    UcxConnection() = default;
    explicit UcxConnection(
        uint64_t connectionId, std::shared_ptr<ucxx::Endpoint> endpoint, UcxConnectionManager* manager);
    void sendGID();
    void send(DataContext const& ctx, void const* data, size_t size) const override;
    void recv(DataContext const& ctx, void* data, size_t size) const override;
    friend class UcxConnectionManager;

private:
    void initializeEndpointTag(int maxTryTimes = 10);

    // a send tag is defined as:
    // | local port (16 bits) | remote port (16 bits) | truncated request id (16 bits) | UCXComm flags (16 bits) |
    // a recv tag is defined as:
    // | remote port (16 bits) | local port (16 bits) | truncated request id (16 bits) | UCXComm flags (16 bits) |
    ucxx::Tag mSendTag{0};
    ucxx::Tag mRecvTag{0};

    mutable std::mutex mMtx;
    mutable std::condition_variable mCv;
    uint64_t mConnectionId;
    uint64_t mLocalGID;
    uint64_t mRemoteGID;
    std::shared_ptr<ucxx::Endpoint> mEndpoint;
    UcxConnectionManager* mManager;
};

} // namespace tensorrt_llm::executor::kv_cache
