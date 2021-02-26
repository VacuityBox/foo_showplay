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

#include <string>
#include <optional>

#include "OptionalSerializer.hpp"

namespace foo_showplay {

// -------------------------------------------------------------------------- //

struct PlayerInfo
{
    std::string Name;
    
    PlayerInfo()
        : Name("UnknownPlayer")
    {
    }
    
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(PlayerInfo, Name)
};

// -------------------------------------------------------------------------- //

enum class PlaybackState
{
    Nothing = 0,
    Paused  = 1,
    Playing = 2,
};

struct PlaybackInfo
{
    std::optional<PlaybackState> State;
    std::optional<double>        Elapsed;

    PlaybackInfo()
        : State   (std::nullopt)
        , Elapsed (std::nullopt)
    {
    }
    
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(PlaybackInfo, State, Elapsed)
};

// -------------------------------------------------------------------------- //

struct SongInfo
{
    std::optional<std::string> Title;
    std::optional<std::string> Artist;
    std::optional<std::string> Album;
    std::optional<std::string> Date;
    std::optional<std::string> Year;
    std::optional<int>         TrackNumber;
    std::optional<double>      Length;
    std::optional<std::string> Path;

    SongInfo()
        : Title       (std::nullopt)
        , Artist      (std::nullopt)
        , Album       (std::nullopt)
        , Date        (std::nullopt)
        , Year        (std::nullopt)
        , TrackNumber (std::nullopt)
        , Length      (std::nullopt)
        , Path        (std::nullopt)
    {
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(
        SongInfo, Title, Artist, Album, Date, Year, TrackNumber, Length, Path
    )
};

// -------------------------------------------------------------------------- //

struct CoverInfo
{
    std::optional<std::string> Image; // in base64

    CoverInfo()
        : Image(std::nullopt)
    {
    }
    
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(CoverInfo, Image)
};

// -------------------------------------------------------------------------- //

struct Payload
{
    std::optional<PlayerInfo>   Player;
    std::optional<PlaybackInfo> Playback;
    std::optional<SongInfo>     Song;
    std::optional<CoverInfo>    Cover;

    Payload()
        : Player   (std::nullopt)
        , Playback (std::nullopt)
        , Song     (std::nullopt)
        , Cover    (std::nullopt)
    {
    }

    Payload(
        std::optional<PlayerInfo>   player,
        std::optional<PlaybackInfo> playback,
        std::optional<SongInfo>     song,
        std::optional<CoverInfo>    cover
    )
        : Player   (player)
        , Playback (playback)
        , Song     (song)
        , Cover    (cover)
    {
    }
    
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Payload, Player, Playback, Song, Cover)
};

// -------------------------------------------------------------------------- //

} // namespace foo_showplay
