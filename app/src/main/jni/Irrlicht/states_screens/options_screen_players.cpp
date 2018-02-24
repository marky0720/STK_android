//  SuperTuxKart - a fun racing game with go-kart
//  Copyright (C) 2009 Marianne Gagnon
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 3
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#include "states_screens/options_screen_players.hpp"

#include "config/player.hpp"
#include "config/device_config.hpp"
#include "graphics/irr_driver.hpp"
#include "guiengine/engine.hpp"
#include "guiengine/screen.hpp"
#include "guiengine/widget.hpp"
#include "guiengine/widgets/list_widget.hpp"
#include "guiengine/widgets/ribbon_widget.hpp"
#include "states_screens/dialogs/enter_player_name_dialog.hpp"
#include "states_screens/dialogs/player_info_dialog.hpp"
#include "states_screens/options_screen_audio.hpp"
#include "states_screens/options_screen_input.hpp"
#include "states_screens/options_screen_video.hpp"
#include "states_screens/state_manager.hpp"


#include <iostream>
#include <sstream>

using namespace GUIEngine;
using namespace irr::core;
using namespace irr::gui;

DEFINE_SCREEN_SINGLETON( OptionsScreenPlayers );

// -----------------------------------------------------------------------------

OptionsScreenPlayers::OptionsScreenPlayers() : Screen("options_players.stkgui")
{
}   // OptionsScreenPlayers

// -----------------------------------------------------------------------------

void OptionsScreenPlayers::loadedFromFile()
{
}   // loadedFromFile

// -----------------------------------------------------------------------------

void OptionsScreenPlayers::init()
{
    Screen::init();

    RibbonWidget* tabBar = this->getWidget<RibbonWidget>("options_choice");
    if (tabBar != NULL) tabBar->select( "tab_players", PLAYER_ID_GAME_MASTER );
    
    ListWidget* players = this->getWidget<ListWidget>("players");
    assert(players != NULL);
    
    const int playerAmount = UserConfigParams::m_all_players.size();
    for(int n=0; n<playerAmount; n++)
    {
        players->addItem( UserConfigParams::m_all_players[n].getName(),
                          UserConfigParams::m_all_players[n].getName() );
    }
}   // init

// -----------------------------------------------------------------------------

bool OptionsScreenPlayers::gotNewPlayerName(const stringw& newName, PlayerProfile* player)
{
    stringc newNameC( newName );
    ListWidget* players = this->getWidget<ListWidget>("players");
    if (players == NULL) return false;
    
    // ---- Add new player
    if (player == NULL)
    {
        // check for duplicates
        const int amount = UserConfigParams::m_all_players.size();
        for (int n=0; n<amount; n++)
        {
            if (stringw(UserConfigParams::m_all_players[n].getName()) == newName) return false;
        }
        
        // add new player
        UserConfigParams::m_all_players.push_back( new PlayerProfile(newNameC.c_str()) );
        
        players->addItem( newNameC.c_str(), newNameC.c_str() );
    }
    else // ---- Rename existing player
    {
        player->setName( newNameC.c_str() );
        
        // refresh list display
        players->clear();
        const int playerAmount =  UserConfigParams::m_all_players.size();
        for(int n=0; n<playerAmount; n++)
        {
            players->addItem(UserConfigParams::m_all_players[n].getName(),
                             UserConfigParams::m_all_players[n].getName());
        }
        
    }
    return true;
}   // gotNewPlayerName

// -----------------------------------------------------------------------------

void OptionsScreenPlayers::deletePlayer(PlayerProfile* player)
{
    UserConfigParams::m_all_players.erase(player);
    
    // refresh list display
    ListWidget* players = this->getWidget<ListWidget>("players");
    if(players == NULL) return;
    players->clear();
    
    const int playerAmount =  UserConfigParams::m_all_players.size();
    for(int n=0; n<playerAmount; n++)
    {
        players->addItem(UserConfigParams::m_all_players[n].getName(),
                         UserConfigParams::m_all_players[n].getName());
    }
}   // deletePlayer

// -----------------------------------------------------------------------------

void OptionsScreenPlayers::tearDown()
{
    Screen::tearDown();
    user_config->saveConfig();
}   // tearDown

// -----------------------------------------------------------------------------

void OptionsScreenPlayers::eventCallback(Widget* widget, const std::string& name, const int playerID)
{    
    if (name == "options_choice")
    {
        std::string selection = ((RibbonWidget*)widget)->getSelectionIDString(PLAYER_ID_GAME_MASTER).c_str();
        
        if (selection == "tab_audio") StateManager::get()->replaceTopMostScreen(OptionsScreenAudio::getInstance());
        else if (selection == "tab_video") StateManager::get()->replaceTopMostScreen(OptionsScreenVideo::getInstance());
        else if (selection == "tab_players") StateManager::get()->replaceTopMostScreen(OptionsScreenPlayers::getInstance());
        else if (selection == "tab_controls") StateManager::get()->replaceTopMostScreen(OptionsScreenInput::getInstance());
    }
    else if (name == "back")
    {
        StateManager::get()->escapePressed();
    }
    else if (name == "addplayer")
    {
        new EnterPlayerNameDialog(0.5f, 0.4f);
    }
    else if (name == "players")
    {
        // Find which player in the list was clicked
        ListWidget* players = this->getWidget<ListWidget>("players");
        assert(players != NULL);
        
        std::string selectedPlayer = stringc( players->getSelectionLabel().c_str() ).c_str();
        const int playerAmount = UserConfigParams::m_all_players.size();
        for (int n=0; n<playerAmount; n++)
        {
            if (UserConfigParams::m_all_players[n].getName() == selectedPlayer)
            {
                if (!(UserConfigParams::m_all_players[n].isGuestAccount()))
                {
                    new PlayerInfoDialog( &UserConfigParams::m_all_players[n], 0.5f, 0.6f );
                }
                return;
            }
        } // end for
    }   // name=="players"
    
}   // eventCallback

// -----------------------------------------------------------------------------
