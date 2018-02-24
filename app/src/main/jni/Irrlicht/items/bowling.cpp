//  $Id: spark.cpp 1284 2007-11-08 12:31:54Z hikerstk $
//
//  SuperTuxKart - a fun racing game with go-kart
//  Copyright (C) 2007 Joerg Henrichs
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

#include "graphics/camera.hpp"
#include "io/xml_node.hpp"
#include "items/bowling.hpp"
#include "karts/kart.hpp"

float Bowling::m_st_max_distance;   // maximum distance for a bowling ball to be attracted
float Bowling::m_st_max_distance_squared;
float Bowling::m_st_force_to_target;

// -----------------------------------------------------------------------------
Bowling::Bowling(Kart *kart) : Flyable(kart, PowerupManager::POWERUP_BOWLING, 
                                       50.0f /* mass */)
{
    float y_offset = 0.5f*kart->getKartLength() + m_extend.getY()/2.0f;
    
    // if the kart is looking backwards, release from the back
    if( kart->getControls().m_look_back ) 
    {
        y_offset   = -y_offset;
        m_speed    = -m_speed*2;
    }
    else
    {
        float min_speed = m_speed*4.0f;
        /* make it go faster when throwing forward
           so the player doesn't catch up with the ball
           and explode by touching it */
        m_speed = kart->getSpeed() + m_speed;
        if(m_speed < min_speed) m_speed = min_speed;
    }

    createPhysics(y_offset, btVector3(0.0f, 0.0f, m_speed*2),
                  new btSphereShape(0.5f*m_extend.getY()), 
                  -70.0f /*gravity*/, 
                  true /*rotates*/);
    // Even if the ball is fired backwards, m_speed must be positive,
    // otherwise the ball can start to vibrate when energy is added.
    m_speed = fabsf(m_speed);
    // Do not adjust the up velociy depending on height above terrain, since
    // this would disable gravity.
    setAdjustUpVelocity(false);

    // unset no_contact_response flags, so that the ball 
    // will bounce off the track
    int flag = getBody()->getCollisionFlags();
    flag = flag & (~ btCollisionObject::CF_NO_CONTACT_RESPONSE);
    getBody()->setCollisionFlags(flag);
    
    // should not live forever, auto-destruct after 20 seconds
    m_max_lifespan = 20;
    
}   // Bowling

// -----------------------------------------------------------------------------
/** Initialises this object with data from the power.xml file.
 *  \param node XML Node
 *  \param bowling The bowling ball mesh
 */
void Bowling::init(const XMLNode &node, scene::IMesh *bowling)
{
    Flyable::init(node, bowling, PowerupManager::POWERUP_BOWLING);
    m_st_max_distance         = 20.0f;
    m_st_max_distance_squared = 20.0f * 20.0f;
    m_st_force_to_target      = 10.0f;
 
    node.get("max-distance",    &m_st_max_distance   );
    m_st_max_distance_squared = m_st_max_distance*m_st_max_distance;
    
    node.get("force-to-target", &m_st_force_to_target);
}   // init

// -----------------------------------------------------------------------------
void Bowling::update(float dt)
{
    Flyable::update(dt);    
    const Kart *kart=0;
    Vec3        direction;
    float       minDistance;
    getClosestKart(&kart, &minDistance, &direction);
    if(kart && minDistance<m_st_max_distance_squared)   // move bowling towards kart
    {
        // limit angle, so that the bowling ball does not turn
        // around to hit a kart behind
        if(fabs(m_body->getLinearVelocity().angle(direction)) < 1.3)
        {
            direction*=1/direction.length()*m_st_force_to_target;
            m_body->applyCentralForce(direction);
        }
    }
    
    // Bowling balls lose energy (e.g. when hitting the track), so increase
    // the speed if the ball is too slow, but only if it's not too high (if
    // the ball is too high, it is 'pushed down', which can reduce the
    // speed, which causes the speed to increase, which in turn causes
    // the ball to fly higher and higher.
    btTransform trans = getTrans();
    float hat         = trans.getOrigin().getY();
    btVector3 v       = m_body->getLinearVelocity();
    float vlen        = v.length2();
    if (hat<= m_max_height)
    {
        if(vlen<0.8*m_speed*m_speed)
        {   // bowling lost energy (less than 80%), i.e. it's too slow - speed it up:
            if(vlen==0.0f) {
                v    = btVector3(.5f, .0, 0.5f);  // avoid 0 div.
            }
            m_body->setLinearVelocity(v*m_speed/sqrt(vlen));
        }   // vlen < 0.8*m_speed*m_speed
    }   // hat< m_max_height  
    
    if(vlen<0.1)
        hit(NULL);
}   // update
// -----------------------------------------------------------------------------
