//  $Id: race_gui_base.hpp 5310 2010-04-28 18:26:23Z auria $
//
//  SuperTuxKart - a fun racing game with go-kart
//  Copyright (C) 2010 Joerg Henrichs
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

#ifndef HEADER_RACE_GUI_BASE_HPP
#define HEADER_RACE_GUI_BASE_HPP

#include "irrlicht.h"
using namespace irr;

class Kart;

/**
  * \brief An abstract base class for the two race guis (race_gui and 
  *  race_result gui)
  * \ingroup states_screens
  */
class RaceGUIBase
{
public:
    /**
      * Used to display the list of karts and their times or
      * whatever other info is relevant to the current mode.
      */
    struct KartIconDisplayInfo
    {
        /** text to display next to icon, if any */
        core::stringw m_text;
        
        /** text color, if any text */
        float r, g, b;
        
        /** if this kart has a special title, e.g. "leader" in follow-the-leader */
        irr::core::stringw special_title;
        
        /** Current lap of this kart, or -1 if irrelevant */
        int lap;
    };   // KartIconDisplayInfo

public:

                  RaceGUIBase() {};
    virtual      ~RaceGUIBase() {};
    virtual void  renderGlobal(float dt) = 0;
    virtual void  renderPlayerView(const Kart *kart) = 0;
    virtual void  addMessage(const irr::core::stringw &m, const Kart *kart, 
                             float time, 
                             int fonst_size, 
                             const video::SColor &color=
                                 video::SColor(255, 255, 0, 255),
                             bool important=true) = 0;

    virtual void  clearAllMessages() = 0;
    
    /** Returns the size of the texture on which to render the minimap to. */
    virtual const core::dimension2du 
                  getMiniMapSize() const = 0;
};   // RaceGUIBase

#endif
