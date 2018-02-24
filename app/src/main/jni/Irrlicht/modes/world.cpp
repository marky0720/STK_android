//  $Id: world.cpp 6338 2010-10-26 06:14:46Z hikerstk $
//
//  SuperTuxKart - a fun racing game with go-kart
//  Copyright (C) 2006 SuperTuxKart-Team
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

#include "modes/world.hpp"

#include <assert.h>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <ctime>

#include "audio/music_manager.hpp"
#include "audio/sfx_base.hpp"
#include "audio/sfx_manager.hpp"
#include "challenges/unlock_manager.hpp"
#include "config/user_config.hpp"
#include "graphics/camera.hpp"
#include "io/file_manager.hpp"
#include "items/projectile_manager.hpp"
#include "karts/controller/default_ai_controller.hpp"
#include "karts/controller/new_ai_controller.hpp"
#include "karts/controller/player_controller.hpp"
#include "karts/controller/end_controller.hpp"
#include "karts/kart_properties_manager.hpp"
#include "network/network_manager.hpp"
#include "network/race_state.hpp"
#include "physics/btKart.hpp"
#include "race/highscore_manager.hpp"
#include "race/history.hpp"
#include "race/race_manager.hpp"
#include "states_screens/state_manager.hpp"
#include "states_screens/race_gui_base.hpp"
#include "states_screens/race_gui.hpp"
#include "states_screens/race_result_gui.hpp"
#include "tracks/track.hpp"
#include "tracks/track_manager.hpp"
#include "utils/constants.hpp"
#include "utils/translation.hpp"
#include "utils/string_utils.hpp"

World* World::m_world = NULL;

/** The main world class is used to handle the track and the karts.
 *  The end of the race is detected in two phases: first the (abstract)
 *  function isRaceOver, which must be implemented by all game modes,
 *  must return true. In which case enterRaceOverState is called. At 
 *  this time a winning (or losing) animation can be played. The WorldStatus
 *  class will in its enterRaceOverState switch to DELAY_FINISH_PHASE,
 *  but the remaining AI kart will keep on racing during that time.
 *  After a time period specified in stk_config.xml WorldStatus will
 *  switch to FINISH_PHASE and call terminateRace. Now the finishing status
 *  of all karts is set (i.e. in a normal race the arrival time for karts
 *  will be estimated), highscore is updated, and the race result gui
 *  is being displayed.
 */
//-----------------------------------------------------------------------------
/** Constructor. Note that in the constructor it is not possible to call any
 *  functions that use World::getWorld(), since this is only defined
 *  after the constructor. Those functions must be called in the init() 
 *  function, which is called immediately after the constructor.
 */
World::World() : WorldStatus(), m_clear_color(255,100,101,140)
{
    m_physics           = NULL;
    m_race_gui          = NULL;
    m_saved_race_gui    = NULL;
    m_use_highscores    = true;
    m_track             = NULL;
    m_clear_back_buffer = false;
    m_schedule_pause    = false;
    m_schedule_unpause  = false;
    
    WorldStatus::setClockMode(CLOCK_CHRONO);
}   // World

// ----------------------------------------------------------------------------
/** This function is called after the World constructor. In init() functions
 *  can be called that use World::getWorld(). The init function is 
 *  called immediately after the constructor.
 */
void World::init()
{
    race_state            = new RaceState();
    m_faster_music_active = false;
    m_fastest_lap         = 9999999.9f;
    m_fastest_kart        = 0;
    m_eliminated_karts    = 0;
    m_eliminated_players  = 0;
    m_num_players         = 0;

    // Create the race gui before anything else is attached to the scene node
    // (which happens when the track is loaded). This allows the race gui to
    // do any rendering on texture.
    m_race_gui = new RaceGUI();

    // Grab the track file
    m_track = track_manager->getTrack(race_manager->getTrackName());
    if(!m_track)
    {
        std::ostringstream msg;
        msg << "Track '" << race_manager->getTrackName() 
            << "' not found.\n";
        throw std::runtime_error(msg.str());
    }

    // Create the physics
    m_physics = new Physics();

    unsigned int num_karts = race_manager->getNumberOfKarts();

    assert(num_karts > 0);

    // Load the track models - this must be done before the karts so that the
    // karts can be positioned properly on (and not in) the tracks.
    m_track->loadTrackModel(this);

    for(unsigned int i=0; i<num_karts; i++)
    {
        const std::string& kart_ident = 
             history->replayHistory() ? history->getKartIdent(i)
                                      : race_manager->getKartIdent(i);
        int local_player_id           = race_manager->getKartLocalPlayerId(i);
        int global_player_id          = race_manager->getKartGlobalPlayerId(i);
        Kart* newkart = createKart(kart_ident, i, local_player_id,  
                                   global_player_id);
        m_karts.push_back(newkart);
        newkart->setWorldKartId(m_karts.size()-1);
        m_track->adjustForFog(newkart->getNode());
    }  // for i

    resetAllKarts();
    // Note: track reset must be called after all karts exist, since check
    // objects need to allocate data structures depending on the number
    // of karts.
    m_track->reset();

    if(!history->replayHistory()) history->initRecording();
    network_manager->worldLoaded();
    
    powerup_manager->updateWeightsForRace(num_karts);
    // erase messages left over
    RaceGUIBase* m = World::getWorld()->getRaceGUI();
    if (m) m->clearAllMessages();
}   // init

//-----------------------------------------------------------------------------
/** Creates a kart, having a certain position, starting location, and local
 *  and global player id (if applicable).
 *  \param kart_ident Identifier of the kart to create.
 *  \param index Index of the kart.
 *  \param local_player_id If the kart is a player kart this is the index of
 *         this player on the local machine.
 *  \param global_player_id If the kart is a player kart this is the index of
 *         this player globally (i.e. including network players).
 */
Kart *World::createKart(const std::string &kart_ident, int index,
                        int local_player_id, int global_player_id)
{
    int position           = index+1;
    btTransform init_pos   = m_track->getStartTransform(index);
    Kart *new_kart         = new Kart(kart_ident, position, init_pos);
    Controller *controller = NULL;
    switch(race_manager->getKartType(index))
    {
    case RaceManager::KT_PLAYER:
        controller = new PlayerController(new_kart, 
                                          StateManager::get()->getActivePlayer(local_player_id),
                                          local_player_id);
        m_num_players ++;
        break;
    case RaceManager::KT_NETWORK_PLAYER:
		break;  // Avoid compiler warning about enum not handled.
        //controller = new NetworkController(kart_ident, position, init_pos,
        //                          global_player_id);
        //m_num_players++;
        //break;
    case RaceManager::KT_AI:
        controller = loadAIController(new_kart);
        break;
    case RaceManager::KT_GHOST:
        break;
    case RaceManager::KT_LEADER:
        break;
    }
    new_kart->setController(controller);
    return new_kart;
}   // createKart

//-----------------------------------------------------------------------------
/** Creates an AI controller for the kart.
 *  \param kart The kart to be controlled by an AI.
 */
Controller* World::loadAIController(Kart *kart)
{
    Controller *controller;
    // const int NUM_ROBOTS = 1;
    // For now: instead of random switching, use each
    // robot in turns: switch(m_random.get(NUM_ROBOTS))
    // static int turn=1;
    // turn=1-turn;

    // For now disable the new AI.
    int turn=0;
    switch(turn)
    {
        case 0:
            controller = new DefaultAIController(kart);
            break;
        case 1:
            controller = new NewAIController(kart);
            break;
        default:
            std::cerr << "Warning: Unknown robot, using default." << std::endl;
            controller = new DefaultAIController(kart);
            break;
    }

    return controller;
}   // loadAIController

//-----------------------------------------------------------------------------
World::~World()
{
    // Delete the in-race-gui:
    if(m_saved_race_gui)
    {
        // If there is a save race gui, this means that the result gui is
        // currently being shown. The race result gui is a screen and so
        // is deleted by the state manager. So we only have to delete
        // the actual race gui:
        delete m_saved_race_gui;
    }
    else
    {
        // No race result gui is shown, so m_race_gui is the in-race
        // gui and this must be deleted.
        delete m_race_gui;
    }
    delete race_state;
    // In case that a race is aborted (e.g. track not found) m_track is 0.
    if(m_track)
        m_track->cleanup();

    for ( unsigned int i = 0 ; i < m_karts.size() ; i++ )
        delete m_karts[i];

    m_karts.clear();
    projectile_manager->cleanup();
    // In case that the track is not found, m_physics is still undefined.
    if(m_physics)
        delete m_physics;

    music_manager->stopMusic();
    m_world = NULL;
}   // ~World

//-----------------------------------------------------------------------------
/** Called when 'go' is being displayed for the first time. Here the brakes
 *  of the karts are released.
 */
void World::onGo()
{
    // Reset the brakes now that the prestart 
    // phase is over (braking prevents the karts 
    // from sliding downhill)
    for(unsigned int i=0; i<m_karts.size(); i++) 
    {
        m_karts[i]->resetBrakes();
    }
}   // onGo

//-----------------------------------------------------------------------------
/** Called at the end of a race. Updates highscores, pauses the game, and
 *  informs the unlock manager about the finished race. This function must
 *  be called after all other stats were updated from the different game
 *  modes.
 */
void World::terminateRace()
{
    m_schedule_pause = false;
    m_schedule_unpause = false;
    
    // Update the estimated finishing time for all karts that haven't
    // finished yet.
    const unsigned int kart_amount = getNumKarts();
    for(unsigned int i = 0; i < kart_amount ; i++)
    {
        if(!m_karts[i]->hasFinishedRace() && !m_karts[i]->isEliminated())
        {
            m_karts[i]->finishedRace(estimateFinishTimeForKart(m_karts[i]));

        }
    }   // i<kart_amount
    updateHighscores();
    unlock_manager->raceFinished();
    
    if (m_race_gui) m_race_gui->clearAllMessages();
    // we can't delete the race gui here, since it is needed in case of 
    // a restart: the constructor of it creates some textures which assume 
    // that no scene nodes exist. In case of a restart there are scene nodes, 
    // so we can't create the race gui again, so we keep it around 
    // and save the pointer.
    assert(m_saved_race_gui==NULL);
    m_saved_race_gui = m_race_gui;
    m_race_gui       = RaceResultGUI::getInstance();
    StateManager::get()->pushScreen(RaceResultGUI::getInstance());
    WorldStatus::terminateRace();
}   // terminateRace

//-----------------------------------------------------------------------------
/** Waits till each kart is resting on the ground
 *
 * Does simulation steps still all karts reach the ground, i.e. are not
 * moving anymore
 */
void World::resetAllKarts()
{
    m_schedule_pause = false;
    m_schedule_unpause = false;
    
    //Project karts onto track from above. This will lower each kart so
    //that at least one of its wheel will be on the surface of the track
    for ( KartList::iterator i=m_karts.begin(); i!=m_karts.end(); i++)
    {
        ///start projection from top of kart
        btVector3 up_offset(0, 0.5f * ((*i)->getKartHeight()), 0);
        (*i)->getVehicle()->getRigidBody()->translate (up_offset);

        bool kart_over_ground = m_physics->projectKartDownwards(*i);

        if (!kart_over_ground)
        {
            fprintf(stderr, "ERROR: no valid starting position for kart %d on track %s.\n",
                    (int)(i-m_karts.begin()), m_track->getIdent().c_str());
            exit(-1);
        }
    }

    bool all_finished=false;
    // kart->isInRest() is not fully correct, since it only takes the
    // velocity in count, which might be close to zero when the kart
    // is just hitting the floor, before being pushed up again by
    // the suspension. So we just do a longer initial simulation,
    // which should be long enough for all karts to be firmly on ground.
    for(int i=0; i<60; i++) m_physics->update(1.f/60.f);

    // Stil wait will all karts are in rest (and handle the case that a kart
    // fell through the ground, which can happen if a kart falls for a long
    // time, therefore having a high speed when hitting the ground.
    while(!all_finished)
    {
        m_physics->update(1.f/60.f);
        all_finished=true;
        for ( KartList::iterator i=m_karts.begin(); i!=m_karts.end(); i++)
        {
            if(!(*i)->isInRest())
            {
                float           hot;
                Vec3            normal;
                const Material *material;
                // We can't use (*i)->getXYZ(), since this is only defined
                // after update() was called. Instead we have to get the
                // real position of the rigid body.
                btTransform     t;
                (*i)->getBody()->getMotionState()->getWorldTransform(t);
                // This test can not be done only once before the loop, since
                // it can happen that the kart falls through the track later!
                m_track->getTerrainInfo(t.getOrigin(), &hot, &normal, &material);
                if(!material)
                {
                    fprintf(stderr, "ERROR: no valid starting position for kart %d on track %s.\n",
                (int)(i-m_karts.begin()), m_track->getIdent().c_str());
                    exit(-1);
                }
                all_finished=false;
                break;
            }
        }
    }   // while

    // Now store the current (i.e. in rest) suspension length for each kart,
    // so that the karts can visualise the suspension.
    for ( KartList::iterator i=m_karts.begin(); i!=m_karts.end(); i++)
        (*i)->setSuspensionLength();
    for(unsigned int i=0; i<m_karts.size(); i++)
        if(m_karts[i]->getCamera())
            m_karts[i]->getCamera()->setInitialTransform();
}   // resetAllKarts


void World::schedulePause(Phase phase)
{
    if (m_schedule_unpause)
    {
        m_schedule_unpause = false;
    }
    else
    {
        m_schedule_pause = true;
        m_scheduled_pause_phase = phase;
    }
}

void World::scheduleUnpause()
{
    if (m_schedule_pause)
    {
        m_schedule_pause = false;
    }
    else
    {
        m_schedule_unpause = true;
    }
}

//-----------------------------------------------------------------------------
/** This is the main interface to update the world. This function calls
 *  update(), and checks then for the end of the race. Note that race over 
 *  handling can not necessarily be done in update(), since not all
 *  data structures might have been updated (e.g.LinearWorld must
 *  call World::update() first, to get updated kart positions. If race
 *  over would be handled in World::update, LinearWorld had no opportunity
 *  to update its data structures before the race is finished).
 *  \param dt Time step size.
 */
void World::updateWorld(float dt)
{
	DBG_LINE;
    if (m_schedule_pause)
    {
        pause(m_scheduled_pause_phase);
        m_schedule_pause = false;
    }
    else if (m_schedule_unpause)
    {
        unpause();
        m_schedule_unpause = false;
    }
    
    // Don't update world if a menu is shown or the race is over.
    if( m_phase == FINISH_PHASE         ||
        m_phase == IN_GAME_MENU_PHASE      )  
        return;

    DBG_LINE;
    update(dt);
    DBG_LINE;
    if( (!isFinishPhase()) && isRaceOver())
    {
    	DBG_LINE;
        enterRaceOverState();
    }
    DBG_LINE;

}   // updateWorld

#define MEASURE_FPS 0

//-----------------------------------------------------------------------------
/** Updates the physics, all karts, the track, and projectile manager.
 *  \param dt Time step size.
 */
void World::update(float dt)
{
	ALOGV("World::update begin  fps:%d",irr_driver->getVideoDriver()->getFPS());
#if MEASURE_FPS
    static float time = 0.0f;
    time += dt;
    if (time > 5.0f)
    {
        time -= 5.0f;
        printf("%i\n",irr_driver->getVideoDriver()->getFPS());
    }
#endif

   // history->update(dt);
   // if(history->replayHistory()) dt=history->getNextDelta();
    WorldStatus::update(dt);
    // Clear race state so that new information can be stored
    race_state->clear();

    ALOGV("m_physics->update(dt) begin");
    if(network_manager->getMode()!=NetworkManager::NW_CLIENT &&
        !history->dontDoPhysics())
    {
        m_physics->update(dt);
    }
    ALOGV("m_physics->update(dt) end");

    const int kart_amount = m_karts.size();
    for (int i = 0 ; i < kart_amount; ++i)
    {
        // Update all karts that are not eliminated
        if(!m_karts[i]->isEliminated()) m_karts[i]->update(dt) ;
    }
    // The order of updates is rather important: if track update would
    // be called before kart update, then the check manager (called from
    // track update) will be using the old kart position to determine
    // e.g. if a kart has crossed a new line. But linear world (from
    // which this is called in case of a race) will be using the new
    // position of the karts to determine the driveline quad a kart
    // is on. So if a kart just arrived at quad 0 (meaning the distance
    // along the track goes from close-to-lap-length to close-to-zero),
    // the order of karts will be incorrect, since this kart will not
    // have started the next lap. While this will only last for one
    // frame (since in the next frame the check manager will detect
    // the new lap), it causes an incorrect display of icons in the
    // icon display for this one frame.
    m_track->update(dt);

    projectile_manager->update(dt);

	ALOGV("World::update end");
}   // update

// ----------------------------------------------------------------------------

Highscores* World::getHighscores() const
{
    if(!m_use_highscores) return NULL;

    const Highscores::HighscoreType type = "HST_" + getIdent();

    Highscores * highscores =
        highscore_manager->getHighscores(type,
                                         getNumKarts(),
                                         race_manager->getDifficulty(),
                                         race_manager->getTrackName(),
                                         race_manager->getNumLaps());

    return highscores;
}   // getHighscores

// ----------------------------------------------------------------------------
/*
 * Usually called at the end of a race. Checks if the current times are worth a new
 * score, if so it notifies the HighscoreManager so the new score is added and saved.
 */
void World::updateHighscores()
{
    if(!m_use_highscores) return;

    // Add times to highscore list. First compute the order of karts,
    // so that the timing of the fastest kart is added first (otherwise
    // someone might get into the highscore list, only to be kicked out
    // again by a faster kart in the same race), which might be confusing
    // if we ever decide to display a message (e.g. during a race)
    unsigned int *index = new unsigned int[m_karts.size()];

    const unsigned int kart_amount = m_karts.size();
    for (unsigned int i=0; i<kart_amount; i++ )
    {
        index[i] = 999; // first reset the contents of the array
    }
    for (unsigned int i=0; i<kart_amount; i++ )
    {
        const int pos = m_karts[i]->getPosition()-1;
        if(pos < 0 || pos >= (int)kart_amount) continue; // wrong position
        index[pos] = i;
    }

    for(unsigned int pos=0; pos<kart_amount; pos++)
    {

        if(index[pos] == 999)
        {
            // no kart claimed to be in this position, most likely means
            // the kart location data is wrong

#ifdef DEBUG
            fprintf(stderr, "Error, incorrect kart positions:\n");
            for (unsigned int i=0; i<m_karts.size(); i++ )
            {
                fprintf(stderr, "i=%d position %d\n",i, m_karts[i]->getPosition());
            }
#endif
            continue;
        }

        // Only record times for player karts and only if they finished the race
        if(!m_karts[index[pos]]->getController()->isPlayerController()) continue;
        if (!m_karts[index[pos]]->hasFinishedRace()) continue;

        assert(index[pos] >= 0);
        assert(index[pos] < m_karts.size());
        Kart *k = (Kart*)m_karts[index[pos]];

        Highscores* highscores = getHighscores();

        PlayerController *controller = (PlayerController*)(k->getController());
        if(highscores->addData(k->getIdent(),
                               controller->getPlayer()->getProfile()->getName(),
                               k->getFinishTime())>0 )
        {
            highscore_manager->saveHighscores();
        }
    } // next position
    delete []index;

}   // updateHighscores

//-----------------------------------------------------------------------------
/** Returns the n-th player kart. Note that this function is O(N), not O(1),
 *  so it shouldn't be called inside of loops.
 *  \param n Index of player kart to return.
 */
Kart *World::getPlayerKart(unsigned int n) const
{
    unsigned int count=-1;

    for(unsigned int i=0; i<m_karts.size(); i++)
        if(m_karts[i]->getController()->isPlayerController())
        {
            count++;
            if(count==n) return m_karts[i];
        }
    return NULL;
}   // getPlayerKart

//-----------------------------------------------------------------------------
/** Returns the nth local player kart, i.e. a player kart that has a camera.
 *  \param n Index of player kart to return.
 */
Kart *World::getLocalPlayerKart(unsigned int n) const
{
    int count=-1;
    const int kart_count = m_karts.size();
    for(int i=0; i<kart_count; i++)
    {
        if(m_karts[i]->getCamera() && m_karts[i]->getController()->isPlayerController())
        {
            count++;
            if(count == (int)n) return m_karts[i];
        }
    }
    return NULL;
}   // getLocalPlayerKart

//-----------------------------------------------------------------------------
/** Remove (eliminate) a kart from the race */
void World::removeKart(int kart_number, bool notifyOfElimination)
{
    Kart *kart = m_karts[kart_number];
    
    // Display a message about the eliminated kart in the race gui
    if (notifyOfElimination)
    {
        for (KartList::iterator i  = m_karts.begin(); i != m_karts.end();  i++ )
        {
            if(!(*i)->getCamera()) continue;
            if(*i==kart)
            {
                m_race_gui->addMessage(_("You have been eliminated!"), *i, 2.0f, 60);
            }
            else
            {
                m_race_gui->addMessage(StringUtils::insertValues(_("'%s' has been eliminated."),
                                                                 kart->getName().c_str()),
                                                                 *i, 2.0f, 60);
            }
        }   // for i in kart
    }
    
    if(kart->getController()->isPlayerController())
    {
        // Change the camera so that it will be attached to the leader
        // and facing backwards.
        Camera* camera=kart->getCamera();
        camera->setMode(Camera::CM_LEADER_MODE);
        m_eliminated_players++;
    }

    // The kart can't be really removed from the m_kart array, since otherwise
    // a race can't be restarted. So it's only marked to be eliminated (and
    // ignored in all loops). Important:world->getCurrentNumKarts() returns
    // the number of karts still racing. This value can not be used for loops
    // over all karts, use race_manager->getNumKarts() instead!
    kart->eliminate();
    m_eliminated_karts++;

}   // removeKart

//-----------------------------------------------------------------------------
void World::getDefaultCollectibles(int& collectible_type, int& amount )
{
    collectible_type = PowerupManager::POWERUP_NOTHING;
    amount = 0;
}
//-----------------------------------------------------------------------------
void World::restartRace()
{
    // If m_saved_race_gui is set, it means that the restart was done
    // when the race result gui was being shown. In this case restore the 
    // race gui (not that the race result gui is cached and so never really
    // destroyed).
    if(m_saved_race_gui)
    {
        m_race_gui       = m_saved_race_gui;
        m_saved_race_gui = NULL;
    }

    m_race_gui->clearAllMessages();

    m_schedule_pause = false;
    m_schedule_unpause = false;
    
    WorldStatus::reset();
    m_faster_music_active = false;
    m_eliminated_karts    = 0;
    m_eliminated_players  = 0;
    
    for ( KartList::iterator i = m_karts.begin(); i != m_karts.end() ; ++i )
    {
        (*i)->reset();
    }

    resetAllKarts();

    // Start music from beginning
    music_manager->stopMusic();
    m_track->reset();

    // Enable SFX again
    sfx_manager->resumeAll();

    projectile_manager->cleanup();
    race_manager->reset();
}   // restartRace

//-----------------------------------------------------------------------------
/** Pauses the music (and then pauses WorldStatus).
 */
void World::pause(Phase phase)
{
    music_manager->pauseMusic();
    sfx_manager->pauseAll();
    
    WorldStatus::pause(phase);
}   // pause

//-----------------------------------------------------------------------------
void World::unpause()
{
    music_manager->resumeMusic() ;
    sfx_manager->resumeAll();
    
    WorldStatus::unpause();
    
    for(unsigned int i=0; i<m_karts.size(); i++)
    {
        // Note that we can not test for isPlayerController here, since
        // an EndController will also return 'isPlayerController' if the
        // kart belonged to a player.
        PlayerController *pc = dynamic_cast<PlayerController*>(m_karts[i]->getController());
        if(pc)
            pc->resetInputState();
    }
}   // pause

/* EOF */
