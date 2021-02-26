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
#include "Preferences.hpp"
#include "Constants.hpp"
#include "Main.hpp"

// These GUIDs identify the variables within our component's configuration file.
static const auto GUID_CFG_SHOWPLAY_SERVER_URL = GUID{ 0x4d7dc091, 0x70cd, 0x4249, { 0xb9, 0x5f, 0xea, 0x9b, 0x99, 0x38, 0xb, 0x82 } };
static auto cfgServerUrl = cfg_string(GUID_CFG_SHOWPLAY_SERVER_URL, foo_showplay::DEFAULT_SERVER_URL);

namespace foo_showplay {
    cfg_string* gCfgServerUrl = &cfgServerUrl;
}

namespace foo_showplay {

auto ShowPlayPreferences::OnInitDialog(CWindow, LPARAM) -> BOOL
{
    uSetDlgItemText(*this, IDC_SERVER_URL, gCfgServerUrl->c_str());
    UpdateConnectionStatus();

    return FALSE;
}

auto ShowPlayPreferences::OnEditChange(UINT, int, CWindow) -> void
{
    OnChanged();
}

auto ShowPlayPreferences::UpdateConnectionStatus() -> void
{
    auto client = GetShowPlayClient();
    if (client)
    {
        auto isConnected = client->IsConnected();
        auto token = client->GetToken();

        uSetDlgItemText(*this, IDC_STATUS, isConnected ? "Connected" : "Disconnected");
        uSetDlgItemText(*this, IDC_TOKEN, token.has_value() ? token.value().c_str() : "");
    }
}

auto ShowPlayPreferences::get_state() -> t_uint32
{
    auto state = static_cast<t_uint32>(preferences_state::resettable);
    if (HasChanged())
    {
        state |= preferences_state::changed;
    }

    return state;
}

auto ShowPlayPreferences::reset() -> void
{
    uSetDlgItemText(*this, IDC_SERVER_URL, DEFAULT_SERVER_URL);
    UpdateConnectionStatus();
    OnChanged();
}

auto ShowPlayPreferences::apply() ->void
{
    auto str = uGetDlgItemText(*this, IDC_SERVER_URL);
    gCfgServerUrl->set_string(str.c_str());

    auto client = GetShowPlayClient();
    if (client)
    {
        client->Connect(str.c_str());
    }

    OnChanged();
}

auto ShowPlayPreferences::HasChanged() -> bool
{
    auto str = uGetDlgItemText(*this, IDC_SERVER_URL);
    return str != *gCfgServerUrl;
}

auto ShowPlayPreferences::OnChanged() -> void
{
    m_callback->on_state_changed();
}

} // namespace foo_showplay
