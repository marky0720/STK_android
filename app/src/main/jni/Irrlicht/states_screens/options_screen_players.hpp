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


#ifndef __HEADER_OPTIONS_SCREEN_PLAYERS_HPP__
#define __HEADER_OPTIONS_SCREEN_PLAYERS_HPP__

#include <string>
#include "irrlicht.h"

#include "guiengine/screen.hpp"

namespace GUIEngine { class Widget; }

struct Input;
class PlayerProfile;

/**
  * \brief Player management options screen
  * \ingroup states_screens
  */
class OptionsScreenPlayers : public GUIEngine::Screen, public GUIEngine::ScreenSingleton<OptionsScreenPlayers>
{

    OptionsScreenPlayers();
public:
    friend class GUIEngine::ScreenSingleton<OptionsScreenPlayers>;
    
    /** \brief implement callback from parent class GUIEngine::Screen */
    virtual void loadedFromFile();
    
    /** \brief implement callback from parent class GUIEngine::Screen */
    virtual void eventCallback(GUIEngine::Widget* widget, const std::string& name, const int playerID);
    
    /**
     * \brief Adds a new player (if 'player' is NULL) or renames an existing player (if 'player' is not NULL)
     * \return  whether adding was successful (can fail e.g. if trying to add a duplicate)
     */
    bool gotNewPlayerName(const irr::core::stringw& newName,  PlayerProfile* player=NULL);
    void deletePlayer(PlayerProfile* player);
    
    /** \brief implement callback from parent class GUIEngine::Screen */
    virtual void init();
    
    /** \brief implement callback from parent class GUIEngine::Screen */
    virtual void tearDown();
};

#endif
