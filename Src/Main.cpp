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
#include "Main.hpp"
#include "Preferences.hpp"

// Declaration of your component's version information
// Since foobar2000 v1.0 having at least one of these in your DLL is mandatory to let the troubleshooter tell different versions of your component apart.
// Note that it is possible to declare multiple components within one DLL, but it's strongly recommended to keep only one declaration per DLL.
// As for 1.1, the version numbers are used by the component update finder to find updates; for that to work, you must have ONLY ONE declaration per DLL. If there are multiple declarations, the component is assumed to be outdated and a version number of "0" is assumed, to overwrite the component with whatever is currently on the site assuming that it comes with proper version numbers.
DECLARE_COMPONENT_VERSION("ShowPlay", "1.0", "ShowPlay Client\nhttps://github.com/VacuityBox/ShowPlay");

// This will prevent users from renaming your component around (important for proper troubleshooter behaviors) or loading multiple instances of it.
VALIDATE_COMPONENT_FILENAME("foo_showplay.dll");


// Initialize Component.
class ShowPlayInit : public initquit
{
    std::unique_ptr<foo_showplay::ShowPlayClient> mClientPtr;

public:
    auto on_init () -> void
    {
        mClientPtr = std::make_unique<foo_showplay::ShowPlayClient>();
        if (!mClientPtr)
        {
            console::error("Failed to initialize ShowPlay client");
            return;
        }

        if (!ix::initNetSystem())
        {
            console::error("Failed to ix::initNetSystem() required by ShowPlay client");
            mClientPtr.reset(nullptr);
            return;
        }
            
        mClientPtr->Start();
        console::info("ShowPlay client started");
    }

    auto on_quit () -> void
    {
        if (mClientPtr)
        {
            mClientPtr->Stop();
            mClientPtr.reset(nullptr);
            
            ix::uninitNetSystem();

            console::info("ShowPlay client stopped");
        }
    }

    auto GetClient () -> foo_showplay::ShowPlayClient*
    {
        return mClientPtr ? mClientPtr.get() : nullptr;
    }
};

static auto gShowPlayComponentInit = initquit_factory_t<ShowPlayInit>();

// Initialize Preferences.
class ShowPlayPreferencesImpl : public preferences_page_v3
    //public preferences_page_impl<foo_showplay::ShowPlayPreferences>
{
    service_ptr_t<preferences_page_instance_impl<foo_showplay::ShowPlayPreferences>> mPtr;

public:
    auto get_name() -> const char*
    {
        return "ShowPlay Client";
    }
    
    auto get_guid() -> GUID
    {
        // {63E40BA4-8114-492B-8607-9AE2FDB25363}
        static const auto guid = GUID{ 0x63e40ba4, 0x8114, 0x492b, { 0x86, 0x7, 0x9a, 0xe2, 0xfd, 0xb2, 0x53, 0x63 } };
        return guid;
    }
    
    auto get_parent_guid() -> GUID
    {
        return guid_tools; 
    }

    auto GetPreferences() -> foo_showplay::ShowPlayPreferences*
    {
        return mPtr.is_valid() ? mPtr.get_ptr() : nullptr;
    }

    auto instantiate(HWND parent, preferences_page_callback::ptr callback) -> preferences_page_instance::ptr
    {
        if (mPtr.is_valid())
        {
            mPtr.reset();
        }

        mPtr = fb2k::service_new_window<preferences_page_instance_impl<foo_showplay::ShowPlayPreferences>>(parent, callback);
        return mPtr;
    }
};

static auto gShowPlayPreferencesImplFactory = preferences_page_factory_t<ShowPlayPreferencesImpl>();

// Get client.
auto GetShowPlayClient()->foo_showplay::ShowPlayClient*
{
    return gShowPlayComponentInit.get_static_instance().GetClient();
}

// Get preferences.
auto GetShowPlayPreferences()->foo_showplay::ShowPlayPreferences*
{
    return gShowPlayPreferencesImplFactory.get_static_instance().GetPreferences();
}
