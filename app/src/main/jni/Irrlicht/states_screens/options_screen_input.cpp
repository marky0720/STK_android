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

#include "states_screens/options_screen_input.hpp"

#include "graphics/irr_driver.hpp"
#include "guiengine/CGUISpriteBank.h"
#include "guiengine/screen.hpp"
#include "guiengine/widget.hpp"
#include "guiengine/widgets/button_widget.hpp"
#include "guiengine/widgets/list_widget.hpp"
#include "guiengine/widgets/ribbon_widget.hpp"
#include "input/input_manager.hpp"
#include "input/device_manager.hpp"
#include "io/file_manager.hpp"
#include "states_screens/options_screen_input2.hpp"
#include "states_screens/options_screen_audio.hpp"
#include "states_screens/options_screen_players.hpp"
#include "states_screens/options_screen_video.hpp"
#include "states_screens/dialogs/add_device_dialog.hpp"
#include "states_screens/state_manager.hpp"
#include "utils/string_utils.hpp"
#include "utils/translation.hpp"

#include <iostream>
#include <sstream>
#include <set>

using namespace GUIEngine;

DEFINE_SCREEN_SINGLETON( OptionsScreenInput );

// -----------------------------------------------------------------------------

OptionsScreenInput::OptionsScreenInput() : Screen("options_input.stkgui")
{
}   // OptionsScreenInput

// -----------------------------------------------------------------------------

void OptionsScreenInput::loadedFromFile()
{    
    video::ITexture* icon1 = irr_driver->getTexture( file_manager->getGUIDir() + "/keyboard.png" );
    video::ITexture* icon2 = irr_driver->getTexture( file_manager->getGUIDir() + "/gamepad.png" );

    m_icon_bank = new irr::gui::STKModifiedSpriteBank( GUIEngine::getGUIEnv() );
    m_icon_bank->addTextureAsSprite(icon1);    
    m_icon_bank->addTextureAsSprite(icon2);
    
    // scale icons depending on screen resolution. the numbers below are a bit arbitrary
    const int screen_width = irr_driver->getFrameSize().Width;
    const float scale = 0.3f + 0.2f*std::max(0, screen_width - 640)/564.0f;
    m_icon_bank->setScale(scale);
}   // loadFromFile

// -----------------------------------------------------------------------------

void OptionsScreenInput::buildDeviceList()
{
    GUIEngine::ListWidget* devices = this->getWidget<GUIEngine::ListWidget>("devices");
    assert( devices != NULL );
    
    assert( m_icon_bank != NULL );
    devices->setIcons(m_icon_bank);
    
    const int keyboard_config_count = input_manager->getDeviceList()->getKeyboardConfigAmount();
    
    for (int i=0; i<keyboard_config_count; i++)
    {
        //KeyboardConfig *config = input_manager->getDeviceList()->getKeyboardConfig(i);
        
        std::ostringstream kbname;
        kbname << "keyboard" << i;
        const std::string internal_name = kbname.str();
        
        //FIXME: I18N: since irrLicht's list widget has the nasty tendency to put the 
        //             icons very close to the text, I'm adding spaces to compensate...
        devices->addItem(internal_name, StringUtils::insertValues(_("   Keyboard %i"), i), 0 /* icon */);
    }
    
    const int gpad_config_count = input_manager->getDeviceList()->getGamePadConfigAmount();
    
    for (int i = 0; i < gpad_config_count; i++)
    {
        GamepadConfig *config = input_manager->getDeviceList()->getGamepadConfig(i);
        
        // Don't display the configuration if a matching device is not available
        if (config->isPlugged())
        {
            //FIXME: since irrLicht's list widget has the nasty tendency to put the 
            //       icons very close to the text, I'm adding spaces to compensate...
            const irr::core::stringw name = irr::core::stringw("   ") + config->getName().c_str();
            
            std::ostringstream gpname;
            gpname << "gamepad" << i;
            const std::string internal_name = gpname.str();
            
            devices->addItem(internal_name, name, 1 /* icon */);
        }   // if config->isPlugged
    }   // for i<gpad_config_count
}   // buildDeviceList

// -----------------------------------------------------------------------------
void OptionsScreenInput::init()
{
    Screen::init();
    RibbonWidget* tabBar = this->getWidget<RibbonWidget>("options_choice");
    if (tabBar != NULL)  tabBar->select( "tab_controls", PLAYER_ID_GAME_MASTER );
    
    /*
    DynamicRibbonWidget* devices = this->getWidget<DynamicRibbonWidget>("devices");
    assert( devices != NULL );
    */
    
      
    buildDeviceList();        

    //devices->updateItemDisplay();
    
    /*
    // trigger displaying bindings for default selected device
    const std::string name2("devices");
    eventCallback(devices, name2, PLAYER_ID_GAME_MASTER);
     */
}   // init

// -----------------------------------------------------------------------------

void OptionsScreenInput::rebuildDeviceList()
{
    /*
    DynamicRibbonWidget* devices = this->getWidget<DynamicRibbonWidget>("devices");
    assert( devices != NULL );
    
    devices->clearItems();
    buildDeviceList();        
    devices->updateItemDisplay();
     */
    
    ListWidget* devices = this->getWidget<ListWidget>("devices");
    assert( devices != NULL );
    
    devices->clear();
    buildDeviceList();        
}   // rebuildDeviceList

// -----------------------------------------------------------------------------

void OptionsScreenInput::eventCallback(Widget* widget, const std::string& name, const int playerID)
{
    //const std::string& screen_name = this->getName();
    
    if (name == "options_choice")
    {
        std::string selection = ((RibbonWidget*)widget)->getSelectionIDString(PLAYER_ID_GAME_MASTER).c_str();
        
        if (selection == "tab_audio") StateManager::get()->replaceTopMostScreen(OptionsScreenAudio::getInstance());
        else if (selection == "tab_video") StateManager::get()->replaceTopMostScreen(OptionsScreenVideo::getInstance());
        else if (selection == "tab_players") StateManager::get()->replaceTopMostScreen(OptionsScreenPlayers::getInstance());
        else if (selection == "tab_controls") StateManager::get()->replaceTopMostScreen(OptionsScreenInput::getInstance());
    }
    else if (name == "add_device")
    {
        new AddDeviceDialog();
    }
    else if (name == "back")
    {
        StateManager::get()->escapePressed();
    }
    else if (name == "devices")
    {
        ListWidget* devices = this->getWidget<ListWidget>("devices");
        assert(devices != NULL);
        
        const std::string& selection = devices->getSelectionInternalName();
        if (selection.find("gamepad") != std::string::npos)
        {
            int i = -1, read = 0;
            read = sscanf( selection.c_str(), "gamepad%i", &i );
            if (read == 1 && i != -1)
            {
                OptionsScreenInput2::getInstance()->setDevice( input_manager->getDeviceList()->getGamepadConfig(i) );
                StateManager::get()->replaceTopMostScreen(OptionsScreenInput2::getInstance());
                //updateInputButtons( input_manager->getDeviceList()->getGamepadConfig(i) );
            }
            else
            {
                std::cerr << "Cannot read internal gamepad input device ID : " << selection.c_str() << std::endl;
            }
        }
        else if (selection.find("keyboard") != std::string::npos)
        {
            int i = -1, read = 0;
            read = sscanf( selection.c_str(), "keyboard%i", &i );
            if (read == 1 && i != -1)
            {
                // updateInputButtons( input_manager->getDeviceList()->getKeyboardConfig(i) );
                OptionsScreenInput2::getInstance()->setDevice( input_manager->getDeviceList()->getKeyboardConfig(i) );
                StateManager::get()->replaceTopMostScreen(OptionsScreenInput2::getInstance());
            }
            else
            {
                std::cerr << "Cannot read internal keyboard input device ID : " << selection.c_str() << std::endl;
            }
        }
        else
        {
            std::cerr << "Cannot read internal input device ID : " << selection.c_str() << std::endl;
        }
    }
    
}   // eventCallback

// -----------------------------------------------------------------------------

void OptionsScreenInput::unloaded()
{
    delete m_icon_bank;
    m_icon_bank = NULL;
}   // unloaded

