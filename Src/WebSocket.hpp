// foo_showplay - ShowPlay client component
//
// Copyright (C) 2021 VacuityBox
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
// SPDX-License-Identifier: GPL-3.0-only 

#pragma once

#include <ixwebsocket/IXWebSocket.h>
#include <nlohmann/json.hpp>
#include <functional>
#include <string>
#include <optional>

namespace foo_showplay {

class WebSocketClient
{
    ix::WebSocket              mContext;
    std::optional<std::string> mToken;
    bool                       mIsActive;
    int                        mFrame;

    std::function<void()> mOnConnectedCallback;
    std::function<void()> mOnDisconnectedCallback;
    std::function<void()> mOnActivatedCallback;
    std::function<void()> mOnDeactivatedCallback;

    auto OnReceiveCallback (const ix::WebSocketMessagePtr& message) -> void;
    auto Reset () -> void;

    auto ParseToken     (std::string message) const -> std::optional<std::string>;
    auto ValidateToken  (std::string token)   const -> bool;
    auto PreparePayload (nlohmann::json json) const -> std::string;

public:
    WebSocketClient();
    ~WebSocketClient();

    auto SetOnConnectedCallback    (std::function<void()> callback) { mOnConnectedCallback    = callback; }
    auto SetOnDisconnectedCallback (std::function<void()> callback) { mOnDisconnectedCallback = callback; }
    auto SetOnActivatedCallback    (std::function<void()> callback) { mOnActivatedCallback    = callback; }
    auto SetOnDeactivatedCallback  (std::function<void()> callback) { mOnDeactivatedCallback  = callback; }
    
    auto TryConnect (const std::string addr)    -> bool;
    auto Send       (const nlohmann::json json) -> void;
    auto Disconnect ()                          -> void;

    auto IsConnected  () const -> bool { return mContext.getReadyState() == ix::ReadyState::Open; }
    auto IsActive     () const -> bool { return mIsActive && IsConnected(); }

    auto GetToken     () const -> std::optional<std::string> { return mToken;            }
    auto GetServerUrl () const -> std::string                { return mContext.getUrl(); }
};

} // namespace foo_showplay
