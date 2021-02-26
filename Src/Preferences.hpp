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

#include <helpers/foobar2000+atl.h>
#include <helpers/atl-misc.h>
#include "Resource.hpp"

namespace foo_showplay {

    extern cfg_string* gCfgServerUrl;
}

namespace foo_showplay {

class ShowPlayPreferences : public CDialogImpl<ShowPlayPreferences>, public preferences_page_instance
{
    const preferences_page_callback::ptr m_callback;
    
    auto OnInitDialog    (CWindow, LPARAM)    -> BOOL;
    auto OnEditChange    (UINT, int, CWindow) -> void;
    auto HasChanged () -> bool;
    auto OnChanged  () -> void;

    auto UpdateConnectionStatus () -> void;

public:
    // Constructor - invoked by preferences_page_impl helpers - don't do Create() in here,
    // preferences_page_impl does this for us
    ShowPlayPreferences(preferences_page_callback::ptr callback)
        : m_callback(callback)
    {
    }

    // Note that we don't bother doing anything regarding destruction of our class.
    // The host ensures that our dialog is destroyed first, then the last reference to
    // our preferences_page_instance object is released, causing our object to be deleted.

    auto UpdateStatus (bool isConnected) -> void
    {
        uSetDlgItemText(*this, IDC_STATUS, isConnected ? "Connected" : "Disconnected");
    }

    auto UpdateToken (std::optional<std::string> token) -> void
    {
        uSetDlgItemText(*this, IDC_TOKEN, token.has_value() ? token.value().c_str() : "");
    }

    //dialog resource ID
    enum { IDD = IDD_MYPREFERENCES };

    // preferences_page_instance methods (not all of them - get_wnd() is supplied by preferences_page_impl helpers)
    auto get_state ()  -> t_uint32;
    auto apply     ()  -> void;
    auto reset     ()  -> void;

    //WTL message map
    BEGIN_MSG_MAP_EX(ShowPlayPreferences)
        MSG_WM_INITDIALOG(OnInitDialog)
        COMMAND_HANDLER_EX(IDC_SERVER_URL, EN_CHANGE, OnEditChange)
    END_MSG_MAP()
};

} // namespace foo_showplay
