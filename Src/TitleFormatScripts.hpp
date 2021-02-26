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
#include <optional>

namespace foo_showplay {

class TitleFormatScript
{
    titleformat_object::ptr mScript;

public:
    TitleFormatScript(const char* p_spec)
    {
        auto compiler = static_api_ptr_t<titleformat_compiler>();
        compiler->compile_safe_ex(mScript, p_spec);
    }

    std::optional<std::string> GetInfo(metadb_handle_ptr p_track)
    {
        auto sf = fb2k::formatTrackTitle(p_track, mScript);
        auto info = std::string(sf.toString());
        
        return info != "?" ? info : std::optional<std::string>(std::nullopt);
    }
};

class FormatScripts
{
    TitleFormatScript mTitleScript;
    TitleFormatScript mArtistScript;
    TitleFormatScript mAlbumScript;
    TitleFormatScript mDateScript;
    TitleFormatScript mTrackNumberScript;
    TitleFormatScript mLengthScript;
    TitleFormatScript mPathScript;
    TitleFormatScript mYearScript;

public:
    FormatScripts()
        : mTitleScript        ("%title%")
        , mArtistScript       ("%artist%")
        , mAlbumScript        ("%album%")
        , mDateScript         ("%date%")
        , mTrackNumberScript  ("%track number%")
        , mLengthScript       ("%length_seconds_fp%")
        , mPathScript         ("%path%")
        , mYearScript         ("%year%")
    {
    }

    inline auto GetTitle       (metadb_handle_ptr p_track) -> std::optional<std::string> { return mTitleScript      .GetInfo(p_track); }
    inline auto GetAlbum       (metadb_handle_ptr p_track) -> std::optional<std::string> { return mArtistScript     .GetInfo(p_track); }
    inline auto GetArtist      (metadb_handle_ptr p_track) -> std::optional<std::string> { return mAlbumScript      .GetInfo(p_track); }
    inline auto GetDate        (metadb_handle_ptr p_track) -> std::optional<std::string> { return mDateScript       .GetInfo(p_track); }
    inline auto GetTrackNumber (metadb_handle_ptr p_track) -> std::optional<std::string> { return mTrackNumberScript.GetInfo(p_track); }
    inline auto GetLength      (metadb_handle_ptr p_track) -> std::optional<std::string> { return mLengthScript     .GetInfo(p_track); }
    inline auto GetPath        (metadb_handle_ptr p_track) -> std::optional<std::string> { return mPathScript       .GetInfo(p_track); }
    inline auto GetYear        (metadb_handle_ptr p_track) -> std::optional<std::string> { return mYearScript       .GetInfo(p_track); }
};

} // namespace foo_showplay
