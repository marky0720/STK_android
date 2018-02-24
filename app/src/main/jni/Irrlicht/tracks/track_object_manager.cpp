//  $Id: track_object_manager.cpp 4308 2009-12-17 00:22:29Z hikerstk $
//
//  SuperTuxKart - a fun racing game with go-kart
//  Copyright (C) 2009 Joerg Henrichs
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

#include "tracks/track_object_manager.hpp"

#include "animations/three_d_animation.hpp"
#include "io/xml_node.hpp"
#include "physics/physical_object.hpp"
#include "tracks/track_object.hpp"

TrackObjectManager::TrackObjectManager()
{
}   // TrackObjectManager

// ----------------------------------------------------------------------------
TrackObjectManager::~TrackObjectManager()
{
    for(unsigned int i=0; i<m_all_objects.size(); i++)
    {
        delete m_all_objects[i];
    }
}   // ~TrackObjectManager

// ----------------------------------------------------------------------------
/** Adds an object to the track object manager. The type to add is specified
 *  in the xml_node.
 */
void TrackObjectManager::add(const XMLNode &xml_node, const Track &track)
{
    std::string type;
    xml_node.get("type", &type);
    if(type=="movable")
    {
        m_all_objects.push_back(new PhysicalObject(xml_node));
    }
    else if(type=="animation")
    {
        m_all_objects.push_back(new ThreeDAnimation(track, xml_node));
    }
    else
    {
        fprintf(stderr, "Unknown track object: '%s' - ignored.\n", type.c_str());
    }
}   // add

// ----------------------------------------------------------------------------
/** Initialises all track objects.
 */
void TrackObjectManager::init()
{
    std::vector<TrackObject*>::iterator i;
    for(i=m_all_objects.begin(); i!=m_all_objects.end(); i++)
    {
        (*i)->init();
    }
}   // reset
// ----------------------------------------------------------------------------
/** Initialises all track objects.
 */
void TrackObjectManager::reset()
{
    std::vector<TrackObject*>::iterator i;
    for(i=m_all_objects.begin(); i!=m_all_objects.end(); i++)
    {
        (*i)->reset();
    }
}   // reset

// ----------------------------------------------------------------------------
/** Handles an explosion, i.e. it makes sure that all physical objects are
 *  affected accordingly.
 *  \param pos  Position of the explosion.
 *  \param obj  If the hit was a physical object, this object will be affected
 *              more. Otherwise this is NULL.
 */

void TrackObjectManager::handleExplosion(const Vec3 &pos, const PhysicalObject *mp) const
{
    for(std::vector<TrackObject*>::const_iterator i=m_all_objects.begin();
        i!=m_all_objects.end(); i++)
    {
        (*i)->handleExplosion(pos, mp==(*i));
    }
}   // handleExplosion

// ----------------------------------------------------------------------------
void TrackObjectManager::update(float dt)
{
    for(std::vector<TrackObject*>::const_iterator i=m_all_objects.begin();
        i!=m_all_objects.end(); i++)
    {
        (*i)->update(dt);
    }
}   // update

// ----------------------------------------------------------------------------
