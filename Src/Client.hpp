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

#include <foobar2000.h>

#include "Payload.hpp"
#include "Preferences.hpp"
#include "TitleFormatScripts.hpp"
#include "WebSocket.hpp"

namespace foo_showplay {

class ShowPlayClient : private play_callback_impl_base
{
    WebSocketClient mWebSocketPtr;
    FormatScripts   mFormatScripts;
    now_playing_album_art_notify* mArtNotify;

    // Playback callback methods.
    auto on_playback_starting           (play_control::t_track_command p_command, bool p_paused) -> void;
    auto on_playback_new_track          (metadb_handle_ptr p_track)            -> void;
    auto on_playback_stop               (play_control::t_stop_reason p_reason) -> void;
    auto on_playback_seek               (double p_time)                        -> void;
    auto on_playback_pause              (bool p_state)                         -> void;
    auto on_playback_edited             (metadb_handle_ptr p_track)            -> void;
    auto on_playback_dynamic_info       (const file_info& p_info)              -> void;
    auto on_playback_dynamic_info_track (const file_info& p_info)              -> void;
    auto on_playback_time               (double p_time)                        -> void;
    auto on_volume_change               (float p_new_val)                      -> void;
    auto on_album_art                   (album_art_data::ptr data)             -> void;

    // WebSocket Client callbacks.
    auto OnConnected    () -> void;
    auto OnDisconnected () -> void;
    auto OnActivated    () -> void;
    auto OnDeactivated  () -> void;

    auto InMainThreadOnConnected    () -> void { fb2k::inMainThread([this]() { OnConnected    (); }); }
    auto InMainThreadOnDisconnected () -> void { fb2k::inMainThread([this]() { OnDisconnected (); }); }
    auto InMainThreadOnActivated    () -> void { fb2k::inMainThread([this]() { OnActivated    (); }); }
    auto InMainThreadOnDeactivated  () -> void { fb2k::inMainThread([this]() { OnDeactivated  (); }); }

    auto GetPlayerInfo   ()                          -> std::optional<PlayerInfo>;
    auto GetPlaybackInfo ()                          -> std::optional<PlaybackInfo>;
    auto GetSongInfo     ()                          -> std::optional<SongInfo>;
    auto GetCoverInfo    ()                          -> std::optional<CoverInfo>;
    auto GetSongInfo     (metadb_handle_ptr p_track) -> std::optional<SongInfo>;
    auto GetCoverInfo    (album_art_data::ptr data)  -> std::optional<CoverInfo>;

    auto SendPlayerInfo   () -> void;
    auto SendPlaybackInfo () -> void;
    auto SendSongInfo     () -> void;
    auto SendCoverInfo    () -> void;
    auto SendPlaybackInfo (double elapsed) -> void;
    auto SendPlaybackInfo (PlaybackState state, std::optional<double> elapsed) -> void;
    auto SendSongInfo     (metadb_handle_ptr p_track) -> void;
    auto SendCoverInfo    (album_art_data::ptr data)  -> void;

    auto SendPayload (Payload payload) -> void;

    auto UpdatePreferencesStatus () -> void;

public:
    ShowPlayClient()
    {
        // Register callbacks.
        mWebSocketPtr.SetOnConnectedCallback    ([this]() { InMainThreadOnConnected    (); });
        mWebSocketPtr.SetOnDisconnectedCallback ([this]() { InMainThreadOnDisconnected (); });
        mWebSocketPtr.SetOnActivatedCallback    ([this]() { InMainThreadOnActivated    (); });
        mWebSocketPtr.SetOnDeactivatedCallback  ([this]() { InMainThreadOnDeactivated  (); });

        // Add art notify callback.
        auto artNotifyManager = static_api_ptr_t<now_playing_album_art_notify_manager>();
        mArtNotify = artNotifyManager->add([this](album_art_data::ptr data) { on_album_art(data); });
    }

    ~ShowPlayClient()
    {
        // Delete art notify callback.
        if (mArtNotify)
        {
            auto artNotifyManager = static_api_ptr_t<now_playing_album_art_notify_manager>();
            artNotifyManager->remove(mArtNotify);
        }
    }

    auto Start () -> void
    {
        mWebSocketPtr.TryConnect(gCfgServerUrl->c_str());
    }

    auto Stop  () -> void
    {
        mWebSocketPtr.Disconnect();
    }

    auto Connect (std::string url) -> void
    {
        mWebSocketPtr.TryConnect(url);
    }

    auto IsConnected () const -> bool                       { return mWebSocketPtr.IsConnected(); }
    auto GetToken    () const -> std::optional<std::string> { return mWebSocketPtr.GetToken(); }
};

} // namespace foo_showplay
