//  $Id: kart.cpp 6341 2010-10-26 09:29:44Z hikerstk $
//
//  SuperTuxKart - a fun racing game with go-kart
//  Copyright (C) 2004-2005 Steve Baker <sjbaker1@airmail.net>
//  Copyright (C) 2006 SuperTuxKart-Team, Joerg Henrichs, Steve Baker
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

#include "karts/kart.hpp"

#include <math.h>
#include <iostream>

#include "audio/music_manager.hpp"
#include "audio/sfx_manager.hpp"
#include "audio/sfx_base.hpp"
#include "config/user_config.hpp"
#include "graphics/camera.hpp"
#include "graphics/material_manager.hpp"
#include "graphics/nitro.hpp"
#include "graphics/shadow.hpp"
#include "graphics/skid_marks.hpp"
#include "graphics/slip_stream.hpp"
#include "graphics/smoke.hpp"
#include "graphics/water_splash.hpp"
#include "modes/world.hpp"
#include "io/file_manager.hpp"
#include "items/item_manager.hpp"
#include "karts/controller/end_controller.hpp"
#include "karts/kart_model.hpp"
#include "karts/kart_properties_manager.hpp"
#include "network/race_state.hpp"
#include "network/network_manager.hpp"
#include "physics/btKart.hpp"
#include "physics/btUprightConstraint.hpp"
#include "physics/physics.hpp"
#include "race/history.hpp"
#include "tracks/track.hpp"
#include "utils/constants.hpp"

#if defined(WIN32) && !defined(__CYGWIN__)
   // Disable warning for using 'this' in base member initializer list
#  pragma warning(disable:4355)
#endif

/** The kart constructor. 
 *  \param ident  The identifier for the kart model to use.
 *  \param position The position (or rank) for this kart (between 1 and
 *         number of karts). This is used to determine the start position.
 *  \param init_transform  The initial position and rotation for this kart.
 */
Kart::Kart (const std::string& ident, int position,
            const btTransform& init_transform)
     : TerrainInfo(1),
       Moveable(), EmergencyAnimation(this), MaxSpeed(this), m_powerup(this)

#if defined(WIN32) && !defined(__CYGWIN__)
#  pragma warning(1:4355)
#endif
{
    m_kart_properties      = kart_properties_manager->getKart(ident);
    assert(m_kart_properties != NULL);
    // We have to take a copy of the kart model, since otherwise
    // the animations will be mixed up (i.e. different instances of
    // the same model will set different animation frames).
    // Technically the mesh in m_kart_model needs to be grab'ed and
    // released when the kart is deleted, but since the original 
    // kart_model is stored in the kart_properties all the time,
    // there is no risk of a mesh being deleted to early.
    m_kart_model           = m_kart_properties->getKartModelCopy();
    m_initial_position     = position;
    m_race_position        = position;
    m_collected_energy     = 0;
    m_finished_race        = false;
    m_finish_time          = 0.0f;
    m_shadow_enabled       = false;
    m_shadow               = NULL;
    m_smoke_system         = NULL;
    m_water_splash_system  = NULL;
    m_nitro                = NULL;
    m_slipstream           = NULL;
    m_slipstream_time      = 0.0f;
    m_skidmarks            = NULL;
    m_camera               = NULL;
    m_controller           = NULL;
    m_saved_controller     = NULL;
    m_flying               = false;
    
    m_view_blocked_by_plunger = 0;

    // Initialize custom sound vector (TODO: add back when properly done)
    // m_custom_sounds.resize(SFXManager::NUM_CUSTOMS);

    // Set position and heading:
    m_reset_transform         = init_transform;
    m_speed                   = 0.0f;
    m_wheel_rotation          = 0;

    // Create SFXBase for each custom sound (TODO: add back when properly done)
    /*
    for (int n = 0; n < SFXManager::NUM_CUSTOMS; n++)
    {
        int id = m_kart_properties->getCustomSfxId((SFXManager::CustomSFX)n);

        // If id == -1 the custom sound was not defined in the .irrkart config file
        if (id != -1)
        {
            m_custom_sounds[n] = sfx_manager->newSFX(id);
        }
    }*/

    //m_engine_sound  = sfx_manager->createSoundSource(m_kart_properties->getEngineSfxType());
    //m_beep_sound    = sfx_manager->createSoundSource( "beep"  );
    //m_crash_sound   = sfx_manager->createSoundSource( "crash" );
    //m_goo_sound     = sfx_manager->createSoundSource( "goo"   );
    //m_skid_sound    = sfx_manager->createSoundSource( "skid"  );
    //m_terrain_sound          = NULL;
 /* m_previous_terrain_sound = NULL;*/

    //if(!m_engine_sound)
    //{
    //    fprintf(stdout, "Error: Could not allocate a sfx object for the kart. Further errors may ensue!\n");
    //}

    loadData();
    float length = m_kart_properties->getSlipstreamLength();

    Vec3 p0(-getKartWidth()*0.5f, 0, -getKartLength()*0.5f       );
    Vec3 p1(-getKartWidth()*0.5f, 0, -getKartLength()*0.5f-length);
    Vec3 p2( getKartWidth()*0.5f, 0, -getKartLength()*0.5f-length);
    Vec3 p3( getKartWidth()*0.5f, 0, -getKartLength()*0.5f       );
    m_slipstream_original_quad = new Quad(p0, p1, p2, p3);
    m_slipstream_quad          = new Quad(p0, p1, p2, p3);

    reset();
}   // Kart

// -----------------------------------------------------------------------------
/** Saves the old controller in m_saved_controller and stores a new 
 *  controller. The save controller is needed in case of a reset.
 *  \param controller The new controller to use (atm it's always an
 *         end controller).
 */
void Kart::setController(Controller *controller)
{
    assert(m_saved_controller==NULL);
    m_saved_controller = m_controller;
    m_controller       = controller;
}   // setController

// -----------------------------------------------------------------------------
/** Returns a transform that will align an object with the kart: the heading 
 *  and the pitch will be set appropriately. A custom pitch value can be 
 *  specified in order to overwrite the terrain pitch (which would be used
 *  otherwise).
 *  \param customPitch Pitch value to overwrite the terrain pitch.
 */
btTransform Kart::getKartHeading(const float customPitch)
{
    btTransform trans = getTrans();

    float pitch = (customPitch == -1 ? getTerrainPitch(getHeading()) : customPitch);

    btMatrix3x3 m;
    m.setEulerYPR(-getHeading(), pitch, 0.0f);
    trans.setBasis(m);

    return trans;
}   // getKartHeading

// ----------------------------------------------------------------------------
/** Creates the physical representation of this kart. Atm it uses the actual
 *  extention of the kart model to determine the size of the collision body.
 */
void Kart::createPhysics()
{
    // First: Create the chassis of the kart
    // -------------------------------------
    float kart_width  = getKartWidth();
    float kart_length = getKartLength();
    float kart_height = getKartHeight();

    btBoxShape *shape = new btBoxShape(btVector3(0.5f*kart_width,
                                                 0.5f*kart_height,
                                                 0.5f*kart_length));
    btTransform shiftCenterOfGravity;
    shiftCenterOfGravity.setIdentity();
    // Shift center of gravity downwards, so that the kart
    // won't topple over too easy.
    shiftCenterOfGravity.setOrigin(m_kart_properties->getGravityCenterShift());
    m_kart_chassis.addChildShape(shiftCenterOfGravity, shape);

    // Set mass and inertia
    // --------------------
    float mass=getMass();

    // Position the chassis
    // --------------------
    btTransform trans;
    trans.setIdentity();
    createBody(mass, trans, &m_kart_chassis);
    m_user_pointer.set(this);
    m_body->setDamping(m_kart_properties->getChassisLinearDamping(),
                       m_kart_properties->getChassisAngularDamping() );

    // Reset velocities
    // ----------------
    m_body->setLinearVelocity (btVector3(0.0f,0.0f,0.0f));
    m_body->setAngularVelocity(btVector3(0.0f,0.0f,0.0f));

    // Create the actual vehicle
    // -------------------------
    m_vehicle_raycaster =
        new btDefaultVehicleRaycaster(World::getWorld()->getPhysics()->getPhysicsWorld());
    m_tuning  = new btKart::btVehicleTuning();
    m_tuning->m_maxSuspensionTravelCm = m_kart_properties->getSuspensionTravelCM();
    m_vehicle = new btKart(*m_tuning, m_body, m_vehicle_raycaster,
                           m_kart_properties->getTrackConnectionAccel());

    // never deactivate the vehicle
    m_body->setActivationState(DISABLE_DEACTIVATION);
    m_vehicle->setCoordinateSystem(/*right: */ 0,  /*up: */ 1,  /*forward: */ 2);

    // Add wheels
    // ----------
    float wheel_radius    = m_kart_properties->getWheelRadius();
    float suspension_rest = m_kart_properties->getSuspensionRest();

    btVector3 wheel_direction(0.0f, -1.0f, 0.0f);
    btVector3 wheel_axle(-1.0f, 0.0f, 0.0f);

    for(unsigned int i=0; i<4; i++)
    {
        bool is_front_wheel = i<2;
        btWheelInfo& wheel = m_vehicle->addWheel(
                            m_kart_model->getWheelPhysicsPosition(i),
                            wheel_direction, wheel_axle, suspension_rest,
                            wheel_radius, *m_tuning, is_front_wheel);
        wheel.m_suspensionStiffness      = m_kart_properties->getSuspensionStiffness();
        wheel.m_wheelsDampingRelaxation  = m_kart_properties->getWheelDampingRelaxation();
        wheel.m_wheelsDampingCompression = m_kart_properties->getWheelDampingCompression();
        wheel.m_frictionSlip             = m_kart_properties->getFrictionSlip();
        wheel.m_rollInfluence            = m_kart_properties->getRollInfluence();
    }
    // Obviously these allocs have to be properly managed/freed
    btTransform t;
    t.setIdentity();
    m_uprightConstraint=new btUprightConstraint(this, t);
    m_uprightConstraint->setLimit(m_kart_properties->getUprightTolerance());
    m_uprightConstraint->setBounce(0.0f);
    m_uprightConstraint->setMaxLimitForce(m_kart_properties->getUprightMaxForce());
    m_uprightConstraint->setErp(1.0f);
    m_uprightConstraint->setLimitSoftness(1.0f);
    m_uprightConstraint->setDamping(0.0f);
    World::getWorld()->getPhysics()->addKart(this);

}   // createPhysics

void Kart::fly()
{
    m_flying = true;
    Moveable::fly();
    //m_vehicle->m_flying = true;
}

// ----------------------------------------------------------------------------
/** Starts the engine sound effect. Called once the track intro phase is over.
 */
void Kart::startEngineSFX()
{
  /*  if(m_engine_sound)
    {
        m_engine_sound->speed(0.6f);
        m_engine_sound->setLoop(true);
        m_engine_sound->play();
    }*/
}   // startEngineSFX

// ----------------------------------------------------------------------------
/** The destructor frees the memory of this kart, but note that the actual kart
 *  model is still stored in the kart_properties (m_kart_model variable), so 
 *  it is not reloaded).
 */
Kart::~Kart()
{
    // Delete all custom sounds (TODO: add back when properly done)
    /*
    for (int n = 0; n < SFXManager::NUM_CUSTOMS; n++)
    {
        if (m_custom_sounds[n] != NULL)
            sfx_manager->deleteSFX(m_custom_sounds[n]);
    }*/

    //sfx_manager->deleteSFX(m_engine_sound );
    //sfx_manager->deleteSFX(m_crash_sound  );
    //sfx_manager->deleteSFX(m_skid_sound   );
    //sfx_manager->deleteSFX(m_goo_sound    );
    //if(m_terrain_sound)          sfx_manager->deleteSFX(m_terrain_sound);
    //if(m_previous_terrain_sound) sfx_manager->deleteSFX(m_previous_terrain_sound);
    if(m_smoke_system)        delete m_smoke_system;
    if(m_water_splash_system) delete m_water_splash_system;
    if(m_nitro)               delete m_nitro;
    if(m_slipstream)          delete m_slipstream;

    delete m_shadow;

    if(m_skidmarks) delete m_skidmarks ;

    World::getWorld()->getPhysics()->removeKart(this);
    delete m_vehicle;
    delete m_tuning;
    delete m_vehicle_raycaster;
    delete m_uprightConstraint;
    for(int i=0; i<m_kart_chassis.getNumChildShapes(); i++)
    {
        delete m_kart_chassis.getChildShape(i);
    }
    delete m_slipstream_original_quad;
    delete m_slipstream_quad;
    delete m_kart_model;
}   // ~Kart

//-----------------------------------------------------------------------------
/** Returns true if the kart is 'resting', i.e. (nearly) not moving.
 */
bool Kart::isInRest() const
{
    return fabs(m_body->getLinearVelocity ().y())<0.2;
}  // isInRest

//-----------------------------------------------------------------------------
/** Multiplies the velocity of the kart by a factor f (both linear
 *  and angular). This is used by anvils, which suddenly slow down the kart
 *  when they are attached.
 */
void Kart::adjustSpeed(float f)
{
    m_body->setLinearVelocity(m_body->getLinearVelocity()*f);
    m_body->setAngularVelocity(m_body->getAngularVelocity()*f);
}   // adjustSpeed

//-----------------------------------------------------------------------------
/** Caps the speed at a given value. If necessary the kart will 
 *  instantaneously change its speed.
 *  \param max_speed Maximum speed of the kart.
 */
void Kart::capSpeed(float max_speed)
{
    if ( m_speed >  max_speed && isOnGround() )
    {
        const float velocity_ratio = max_speed/m_speed;
        btVector3 velocity         = getBody()->getLinearVelocity();
        velocity *= velocity_ratio;
        getVehicle()->getRigidBody()->setLinearVelocity( velocity );
    }

}   // capSpeed

//-----------------------------------------------------------------------------
/** This method is to be called every time the mass of the kart is updated,
 *  which includes attaching an anvil to the kart (and detaching).
 */
void Kart::updatedWeight()
{
    // getMass returns the mass increased by the attachment
    btVector3 inertia;
    float m=getMass();
    m_kart_chassis.calculateLocalInertia(m, inertia);
    m_body->setMassProps(m, inertia);
}   // updatedWeight

//-----------------------------------------------------------------------------
/** Reset before a new race. It will remove all attachments, and
 *  puts the kart back at its original start position.
 */
void Kart::reset()
{
    EmergencyAnimation::reset();
    MaxSpeed::reset();
    if (m_camera)
    {
        m_camera->reset();
        m_camera->setInitialTransform();
    }
    
    // Stop any animations currently being played.
    m_kart_model->setAnimation(KartModel::AF_DEFAULT);
    // If the controller was replaced (e.g. replaced by end controller), 
    //  restore the original controller. 
    if(m_saved_controller)
    {
        m_controller       = m_saved_controller;
        m_saved_controller = NULL;
    }
    m_kart_model->setAnimation(KartModel::AF_DEFAULT);
    m_view_blocked_by_plunger = 0.0;
    m_attachment->clear();
    m_nitro->setCreationRate(0.0f);
    m_powerup.reset();

    m_race_position        = m_initial_position;
    m_finished_race        = false;
    m_finish_time          = 0.0f;
    m_collected_energy     = 0;
    m_has_started          = false;
    m_wheel_rotation       = 0;
    m_bounce_back_time     = 0.0f;
    m_skidding             = 1.0f;
    m_time_last_crash      = 0.0f;
    m_slipstream_mode      = SS_NONE;
    m_last_material        = NULL;
    //if(m_terrain_sound)
    {
       // sfx_manager->deleteSFX(m_terrain_sound);
    }
   // if(m_previous_terrain_sound)
    {
        //sfx_manager->deleteSFX(m_previous_terrain_sound);
    }
    
   // m_terrain_sound = NULL;
   // m_previous_terrain_sound = NULL;
    
  /*  if(m_engine_sound)
        m_engine_sound->stop();*/

    m_controls.m_steer     = 0.0f;
    m_controls.m_accel     = 0.0f;
    m_controls.m_brake     = false;
    m_controls.m_nitro     = false;
    m_controls.m_drift     = false;
    m_controls.m_fire      = false;
    m_controls.m_look_back = false;
    m_slipstream->reset();
    m_vehicle->deactivateZipper();

    // Set the brakes so that karts don't slide downhill
    for(int i=0; i<4; i++) m_vehicle->setBrake(5.0f, i);

    setTrans(m_reset_transform);

    applyEngineForce (0.0f);

    Moveable::reset();
    if(m_skidmarks) m_skidmarks->reset();
    for(int j=0; j<m_vehicle->getNumWheels(); j++)
    {
        m_vehicle->updateWheelTransform(j, true);
    }

    TerrainInfo::update(getXYZ());

    // Reset is also called when the kart is created, at which time
    // m_controller is not yet defined, so this has to be tested here.
    if(m_controller)
        m_controller->reset();

}   // reset

//-----------------------------------------------------------------------------
/** Sets that this kart has finished the race and finishing time. It also
 *  notifies the race_manager about the race completion for this kart.
 *  \param time The finishing time for this kart. It can either be the
 *         actual time when the kart finished (in which case time() = 
 *         world->getTime()), or the estimated time in case that all
 *         player kart have finished the race and all AI karts get
 *         an estimated finish time set.
 */
void Kart::finishedRace(float time)
{
    // m_finished_race can be true if e.g. an AI kart was set to finish
    // because the race was over (i.e. estimating the finish time). If
    // this kart then crosses the finish line (with the end controller)
    // it would trigger a race end again.
    if(m_finished_race) return;
    m_finished_race = true;
    m_finish_time   = time;
    m_controller->finishedRace(time);
    race_manager->kartFinishedRace(this, time);
    
    if (race_manager->getMinorMode() == RaceManager::MINOR_MODE_NORMAL_RACE ||
        race_manager->getMinorMode() == RaceManager::MINOR_MODE_TIME_TRIAL)
    {
        // in modes that support it, start end animation
        setController(new EndController(this, m_controller->getPlayer()));
        if(m_race_position<=0.5f*race_manager->getNumberOfKarts() ||
            m_race_position==1)
            m_kart_model->setAnimation(KartModel::AF_WIN_START);
        else 
            m_kart_model->setAnimation(KartModel::AF_LOSE_START);
        
        // Not all karts have a camera
        if (m_camera) m_camera->setMode(Camera::CM_FINAL);
        
        RaceGUIBase* m = World::getWorld()->getRaceGUI();
        if(m)
        {
            m->addMessage((getPosition() == 1 ? _("You won the race!") : _("You finished the race!")) ,
                          this, 2.0f, 60);
        }
    }
    else if (race_manager->getMinorMode() == RaceManager::MINOR_MODE_FOLLOW_LEADER)
    {        
        // start end animation
        setController(new EndController(this, m_controller->getPlayer()));
        if(m_race_position<=2)
            m_kart_model->setAnimation(KartModel::AF_WIN_START);
        else if(m_race_position>=0.7f*race_manager->getNumberOfKarts())
            m_kart_model->setAnimation(KartModel::AF_LOSE_START);
            
        // Not all karts have a camera
        if (m_camera) m_camera->setMode(Camera::CM_REVERSE);
        
        RaceGUIBase* m = World::getWorld()->getRaceGUI();
        if(m)
        {
            m->addMessage((getPosition() == 2 ? _("You won the race!") : _("You finished the race!")) ,
                          this, 2.0f, 60);
        }
    }
    else if (race_manager->getMinorMode() == RaceManager::MINOR_MODE_3_STRIKES)
    {
    }             
        
}   // finishedRace

//-----------------------------------------------------------------------------
/** Called when an item is collected. It will either adjust the collected
 *  energy, or update the attachment or powerup for this kart.
 *  \param item The item that was hit.
 *  \param add_info Additional info, used in networking games to force
 *         a specific item to be used (instead of a random item) to keep
 *         all karts in synch.
 */
void Kart::collectedItem(Item *item, int add_info)
{
    float old_energy          = m_collected_energy;
    const Item::ItemType type = item->getType();

    switch (type)
    {
    case Item::ITEM_BANANA: 
        m_attachment->hitBanana(item, add_info); 
        break;
    case Item::ITEM_NITRO_SMALL: 
        m_collected_energy += m_kart_properties->getNitroSmallContainer();
        break;
    case Item::ITEM_NITRO_BIG:   
        m_collected_energy += m_kart_properties->getNitroBigContainer();
        break;
    case Item::ITEM_BONUS_BOX  :
        {
            m_powerup.hitBonusBox(*item, add_info);
            break;
        }
    case Item::ITEM_BUBBLEGUM:
        // slow down
       /* m_body->setLinearVelocity(m_body->getLinearVelocity()*0.3f);
        m_goo_sound->position(getXYZ());
        m_goo_sound->play();*/
        // Play appropriate custom character sound
        playCustomSFX(SFXManager::CUSTOM_GOO);
        break;
    default        : break;
    }   // switch TYPE

    // Attachments and powerups are stored in the corresponding
    // functions (hit{Red,Green}Item), so only coins need to be
    // stored here.
    if(network_manager->getMode()==NetworkManager::NW_SERVER &&
        (type==Item::ITEM_NITRO_BIG || type==Item::ITEM_NITRO_SMALL) )
    {
        race_state->itemCollected(getWorldKartId(), item->getItemId());
    }

    if ( m_collected_energy > MAX_ITEMS_COLLECTED )
        m_collected_energy = MAX_ITEMS_COLLECTED;
    m_controller->collectedItem(*item, add_info, old_energy);

}   // collectedItem

//-----------------------------------------------------------------------------
/** Simulates gears by adjusting the force of the engine. It also takes the
 *  effect of the zipper into account.
 */
float Kart::getActualWheelForce()
{
    float time_left = MaxSpeed::getSpeedIncreaseTimeLeft(MS_INCREASE_ZIPPER);
    float zipper_force = time_left>0.0f ? m_kart_properties->getZipperForce(): 0.0f;
    const std::vector<float>& gear_ratio=m_kart_properties->getGearSwitchRatio();
    for(unsigned int i=0; i<gear_ratio.size(); i++)
    {
        if(m_speed <= m_kart_properties->getMaxSpeed()*gear_ratio[i])
        {
            return getMaxPower()*m_kart_properties->getGearPowerIncrease()[i]
                  +zipper_force;
        }
    }
    return getMaxPower()+zipper_force;

}   // getActualWheelForce

//-----------------------------------------------------------------------------
/** The kart is on ground if all 4 wheels touch the ground, and if no special
 *  animation (rescue, explosion etc.) is happening).
 */
bool Kart::isOnGround() const
{
    return (m_vehicle->getNumWheelsOnGround() == m_vehicle->getNumWheels()
          && !playingEmergencyAnimation());
}   // isOnGround

//-----------------------------------------------------------------------------
/** The kart is near the ground, but not necesarily on it (small jumps). This
 *  is used to determine when to switch off the upright constraint, so that
 *  explosions can be more violent, while still
*/
bool Kart::isNearGround() const
{
    if(getHoT()==Track::NOHIT)
        return false;
    else
        return ((getXYZ().getY() - getHoT()) < stk_config->m_near_ground);
}   // isNearGround

//-----------------------------------------------------------------------------
/** Updates the kart in each time step. It updates the physics setting,
 *  particle effects, camera position, etc.
 *  \param dt Time step size.
 */
void Kart::update(float dt)
{
    // Update the position and other data taken from the physics    
    Moveable::update(dt);

    if(!history->replayHistory())
        m_controller->update(dt);
    if(m_camera)
        m_camera->update(dt);
    // if its view is blocked by plunger, decrease remaining time
    if(m_view_blocked_by_plunger > 0) m_view_blocked_by_plunger -= dt;

    // Store the actual kart controls at the start of update in the server
    // state. This makes it easier to reset some fields when they are not used
    // anymore (e.g. controls.fire).
    if(network_manager->getMode()==NetworkManager::NW_SERVER)
    {
        race_state->storeKartControls(*this);
    }

    // On a client fiering is done upon receiving the command from the server.
    if ( m_controls.m_fire && network_manager->getMode()!=NetworkManager::NW_CLIENT
        && !playingEmergencyAnimation())
    {
        // use() needs to be called even if there currently is no collecteable
        // since use() can test if something needs to be switched on/off.
        m_powerup.use() ;
        m_controls.m_fire = false;
    }

    if (!m_flying)
    {
        // When really on air, free fly, when near ground, try to glide / adjust for landing
        // If zipped, be stable, so ramp+zipper can allow nice jumps without scripting the fly
        if(!isNearGround() && 
            MaxSpeed::getSpeedIncreaseTimeLeft(MS_INCREASE_ZIPPER)<=0.0f )
            m_uprightConstraint->setLimit(M_PI);
        else
            m_uprightConstraint->setLimit(m_kart_properties->getUprightTolerance());
    }
    
    // TODO: hiker said this probably will be moved to btKart or so when updating bullet engine.
    // Neutralize any yaw change if the kart leaves the ground, so the kart falls more or less
    // straight after jumping, but still allowing some "boat shake" (roll and pitch).
    // Otherwise many non perfect jumps end in a total roll over or a serious change of
    // direction, sometimes 90 or even full U turn (real but less fun for a karting game).
    // As side effect steering becames a bit less responsive (any wheel on air), but not too bad.
    if(!isOnGround()) {
        btVector3 speed = m_body->getAngularVelocity();
        speed.setX(speed.getX() * 0.95f);
        speed.setY(speed.getY() * 0.25f); // or 0.0f for sharp neutralization of yaw
        speed.setZ(speed.getZ() * 0.95f);
        m_body->setAngularVelocity(speed);
        // This one keeps the kart pointing "100% as launched" instead,
        // like in ski jump sports, too boring but also works.
        //m_body->setAngularVelocity(btVector3(0,0,0));
    }

    //m_wheel_rotation gives the rotation around the X-axis, and since velocity's
    //timeframe is the delta time, we don't have to multiply it with dt.
    m_wheel_rotation += m_speed*dt / m_kart_properties->getWheelRadius();
    m_wheel_rotation=fmodf(m_wheel_rotation, 2*M_PI);

    EmergencyAnimation::update(dt);

    m_attachment->update(dt);

    //smoke drawing control point
    if ( UserConfigParams::m_graphical_effects )
    {
        m_smoke_system->update(dt);
        m_water_splash_system->update(dt);
        m_nitro->update(dt);
        m_slipstream->update(dt);
    }  // UserConfigParams::m_graphical_effects

    updatePhysics(dt);

    /* (TODO: add back when properly done)
    for (int n = 0; n < SFXManager::NUM_CUSTOMS; n++)
    {
        if (m_custom_sounds[n] != NULL) m_custom_sounds[n]->position   ( getXYZ() );
    }
     */
    
    //m_beep_sound->position   ( getXYZ() );
    //m_engine_sound->position ( getXYZ() );
    //m_crash_sound->position  ( getXYZ() );
    //m_skid_sound->position   ( getXYZ() );

    // Check if a kart is (nearly) upside down and not moving much --> automatic rescue
    if((fabs(getRoll())>60*DEGREE_TO_RAD && fabs(getSpeed())<3.0f) )
    {
        forceRescue();
    }

    btTransform trans=getTrans();
    // Add a certain epsilon (0.3) to the height of the kart. This avoids
    // problems of the ray being cast from under the track (which happened
    // e.g. on tux tollway when jumping down from the ramp, when the chassis
    // partly tunnels through the track). While tunneling should not be
    // happening (since Z velocity is clamped), the epsilon is left in place
    // just to be on the safe side (it will not hit the chassis itself).
    Vec3 pos_plus_epsilon = trans.getOrigin()+btVector3(0,0.3f,0);

    // Make sure that the ray doesn't hit the kart. This is done by
    // resetting the collision filter group, so that this collision
    // object is ignored during raycasting.
    short int old_group = 0;
    if(m_body->getBroadphaseHandle())
    {
        old_group = m_body->getBroadphaseHandle()->m_collisionFilterGroup;
        m_body->getBroadphaseHandle()->m_collisionFilterGroup = 0;
    }
    TerrainInfo::update(pos_plus_epsilon);
    if(m_body->getBroadphaseHandle())
    {
        m_body->getBroadphaseHandle()->m_collisionFilterGroup = old_group;
    }
    const Material* material=TerrainInfo::getMaterial();
    if (getHoT()==Track::NOHIT)   // kart falling off the track
    {
        // let kart fall a bit before rescuing
        const Vec3 *min, *max;
        World::getWorld()->getTrack()->getAABB(&min, &max);
        if(min->getY() - getXYZ().getY() > 17)
            forceRescue();
    }

    // Sometimes the material can be 0. This can happen if a kart is above
    // another kart (e.g. mass collision, or one kart falling on another
    // kart). Bullet does not have any triangle information in this case,
    // and so material can not be set. In this case it is simply ignored
    // since it can't hurt (material is only used for friction, zipper and
    // rescue, so those things are not triggered till the kart is on the
    // track again)
    else if(material)
    {
        // If a terrain specific sfx is already being played, when a new
        // terrain is entered, an old sfx should be finished (once, not
        // looped anymore of course). The m_terrain_sound is then copied
        // to a m_previous_terrain_sound, for which looping is disabled.
        // In case that three sfx needed to be played (i.e. a previous is
        // playing, a current is playing, and a new terrain with sfx is
        // entered), the oldest (previous) sfx is stopped and deleted.
        if(m_last_material!=material)
        {
            // First stop any previously playing terrain sound
            // and remove it, sp that m_previous_terrain_sound
            // can be used again.
           /* if(m_previous_terrain_sound)
            {
                sfx_manager->deleteSFX(m_previous_terrain_sound);
            }
            m_previous_terrain_sound = m_terrain_sound;
            if(m_previous_terrain_sound)
                m_previous_terrain_sound->setLoop(false);*/

            const std::string s = material->getSFXName();
            if(s!="")
            {
              /*  m_terrain_sound = sfx_manager->createSoundSource(s);
                m_terrain_sound->play();
                m_terrain_sound->setLoop(true);*/
            }
           /* else
                m_terrain_sound = NULL;*/
        }
        //if(m_previous_terrain_sound && 
        //    m_previous_terrain_sound->getStatus()==SFXManager::SFX_STOPED)
        //{
        //    // We don't modify the position of m_previous_terrain_sound 
        //    // anymore, so that it keeps on playing at the place where the 
        //    // kart left the material.
        //    sfx_manager->deleteSFX(m_previous_terrain_sound);
        //    m_previous_terrain_sound = NULL;
        //}
        //if(m_terrain_sound) 
        //{
        //    m_terrain_sound->position(getXYZ());
        //    material->setSFXSpeed(m_terrain_sound, m_speed);
        //}
        m_last_material = material;

        if     (material->isReset()  && isOnGround()) forceRescue();
        else if(material->isZipper() && isOnGround()) handleZipper();
        else
        {
            MaxSpeed::setSlowdown(MaxSpeed::MS_DECREASE_TERRAIN,
                                  material->getMaxSpeedFraction(), 
                                  material->getSlowDownTime()     );
#ifdef DEBUG
            if(UserConfigParams::m_material_debug)
            {
                printf("%s\tfraction %f\ttime %f.\n",
                       material->getTexFname().c_str(),
                       material->getMaxSpeedFraction(),
                       material->getSlowDownTime()       );
            }
#endif
        }
    }   // if there is material

    // Check if any item was hit.
    item_manager->checkItemHit(this);
    if(m_kart_properties->hasSkidmarks())
        m_skidmarks->update(dt);

    // Remove the shadow if the kart is not on the ground (if a kart
    // is rescued isOnGround might still be true, since the kart rigid
    // body was removed from the physics, but still retain the old
    // values for the raycasts).
    if( (!isOnGround() || playingEmergencyAnimation()) && m_shadow_enabled)
    {
        m_shadow_enabled = false;
        m_shadow->disableShadow();
    }
    if(!m_shadow_enabled && isOnGround() && !playingEmergencyAnimation())
    {
        m_shadow->enableShadow();
        m_shadow_enabled = true;
    }
}   // update

//-----------------------------------------------------------------------------
/** Sets zipper time, and apply one time additional speed boost.
 */
void Kart::handleZipper()
{
    // Ignore a zipper that's activated while braking
    if(m_controls.m_brake) return;

    MaxSpeed::increaseMaxSpeed(MaxSpeed::MS_INCREASE_ZIPPER, 
                               m_kart_properties->getZipperMaxSpeedIncrease(),
                               m_kart_properties->getZipperTime(),
                               /*fade_out_time*/ 3.0f);
    // This will result in all max speed settings updated, but no 
    // changes to any slow downs since dt=0
    MaxSpeed::update(0);
    float speed = std::min(m_speed+m_kart_properties->getZipperSpeedGain(),
                           MaxSpeed::getCurrentMaxSpeed()                   );

    m_vehicle->activateZipper(speed);
    // Play custom character sound (weee!)
    playCustomSFX(SFXManager::CUSTOM_ZIPPER);
    m_controller->handleZipper();
}   // handleZipper

// -----------------------------------------------------------------------------
/** Returned an additional engine power boost when using nitro.
 *  \param dt Time step size.
 */
float Kart::handleNitro(float dt)
{
    if(!m_controls.m_nitro) return 0.0;
    m_collected_energy -= dt * m_kart_properties->getNitroConsumption();
    if(m_collected_energy<0)
    {
        m_collected_energy = 0;
        return 0.0;
    }
    MaxSpeed::increaseMaxSpeed(MaxSpeed::MS_INCREASE_NITRO,
                               m_kart_properties->getNitroMaxSpeedIncrease(),
                               m_kart_properties->getNitroDuration(), 
                               m_kart_properties->getNitroFadeOutTime()      );
    return m_kart_properties->getNitroPowerBoost() * getMaxPower();

}   // handleNitro

//-----------------------------------------------------------------------------
/** This function manages slipstreaming. It adds up the time a kart was
 *  slipstreaming, and if a kart was slipstreaming long enough, it will
 *  add power to the kart for a certain amount of time.
 */
float Kart::handleSlipstream(float dt)
{
    // Update this karts slipstream quad (even for low level AI which don't
    // use slipstream, since even then player karts can get slipstream, 
    // and so have to compare with the modified slipstream quad.
    m_slipstream_original_quad->transform(getTrans(), m_slipstream_quad);

    // Low level AIs should not do any slipstreaming.
    if(!getController()->isPlayerController() &&
        race_manager->getDifficulty()==RaceManager::RD_EASY) return 0;

    // First see if we are currently using accumulated slipstream credits:
    // -------------------------------------------------------------------
    if(m_slipstream_mode==SS_USE)
    {
        m_slipstream_time -= dt;
        if(m_slipstream_time<0) m_slipstream_mode=SS_NONE;
        m_slipstream->setIntensity(2.0f, NULL);
        MaxSpeed::increaseMaxSpeed(MaxSpeed::MS_INCREASE_SLIPSTREAM,
                    m_kart_properties->getSlipstreamMaxSpeedIncrease(),
                    m_kart_properties->getSlipstreamDuration(),
                    m_kart_properties->getSlipstreamFadeOutTime()       );
        return m_kart_properties->getSlipstreamAddPower();
    }

    // If this kart is too slow for slipstreaming taking effect, do nothing
    // --------------------------------------------------------------------

    // Define this to get slipstream effect shown even when the karts are
    // not moving. This is useful for debugging the graphics of SS-ing.
#undef DISPLAY_SLIPSTREAM_WITH_0_SPEED_FOR_DEBUGGING
#ifndef DISPLAY_SLIPSTREAM_WITH_0_SPEED_FOR_DEBUGGING
    if(getSpeed()<m_kart_properties->getSlipstreamMinSpeed())
    {
        m_slipstream->setIntensity(0, NULL);
        return 0;
    }
#endif
    // Then test if this kart is in the slipstream range of another kart:
    // ------------------------------------------------------------------
    World *world           = World::getWorld();
    unsigned int num_karts = world->getNumKarts();
    bool is_sstreaming     = false;
    m_slipstream_target    = NULL;
    // Note that this loop can not be simply replaced with a shorter loop
    // using only the karts with a better position - since a kart might
    // be a lap behind
    for(unsigned int i=0; i<num_karts; i++)
    {
        m_slipstream_target= world->getKart(i);
        // Don't test for slipstream with itself.
        if(m_slipstream_target==this            || 
            m_slipstream_target->isEliminated()    ) continue;

        // If the kart we are testing against is too slow, no need to test
        // slipstreaming. Note: We compare the speed of the other kart 
        // against the minimum slipstream speed kart of this kart - not 
        // entirely sure if this makes sense, but it makes it easier to 
        // give karts different slipstream properties.
#ifndef DISPLAY_SLIPSTREAM_WITH_0_SPEED_FOR_DEBUGGING
        if(m_slipstream_target->getSpeed() <
            m_kart_properties->getSlipstreamMinSpeed()) 
            continue;
#endif
        // Quick test: the kart must be not more than
        // slipstream length+kart_length() away from the other kart
        Vec3 delta = getXYZ() - m_slipstream_target->getXYZ();
        float l    = m_slipstream_target->m_kart_properties->getSlipstreamLength() 
                   + m_slipstream_target->getKartLength()*0.5f;
        if(delta.length2_2d() > l*l) continue;
        // Real test: if in slipstream quad of other kart
        if(m_slipstream_target->m_slipstream_quad->pointInQuad(getXYZ()))
        {
            is_sstreaming     = true;
            break;
        }
    }   // for i < num_karts

    if(!is_sstreaming)
    {
        if(isSlipstreamReady()) 
        {
            // The first time slipstream is ready after collecting
            // and you are leaving the slipstream area, you get a
            // zipper bonus. 
            if(m_slipstream_mode==SS_COLLECT)
            {
                m_slipstream_mode = SS_USE;
                handleZipper();
                // And the time is set to a maximum (to avoid that e.g. AI 
                // karts slipstream for a long time and then get a very long 
                // bonus).
                m_slipstream_time = 
                    m_kart_properties->getSlipstreamCollectTime();
                return m_kart_properties->getSlipstreamAddPower();
            }
        }
        m_slipstream_time -=dt;
        if(m_slipstream_time<0) m_slipstream_mode = SS_NONE;
        m_slipstream->setIntensity(0, NULL);
        return 0;
    }   // for i<number of karts

    // Accumulate slipstream credits now
    m_slipstream_time = m_slipstream_mode==SS_NONE ? dt 
                                                   : m_slipstream_time+dt;
    //printf("Collecting slipstream %f\n", m_slipstream_time);
    if(isSlipstreamReady())
        m_nitro->setCreationRate(3.0f);
    m_slipstream->setIntensity(m_slipstream_time, m_slipstream_target);

    m_slipstream_mode = SS_COLLECT;
    if(m_slipstream_time>m_kart_properties->getSlipstreamCollectTime())
    {
        m_slipstream->setIntensity(1.0f, m_slipstream_target);
        return 0;       // see below about abusing zipper without zipper
        //return m_kart_properties->getSlipstreamAddPower();
    }
    return 0;
   
}   // handleSlipstream

// -----------------------------------------------------------------------------
/** Returns true if enough slipstream credits have been accumulated
 *  to get a boost when leaving the slipstream area.
 */
bool Kart::isSlipstreamReady() const
{
    return m_slipstream_time>m_kart_properties->getSlipstreamCollectTime();
}   // isSlipstreamReady

// -----------------------------------------------------------------------------
/** This function is called when the race starts. Up to then all brakes are
    braking (to avoid the kart from rolling downhill), but they need to be set
    to zero (otherwise the brakes will be braking whenever no engine force
    is set, i.e. the kart is not accelerating).
    */
void Kart::resetBrakes()
{
    for(int i=0; i<4; i++) m_vehicle->setBrake(0.0f, i);
}   // resetBrakes

// -----------------------------------------------------------------------------
/** Called when the kart crashes against the track (k=NULL) or another kart.
 *  \params k Either a kart if a kart was hit, or NULL if the track was hit.
 */
void Kart::crashed(Kart *k)
{
    m_controller->crashed();
    /** If a kart is crashing against the track, the collision is often
     *  reported more than once, resulting in a machine gun effect, and too
     *  long disabling of the engine. Therefore, this reaction is disabled
     *  for 0.5 seconds after a crash.
     */
    if(World::getWorld()->getTime()-m_time_last_crash < 0.5f) return;

    m_time_last_crash = World::getWorld()->getTime();
    // After a collision disable the engine for a short time so that karts
    // can 'bounce back' a bit (without this the engine force will prevent
    // karts from bouncing back, they will instead stuck towards the obstable).
    if(m_bounce_back_time<=0.0f)
    {
        // In case that the sfx is longer than 0.5 seconds, only play it if
        // it's not already playing.
        //if(m_crash_sound->getStatus() != SFXManager::SFX_PLAYING)
        //    m_crash_sound->play();

        m_bounce_back_time = 0.1f;
    }
}   // crashed

// -----------------------------------------------------------------------------
void Kart::beep()
{
    // If the custom horn can't play (isn't defined) then play the default one
    //if (!playCustomSFX(SFXManager::CUSTOM_HORN))
    //    m_beep_sound->play();

} // beep

// -----------------------------------------------------------------------------
/*
    playCustomSFX()

    This function will play a particular character voice for this kart.  It
    returns whether or not a character voice sample exists for the particular
    event.  If there is no voice sample, a default can be played instead.

    Use entries from the CustomSFX enumeration as a parameter (see
    sfx_manager.hpp).  eg. playCustomSFX(SFXManager::CUSTOM_CRASH)

    Obviously we don't want a certain character voicing multiple phrases
    simultaneously.  It just sounds bad.  There are two ways of avoiding this:

    1.  If there is already a voice sample playing for the character
        don't play another until it is finished.

    2.  If there is already a voice sample playing for the character
        stop the sample, and play the new one.

    Currently we're doing #2.

    rforder

*/

bool Kart::playCustomSFX(unsigned int type)
{
    // (TODO: add back when properly done)
    return false;
    
    /*
    bool ret = false;

    // Stop all other character voices for this kart before playing a new one
    // we don't want overlapping phrases coming from the same kart
    for (unsigned int n = 0; n < SFXManager::NUM_CUSTOMS; n++)
    {
        if (m_custom_sounds[n] != NULL)
        {
            // If the sound we're trying to play is already playing
            // don't stop it, we'll just let it finish.
            if (type != n) m_custom_sounds[n]->stop();
        }
    }

    if (type < SFXManager::NUM_CUSTOMS) 
    {
        if (m_custom_sounds[type] != NULL)
        {
            ret = true;
            //printf("Kart SFX: playing %s for %s.\n", sfx_manager->getCustomTagName(type), m_kart_properties->getIdent().c_str());
            // If it's already playing, let it finish
            if (m_custom_sounds[type]->getStatus() != SFXManager::SFX_PLAYING)
            {
                m_custom_sounds[type]->play();
            }
        }
    }
    return ret;
     */
}
// ----------------------------------------------------------------------------
/** Updates the physics for this kart: computing the driving force, set 
 *  steering, handles skidding, terrain impact on kart, ...
 *  \param dt Time step size.
 */
void Kart::updatePhysics(float dt)
{
    // Check if accel is pressed for the first time. The actual timing
    // is done in getStartupBoost - it returns 0 if the start was actually
    // too slow to qualify for a boost.
    if(!m_has_started && m_controls.m_accel)
    {
        m_has_started = true;
        float f       = m_kart_properties->getStartupBoost();
        m_vehicle->activateZipper(f);
        MaxSpeed::increaseMaxSpeed(MS_INCREASE_ZIPPER, +10,
                                   5.0f, 5.0f);

    }

    m_bounce_back_time-=dt;
    float engine_power = getActualWheelForce() + handleNitro(dt)
                                               + handleSlipstream(dt);
    if(m_attachment->getType()==ATTACH_PARACHUTE) engine_power*=0.2f;

    if (m_flying)
    {
        if (m_controls.m_accel)
        {
            float orientation = getHeading();
            m_body->applyCentralImpulse(btVector3(60.0f*sin(orientation), 0.0, 60.0f*cos(orientation)));
        }
        if (m_controls.m_steer != 0.0f)
        {
            m_body->applyTorque(btVector3(0.0, m_controls.m_steer * 2000.0f, 0.0));
        }
        if (m_controls.m_brake)
        {
            float orientation = getHeading();
            m_body->applyCentralImpulse(btVector3(-60.0f*sin(orientation), 0.0, -60.0f*cos(orientation)));
        }
    }
    
    
    if(m_controls.m_accel)   // accelerating
    {
        // For a short time after a collision disable the engine,
        // so that the karts can bounce back a bit from the obstacle.
        if(m_bounce_back_time>0.0f)
            engine_power = 0.0f;
        // let a player going backwards accelerate quickly (e.g. if a player hits a
        // wall, he needs to be able to start again quickly after going backwards)
        else if(m_speed < 0.0f)
            engine_power *= 5.0f;

        // Lose some traction when skidding, to balance the adventage
        // Up to r5483 AIs were allowed to cheat in medium and high diff levels
        if(m_controls.m_drift)
            engine_power *= 0.5f;

        applyEngineForce(engine_power*m_controls.m_accel);

        // Either all or no brake is set, so test only one to avoid
        // resetting all brakes most of the time.
        if(m_vehicle->getWheelInfo(0).m_brake &&
            !World::getWorld()->isStartPhase())
            resetBrakes();
    }
    else
    {   // not accelerating
        if(m_controls.m_brake)
        {   // check if the player is currently only slowing down or moving backwards
            if(m_speed > 0.0f)
            {   // going forward
                applyEngineForce(0.f);

                //apply the brakes
                for(int i=0; i<4; i++) m_vehicle->setBrake(getBrakeFactor(), i);
                m_skidding*= 1.08f;//skid a little when the brakes are hit (just enough to make the skiding sound)
                if(m_skidding>m_kart_properties->getMaxSkid())
                    m_skidding=m_kart_properties->getMaxSkid();
            }
            else   // m_speed < 0
            {
                resetBrakes();
                // going backward, apply reverse gear ratio (unless he goes too fast backwards)
                if ( -m_speed <  MaxSpeed::getCurrentMaxSpeed()
                                 *m_kart_properties->getMaxSpeedReverseRatio() )
                {
                    // The backwards acceleration is artificially increased to
                    // allow players to get "unstuck" quicker if they hit e.g.
                    // a wall. 
                    applyEngineForce(-engine_power*2.5f);
                }
                else  // -m_speed >= max speed on this terrain
                {
                    applyEngineForce(0.0f);
                }

            }   // m_speed <00
        }
        else   // !m_brake
        {
            // lift the foot from throttle, brakes with 10% engine_power
            applyEngineForce(-m_controls.m_accel*engine_power*0.1f);

            // If not giving power (forward or reverse gear), and speed is low
            // we are "parking" the kart, so in battle mode we can ambush people, eg
            if(abs(m_speed) < 5.0f) {
                for(int i=0; i<4; i++) m_vehicle->setBrake(20.0f, i);
            }
        }   // !m_brake
    }   // not accelerating
#ifdef ENABLE_JUMP
    if(m_controls.jump && isOnGround())
    {
      //Vector3 impulse(0.0f, 0.0f, 10.0f);
      //        getVehicle()->getRigidBody()->applyCentralImpulse(impulse);
        btVector3 velocity         = m_body->getLinearVelocity();
        velocity.setZ( m_kart_properties->getJumpVelocity() );

        getBody()->setLinearVelocity( velocity );

    }
#endif
    if (isOnGround()){
        if((fabs(m_controls.m_steer) > 0.001f) && m_controls.m_drift)
        {
            m_skidding +=  m_kart_properties->getSkidIncrease()
                          *dt/m_kart_properties->getTimeTillMaxSkid();
            if(m_skidding>m_kart_properties->getMaxSkid())
                m_skidding=m_kart_properties->getMaxSkid();
        }
        else if(m_skidding>1.0f)
        {
            m_skidding *= m_kart_properties->getSkidDecrease();
            if(m_skidding<1.0f) m_skidding=1.0f;
        }
    }
    else
    {
        m_skidding = 1.0f; // Lose any skid factor as soon as we fly
    }
    if(m_skidding>1.0f)
    {
      /*  if(m_skid_sound->getStatus() != SFXManager::SFX_PLAYING &&
            m_kart_properties->hasSkidmarks())
            m_skid_sound->play();*/
    }
   /* else if(m_skid_sound->getStatus() == SFXManager::SFX_PLAYING)
    {
        m_skid_sound->stop();
    }*/
    float steering = getMaxSteerAngle() * m_controls.m_steer*m_skidding;

    m_vehicle->setSteeringValue(steering, 0);
    m_vehicle->setSteeringValue(steering, 1);

    // Only compute the current speed if this is not the client. On a client the
    // speed is actually received from the server.
    if(network_manager->getMode()!=NetworkManager::NW_CLIENT)
        m_speed = getVehicle()->getRigidBody()->getLinearVelocity().length();

    // calculate direction of m_speed
    const btTransform& chassisTrans = getVehicle()->getChassisWorldTransform();
    btVector3 forwardW (
               chassisTrans.getBasis()[0][2],
               chassisTrans.getBasis()[1][2],
               chassisTrans.getBasis()[2][2]);

    if (forwardW.dot(getVehicle()->getRigidBody()->getLinearVelocity()) < btScalar(0.))
        m_speed *= -1.f;

    // Cap speed if necessary
    MaxSpeed::update(dt);

    // To avoid tunneling (which can happen on long falls), clamp the
    // velocity in Y direction. Tunneling can happen if the Y velocity
    // is larger than the maximum suspension travel (per frame), since then
    // the wheel suspension can not stop/slow down the fall (though I am
    // not sure if this is enough in all cases!). So the speed is limited
    // to suspensionTravel / dt with dt = 1/60 (since this is the dt
    // bullet is using).
    // Only apply if near ground instead of purely based on speed avoiding
    // the "parachute on top" look.
    const Vec3 &v = m_body->getLinearVelocity();
    if(/*isNearGround() &&*/ v.getY() < - m_kart_properties->getSuspensionTravelCM()*0.01f*60)
    {
        Vec3 v_clamped = v;
        // clamp the speed to 99% of the maxium falling speed.
        v_clamped.setY(-m_kart_properties->getSuspensionTravelCM()*0.01f*60 * 0.99f);
        m_body->setLinearVelocity(v_clamped);
    }

    //at low velocity, forces on kart push it back and forth so we ignore this
    if(fabsf(m_speed) < 0.2f) // quick'n'dirty workaround for bug 1776883
         m_speed = 0;
	/*
    // when going faster, use higher pitch for engine
    if(m_engine_sound && sfx_manager->sfxAllowed())
    {
        if(isOnGround())
        {
            float max_speed = MaxSpeed::getCurrentMaxSpeed();
            // Engine noise is based half in total speed, half in fake gears:
            // With a sawtooth graph like /|/|/| we get 3 even spaced gears,
            // ignoring the gear settings from stk_config, but providing a
            // good enough brrrBRRRbrrrBRRR sound effect. Speed factor makes
            // it a "staired sawtooth", so more acoustically rich.
            float gears = 3.0f * fmod((float)(m_speed / max_speed), 0.333334f);
            m_engine_sound->speed(0.6f +
                                  (float)(m_speed / max_speed) * 0.35f +
                                  gears * 0.35f);
        }
        else
        {
            // When flying, fixed value but not too high pitch
            // This gives some variation (vs previous "on wheels" one)
            m_engine_sound->speed(0.9f);
        }
        m_engine_sound->position(getXYZ());
    }*/
#ifdef XX
    printf("forward %f %f %f %f  side %f %f %f %f angVel %f %f %f heading %f\n"
       ,m_vehicle->m_forwardImpulse[0]
       ,m_vehicle->m_forwardImpulse[1]
       ,m_vehicle->m_forwardImpulse[2]
       ,m_vehicle->m_forwardImpulse[3]
       ,m_vehicle->m_sideImpulse[0]
       ,m_vehicle->m_sideImpulse[1]
       ,m_vehicle->m_sideImpulse[2]
       ,m_vehicle->m_sideImpulse[3]
       ,m_body->getAngularVelocity().getX()
       ,m_body->getAngularVelocity().getY()
       ,m_body->getAngularVelocity().getZ()
       ,getHeading()
       );
#endif
    
}   // updatePhysics


//-----------------------------------------------------------------------------
/** Attaches the right model, creates the physics and loads all special 
 *  effects (particle systems etc.)
 */
void Kart::loadData()
{
    m_kart_model->attachModel(&m_node);
    // Attachment must be created after attachModel, since only then the
    // scene node will exist (to which the attachment is added). But the
    // attachment is needed in createPhysics (which gets the mass, which
    // is dependent on the attachment).
    m_attachment = new Attachment(this);
    createPhysics();

    // Attach Particle System
    m_smoke_system        = new Smoke(this);
    m_water_splash_system = new WaterSplash(this);
    m_nitro               = new Nitro(this);
    m_slipstream          = new SlipStream(this);

    if(m_kart_properties->hasSkidmarks())
        m_skidmarks = new SkidMarks(*this);
       
    m_shadow = new Shadow(m_kart_properties->getShadowTexture(),
                          m_node);
}   // loadData

//-----------------------------------------------------------------------------
/** Stores the current suspension length. This function is called from world
 *  after all karts are in resting position (see World::resetAllKarts), so
 *  that the default suspension rest length can be stored. This is then used
 *  later to move the wheels depending on actual suspension, so that when
 *  a kart is in rest, the wheels are at the position at which they were
 *  modelled.
 */
void Kart::setSuspensionLength()
{
    for(unsigned int i=0; i<4; i++)
    {
        m_default_suspension_length[i] =
            m_vehicle->getWheelInfo(i).m_raycastInfo.m_suspensionLength;
    }   // for i
}   // setSuspensionLength

//-----------------------------------------------------------------------------
/** Applies engine power to all the wheels that are traction capable,
 *  so other parts of code do not have to be adjusted to simulate different
 *  kinds of vehicles in the general case, only if they are trying to
 *  simulate traction control, diferentials or multiple independent electric
 *  engines, they will have to tweak the power in a per wheel basis.
 */
void Kart::applyEngineForce(float force)
{
    // Split power to simulate a 4WD 40-60, other values possible
    // FWD or RWD is a matter of putting a 0 and 1 in the right place
    float frontForce = force*0.4f;
    float rearForce = force*0.6f;
    // Front wheels
    for(unsigned int i=0; i<2; i++)
    {
        m_vehicle->applyEngineForce (frontForce, i);
    }
    // Rear wheels
    for(unsigned int i=2; i<4; i++)
    {
        m_vehicle->applyEngineForce (rearForce, i);
    }
}   // applyEngineForce

//-----------------------------------------------------------------------------
/** Updates the graphics model. Mainly set the graphical position to be the
 *  same as the physics position, but uses offsets to position and rotation
 *  for special gfx effects (e.g. skidding will turn the karts more). These
 *  variables are actually not used here atm, but are defined here and then
 *  used in Moveable.
 *  \param offset_xyz Offset to be added to the position.
 *  \param rotation Additional rotation.
 */
void Kart::updateGraphics(const Vec3& offset_xyz, 
                          const btQuaternion& rotation)
{
    float wheel_up_axis[4];
    for(unsigned int i=0; i<4; i++)
    {
        // Set the suspension length
        wheel_up_axis[i] = m_default_suspension_length[i]
                         - m_vehicle->getWheelInfo(i).m_raycastInfo.m_suspensionLength;
    }
    m_kart_model->update(m_wheel_rotation, getSteerPercent(), wheel_up_axis);

    Vec3        center_shift  = m_kart_properties->getGravityCenterShift();
    float y = m_vehicle->getWheelInfo(0).m_chassisConnectionPointCS.getY()
            - m_default_suspension_length[0]
            - m_vehicle->getWheelInfo(0).m_wheelsRadius
            - (m_kart_model->getWheelGraphicsRadius(0)
               -m_kart_model->getWheelGraphicsPosition(0).getY() );
    center_shift.setY(y);

    if(m_smoke_system)
    {
        float f=0.0f;
        if(getMaterial() && getMaterial()->hasSmoke() && 
            fabsf(m_controls.m_steer) > 0.8           &&
            isOnGround()                                  )
            f=250.0f;
        m_smoke_system->setCreationRate((m_skidding-1)*f);
    }
    if(m_water_splash_system)
    {
        float f = getMaterial() && getMaterial()->hasWaterSplash() && isOnGround()
                ? sqrt(getSpeed())*40.0f
                : 0.0f;
        m_water_splash_system->setCreationRate(f);
    }
    if(m_nitro)
        // fabs(speed) is important, otherwise the negative number will
        // become a huge unsigned number in the particle scene node!
        m_nitro->setCreationRate(m_controls.m_nitro && m_collected_energy>0
                                 ? (10.0f + fabsf(getSpeed())*20.0f) : 0);

    // For testing purposes mis-use the nitro graphical effects to show
    // then the slipstream becomes usable.
    if(m_slipstream_mode == SS_USE)
        m_nitro->setCreationRate(20.0f);

    float speed_ratio    = getSpeed()/MaxSpeed::getCurrentMaxSpeed();
    float offset_heading = getSteerPercent()*m_kart_properties->getSkidVisual()
                         * speed_ratio * m_skidding*m_skidding;
    Moveable::updateGraphics(center_shift, 
                             btQuaternion(offset_heading, 0, 0));
}   // updateGraphics

/* EOF */
