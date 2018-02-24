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

#ifndef HEADER_MAIN_MENU_SCREEN_HPP
#define HEADER_MAIN_MENU_SCREEN_HPP

#include "guiengine/screen.hpp"

namespace GUIEngine { class Widget; }

/**
  * \brief Handles the main menu
  * \ingroup states_screens
  */
class MainMenuScreen : public GUIEngine::Screen, public GUIEngine::ScreenSingleton<MainMenuScreen>
{
    friend class GUIEngine::ScreenSingleton<MainMenuScreen>;
    MainMenuScreen();
public:
    
#ifdef ADDONS_MANAGER
    void changeNewsText(std::string action);
    pthread_mutex_t m_mutex_news_text;
    pthread_t m_thread_news_text;

    std::string m_news_text;
    virtual void onUpdate(float delta,  irr::video::IVideoDriver* driver);
    virtual void tearDown();
#endif
    
    /** \brief implement callback from parent class GUIEngine::Screen */
    virtual void loadedFromFile();
    
    /** \brief implement callback from parent class GUIEngine::Screen */
    virtual void eventCallback(GUIEngine::Widget* widget, const std::string& name, const int playerID);
    
    /** \brief implement callback from parent class GUIEngine::Screen */
    virtual void init();
    
    /** This function is used to download a text from the server to show the news. */
    static void * downloadNews(void *);
    
};

#endif
