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

#include "PCH.hpp"
#include "WebSocket.hpp"

namespace foo_showplay {

auto WebSocketClient::OnReceiveCallback(const ix::WebSocketMessagePtr& message) -> void
{
    switch (message->type)
    {
    case ix::WebSocketMessageType::Open:
        Reset();
        std::invoke(mOnConnectedCallback);
        break;

    case ix::WebSocketMessageType::Close:
        Reset();
        std::invoke(mOnDisconnectedCallback);
        break;

    case ix::WebSocketMessageType::Message:
        // Call callback only if state changes.
        mToken = ParseToken(message->str);
        if (mToken.has_value() && !mIsActive)
        {
            mIsActive = true;
            std::invoke(mOnActivatedCallback);
        }
        else if (!mToken.has_value() && mIsActive)
        {
            mIsActive = false;
            std::invoke(mOnDeactivatedCallback);
        }
        break;
    }
}

auto WebSocketClient::Reset() -> void
{
    mToken    = std::nullopt;
    mIsActive = false;
    mFrame    = 0;
}

auto WebSocketClient::ParseToken(std::string message) const -> std::optional<std::string>
{
    // Try to deserialize json. And parse token.
    auto json = nlohmann::json::parse(message, nullptr, false);
    if (!json.is_discarded())
    {
        auto tokenIt = json.find<std::string>("Token");
        if (tokenIt != json.end())
        {
            if (tokenIt.value().is_string())
            {
                auto tokenStr = std::string();
                tokenIt.value().get_to<std::string>(tokenStr);

                if (ValidateToken(tokenStr))
                {
                    return tokenStr;
                }
            }
        }
    }

    return std::nullopt;
}

auto WebSocketClient::ValidateToken(std::string token) const -> bool
{
    auto validTokenLength = 36;
    if (token.length() != validTokenLength)
    {
        return false;
    }

    auto i = 0;
    for (auto c : token)
    {
        if (i == 8 || i == 13 || i == 18 || i == 23)
        {
            if (c != '-')
            {
                return false;
            }
        }
        else
        {
            if (std::isalnum(c) == 0)
            {
                return false;
            }
        }

        i += 1;
    }

    return true;
}

auto WebSocketClient::PreparePayload(nlohmann::json json) const -> std::string
{
    json["Token"] = mToken.has_value() ? mToken.value() : nullptr;
    json["Frame"] = mFrame;

    return json.dump(4);
}

WebSocketClient::WebSocketClient()
    : mToken    (std::nullopt)
    , mIsActive (false)
    , mFrame    (0)
    , mOnConnectedCallback    ([]{})
    , mOnDisconnectedCallback ([]{})
    , mOnActivatedCallback    ([]{})
    , mOnDeactivatedCallback  ([]{})
{
    // Enabled by default.
    mContext.disablePerMessageDeflate();

    mContext.setOnMessageCallback(
        [this](const ix::WebSocketMessagePtr& message)
        {
            OnReceiveCallback(message);
        }
    );
}

WebSocketClient::~WebSocketClient()
{
    Disconnect();
}

auto WebSocketClient::TryConnect(std::string url) -> bool
{
    // Address changed. Disconnect and connect to new server.
    if (mContext.getUrl() != url)
    {
        Disconnect();
    }

    // Don't try to connect if connecting/connected/closing.
    if (mContext.getReadyState() != ix::ReadyState::Closed)
    {
        return false;
    }

    Reset();

    // Connect.
    mContext.setUrl(url);
    mContext.start();
    
    return true;
}

auto WebSocketClient::Send(const nlohmann::json json) -> void
{
    if (!IsConnected())
    {
        return;
    }

    auto payload = PreparePayload(json);
    auto sendInfo = mContext.sendText(payload);
    mFrame += 1;
}

auto WebSocketClient::Disconnect() -> void
{
    if (!IsConnected())
    {
        return;
    }

    mContext.stop();
}

} // namespace foo_showplay
