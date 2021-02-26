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
#include "Client.hpp"
#include "Payload.hpp"
#include "Constants.hpp"
#include "Main.hpp"

namespace foo_showplay {

auto ShowPlayClient::on_playback_starting(play_control::t_track_command p_command, bool p_paused) -> void
{
}

auto ShowPlayClient::on_playback_new_track(metadb_handle_ptr p_track) -> void
{
    SendSongInfo(p_track);
    SendPlaybackInfo();
}

auto ShowPlayClient::on_playback_stop(play_control::t_stop_reason p_reason) -> void
{
    SendPlaybackInfo(PlaybackState::Nothing, std::nullopt);
}

auto ShowPlayClient::on_playback_seek(double p_time) -> void
{
    SendPlaybackInfo(p_time);
}

auto ShowPlayClient::on_playback_pause(bool p_state) -> void
{
    if (p_state)
    {
        SendPlaybackInfo(PlaybackState::Paused, std::nullopt);
    }
    else
    {
        SendPlaybackInfo(PlaybackState::Playing, std::nullopt);
    }
}

auto ShowPlayClient::on_playback_edited(metadb_handle_ptr p_track) -> void
{
}

auto ShowPlayClient::on_playback_dynamic_info(const file_info& p_info) -> void
{
}

auto ShowPlayClient::on_playback_dynamic_info_track(const file_info& p_info) -> void
{
}

auto ShowPlayClient::on_playback_time(double p_time) -> void
{
    SendPlaybackInfo(p_time);
}

auto ShowPlayClient::on_volume_change(float p_new_val) -> void
{
}

auto ShowPlayClient::on_album_art(album_art_data::ptr data) -> void
{
    SendCoverInfo(data);
}

auto ShowPlayClient::OnConnected() -> void
{
    UpdatePreferencesStatus();
    SendPlayerInfo();
}

auto ShowPlayClient::OnDisconnected() -> void
{
    UpdatePreferencesStatus();
}

auto ShowPlayClient::OnActivated() -> void
{
    UpdatePreferencesStatus();
    SendPlaybackInfo();
    SendSongInfo();
    SendCoverInfo();
}

auto ShowPlayClient::OnDeactivated() -> void
{
    UpdatePreferencesStatus();
}

auto ShowPlayClient::GetPlayerInfo() -> std::optional<PlayerInfo>
{
    auto player = PlayerInfo();
    player.Name = PLAYER_NAME;
    return player;
}

auto ShowPlayClient::GetPlaybackInfo() -> std::optional<PlaybackInfo>
{
    // We need to get current playback state.
    auto playbackControl = static_api_ptr_t<playback_control>();

    auto playback = PlaybackInfo();
    if (playbackControl->is_paused())
    {
        playback.State   = PlaybackState::Paused;
        playback.Elapsed = static_cast<int>(playbackControl->playback_get_position());
    }
    else if (playbackControl->is_playing())
    {
        playback.State   = PlaybackState::Playing;
        playback.Elapsed = static_cast<int>(playbackControl->playback_get_position());
    }
    else
    {
        playback.State   = PlaybackState::Nothing;
        playback.Elapsed = std::nullopt;
    }

    return playback;
}

auto ShowPlayClient::GetSongInfo() -> std::optional<SongInfo>
{
    // We need to get current song.
    auto playbackControl = static_api_ptr_t<playback_control>();
    auto track = metadb_handle_ptr();
    playbackControl->get_now_playing(track);

    return GetSongInfo(track);
}

auto ShowPlayClient::GetCoverInfo() -> std::optional<CoverInfo>
{
    // We need to get current song album art.
    auto playbackControl = static_api_ptr_t<playback_control>();

    auto track = metadb_handle_ptr();
    playbackControl->get_now_playing(track);

    if (track.is_empty())
    {
        return std::nullopt;
    }

    // Get Art.
    auto artNotifyManager = static_api_ptr_t<now_playing_album_art_notify_manager>();
    auto art = artNotifyManager->current();

    return GetCoverInfo(art);
}

auto ShowPlayClient::GetSongInfo(metadb_handle_ptr p_track) -> std::optional<SongInfo>
{
    if (p_track.is_empty())
    {
        return std::nullopt;
    }

    auto song   = SongInfo();
    song.Title  = mFormatScripts.GetTitle(p_track);
    song.Album  = mFormatScripts.GetAlbum(p_track);
    song.Artist = mFormatScripts.GetArtist(p_track);
    song.Date   = mFormatScripts.GetDate(p_track);
    song.Year = mFormatScripts.GetYear(p_track);
    song.Path   = mFormatScripts.GetPath(p_track);

    auto lengthStr = mFormatScripts.GetLength(p_track);
    if (lengthStr.has_value())
    {
        song.Length = std::atof(lengthStr.value().c_str());
    }

    auto trackNumberStr = mFormatScripts.GetTrackNumber(p_track);
    if (trackNumberStr.has_value())
    {
        song.TrackNumber = std::atoi(trackNumberStr.value().c_str());
    }

    return song;
}

auto ShowPlayClient::GetCoverInfo(album_art_data::ptr data) -> std::optional<CoverInfo>
{
    // We need to get current song album art.
    auto playbackControl = static_api_ptr_t<playback_control>();

    auto track = metadb_handle_ptr();
    playbackControl->get_now_playing(track);

    if (track.is_empty())
    {
        return std::nullopt;
    }

    // Get Art.
    auto artNotifyManager = static_api_ptr_t<now_playing_album_art_notify_manager>();
    auto art = artNotifyManager->current();

    // Create cover.
    auto cover = CoverInfo();
    if (art.is_valid())
    {
        auto artDataPtr = art.get_ptr();
        if (artDataPtr != nullptr)
        {
            auto data = static_cast<const unsigned char*>(artDataPtr->get_ptr());
            auto size = static_cast<size_t>(artDataPtr->get_size());
            cover.Image = base64_encode(data, size);
        }
    }

    return cover;
}

auto ShowPlayClient::SendPlayerInfo() -> void
{
    // If client is not active then skip sending.
    if (!mWebSocketPtr.IsActive())
    {
        return;
    }

    // Create PlayerInfo.
    auto player = GetPlayerInfo();
    SendPayload(Payload(player, std::nullopt, std::nullopt, std::nullopt));
}

auto ShowPlayClient::SendPlaybackInfo() -> void
{
    // If client is not active then skip sending.
    if (!mWebSocketPtr.IsActive())
    {
        return;
    }

    auto playback = GetPlaybackInfo();
    SendPayload(Payload(std::nullopt, playback, std::nullopt, std::nullopt));
}

auto ShowPlayClient::SendSongInfo() -> void
{
    // If client is not active then skip sending.
    if (!mWebSocketPtr.IsActive())
    {
        return;
    }

    auto song = GetSongInfo();
    SendPayload(Payload(std::nullopt, std::nullopt, song, std::nullopt));
}

auto ShowPlayClient::SendCoverInfo() -> void
{
    // If client is not active then skip sending.
    if (!mWebSocketPtr.IsActive())
    {
        return;
    }
    
    auto cover = GetCoverInfo();
    SendPayload(Payload(std::nullopt, std::nullopt, std::nullopt, cover));
}

auto ShowPlayClient::SendPlaybackInfo(double elapsed) -> void
{
    // If client is not active then skip sending.
    if (!mWebSocketPtr.IsActive())
    {
        return;
    }

    auto playback    = PlaybackInfo();
    playback.State   = std::nullopt;
    playback.Elapsed = elapsed;

    SendPayload(Payload(std::nullopt, playback, std::nullopt, std::nullopt));
}

auto ShowPlayClient::SendPlaybackInfo(PlaybackState state, std::optional<double> elapsed) -> void
{
    // If client is not active then skip sending.
    if (!mWebSocketPtr.IsActive())
    {
        return;
    }

    // Create PlaybackInfo.
    auto playback    = PlaybackInfo();
    playback.State   = state;
    playback.Elapsed = elapsed;

    SendPayload(Payload(std::nullopt, playback, std::nullopt, std::nullopt));
}

auto ShowPlayClient::SendSongInfo(metadb_handle_ptr p_track) -> void
{
    // If client is not active then skip sending.
    if (!mWebSocketPtr.IsActive())
    {
        return;
    }

    auto song = GetSongInfo(p_track);
    SendPayload(Payload(std::nullopt, std::nullopt, song, std::nullopt));
}

auto ShowPlayClient::SendCoverInfo(album_art_data::ptr data) -> void
{
    // If client is not active then skip sending.
    if (!mWebSocketPtr.IsActive())
    {
        return;
    }

    auto cover = GetCoverInfo(data);
    SendPayload(Payload(std::nullopt, std::nullopt, std::nullopt, cover));
}

auto ShowPlayClient::SendPayload(Payload payload) -> void
{
    auto json = nlohmann::json(payload);

    mWebSocketPtr.Send(json);
}

auto ShowPlayClient::UpdatePreferencesStatus() -> void
{
    auto prefs = GetShowPlayPreferences();
    if (prefs)
    {
        prefs->UpdateStatus(mWebSocketPtr.IsConnected());
        prefs->UpdateToken(mWebSocketPtr.GetToken());
    }
}

} // namespace foo_showplay

