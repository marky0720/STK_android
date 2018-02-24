// $Id$
//
//  SuperTuxKart - a fun racing game with go-kart
//  Copyright (C) 2010      Joerg Henrichs
//// $Id
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

#ifndef HEADER_AI_BASE_CONTROLLER_HPP
#define HEADER_AI_BASE_CONTROLLER_HPP

#include "karts/controller/controller.hpp"
#include "states_screens/state_manager.hpp"

class LinearWorld;
class QuadGraph;
class Track;
class Vec3;

/** A base class for all AI karts. This class basically provides some
 *  common low level functions.
 */
class AIBaseController : public Controller
{
private:
    /** The minimum steering angle at which the AI adds skidding. Lower values
     *  tend to improve the line the AI is driving. This is used to adjust for
     *  different AI levels.
     */
    float    m_skidding_threshold;
protected:
    /** Length of the kart, storing it here saves many function calls. */
    float m_kart_length;

    /** Cache width of kart. */
    float m_kart_width;

    /** Keep a pointer to the track to reduce calls */
    Track       *m_track;

    /** Keep a pointer to world. */
    LinearWorld *m_world;

    /** The graph of qudas of this track. */
    const    QuadGraph *m_quad_graph;

    float    steerToAngle  (const unsigned int sector, const float angle);
    float    steerToPoint  (const Vec3 &point);
    float    normalizeAngle(float angle);
    void     setSteering   (float angle, float dt);
    void     setSkiddingFraction(float f);
    virtual unsigned int getNextSector(unsigned int index);
public:
             AIBaseController(Kart *kart,
                              StateManager::ActivePlayer *player=NULL);
    virtual ~AIBaseController() {};
};

#endif

/* EOF */
