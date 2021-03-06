// $Id: main.cpp 6341 2010-10-26 09:29:44Z hikerstk $
//
//  SuperTuxKart - a fun racing game with go-kart
//  Copyright (C) 2004-2005 Steve Baker <sjbaker1@airmail.net>
//  Copyright (C) 2006 Joerg Henrichs, Steve Baker
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


#ifdef WIN32
#  ifdef __CYGWIN__
#    include <unistd.h>
#  endif
#  define _WINSOCKAPI_
#  include <windows.h>
#  ifdef _MSC_VER
#    include <io.h>
#    include <direct.h>
#  endif
#else
#  include <unistd.h>
#endif
#include <stdexcept>
#include <cstdio>
#include <string>
#include <cstring>
#include <sstream>
#include <algorithm>

#include "main_loop.hpp"
#include "audio/music_manager.hpp"
#include "audio/sfx_manager.hpp"
#include "challenges/unlock_manager.hpp"
#include "config/stk_config.hpp"
#include "config/user_config.hpp"
#include "config/player.hpp"
#include "graphics/irr_driver.hpp"
#include "graphics/material_manager.hpp"
#include "guiengine/engine.hpp"
#include "io/file_manager.hpp"
#include "input/input_manager.hpp"
#include "input/device_manager.hpp"
#include "items/attachment_manager.hpp"
#include "items/item_manager.hpp"
#include "items/projectile_manager.hpp"
#include "karts/kart_properties_manager.hpp"
#include "karts/kart.hpp"
#include "modes/profile_world.hpp"
#include "network/network_manager.hpp"
#include "race/grand_prix_manager.hpp"
#include "race/highscore_manager.hpp"
#include "race/history.hpp"
#include "race/race_manager.hpp"
#include "states_screens/main_menu_screen.hpp"
#include "states_screens/state_manager.hpp"
#include "tracks/track.hpp"
#include "tracks/track_manager.hpp"
#include "utils/translation.hpp"

#ifdef ADDONS_MANAGER
#include "addons/network.hpp"
#include "addons/addons.hpp"
#endif

void cmdLineHelp (char* invocation)
{
    fprintf ( stdout,
    "Usage: %s [OPTIONS]\n\n"
    "Run SuperTuxKart, a racing game with go-kart that features"
    " the Tux and friends.\n\n"
    "Options:\n"
    "  -N,  --no-start-screen  Immediatgely start race without showing a menu.\n"
    "  -t,  --track NAME       Start at track NAME (see --list-tracks).\n"
    "  --gp name               Start the specified Grand Prix."
    "       --stk-config FILE  use ./data/FILE instead of ./data/stk_config.xml\n"
    "  -l,  --list-tracks      Show available tracks.\n"
    "  -k,  --numkarts NUM     Number of karts on the racetrack.\n"
    "       --kart NAME        Use kart number NAME (see --list-karts).\n"
    "       --list-karts       Show available karts.\n"
    "       --laps N           Define number of laps to N.\n"
    "       --mode N           N=1 novice, N=2 driver, N=3 racer.\n"
    //FIXME"     --players n             Define number of players to between 1 and 4.\n"
    "       --item STYLE       Use STYLE as your item style.\n"
    "  -f,  --fullscreen       Select fullscreen display.\n"
    "  -w,  --windowed         Windowed display (default).\n"
    "  -s,  --screensize WxH   Set the screen size (e.g. 320x200).\n"
    "  -v,  --version          Show version of SuperTuxKart.\n"
    "       --trackdir DIR     A directory from which additional tracks are loaded.\n"
    "       --renderer NUM     Choose the renderer. Valid renderers are:"
    "                          (Default: 0, OpenGL: 1, Direct3D9: 2, \n"
    "                           Direct3D8: 3, Software: 4, \n"
    "                           Burning's Software: 5, Null device: 6).\n"
    // should not be used by unaware users:u
    // "  --profile            Enable automatic driven profile mode for 20 seconds.\n"
    // "  --profile=n          Enable automatic driven profile mode for n seconds.\n"
    // "                       if n<0 --> (-n) = number of laps to drive.
    // "  --history            Replay history file 'history.dat'.\n"
    // "  --history=n          Replay history file 'history.dat' using mode:\n"
    // "                       n=1: use recorded positions\n"
    // "                       n=2: use recorded key strokes\n"
    "  --server[=port]         This is the server (running on the specified port).\n"
    "  --client=ip             This is a client, connect to the specified ip address.\n"
    "  --port=n                Port number to use.\n"
    "  --numclients=n          Number of clients to wait for (server only).\n"
    "  --log=terminal          Write messages to screen.\n"
    "  --log=file              Write messages/warning to log files stdout.log/stderr.log.\n"
    "  -h,  --help             Show this help.\n"
    "\n"
    "You can visit SuperTuxKart's homepage at "
    "http://supertuxkart.sourceforge.net\n\n", invocation
    );
}   // cmdLineHelp

//=============================================================================
/** For base options that don't need much to be inited (and, in some cases,
 *  that need to be read before initing stuff) - it only assumes that
 *  user config is loaded (necessary to check for blacklisted screen
 *  resolutions), but nothing else (esp. not kart_properties_manager and
 *  track_manager, since their search path might be extended by command
 *  line options).
 */
int handleCmdLinePreliminary(int argc, char **argv)
{
    for(int i=1; i<argc; i++)
    {
        if(argv[i][0] != '-') continue;
        if (!strcmp(argv[i], "--help" ) ||
            !strcmp(argv[i], "-help"  ) ||
            !strcmp(argv[i], "--help" ) ||
            !strcmp(argv[i], "-help"  ) ||
            !strcmp(argv[i], "-h"     )     )
        {
            cmdLineHelp(argv[0]);
            exit(0);
        }
        else if( (!strcmp(argv[i], "--stk-config")) && i+1<argc )
        {
            stk_config->load(file_manager->getConfigFile(argv[i+1]));
            fprintf ( stdout, "STK config will be read from %s.\n", argv[i+1] ) ;
            i++;
        }
        else if( !strcmp(argv[i], "--trackdir") && i+1<argc )
        {
            TrackManager::addTrackSearchDir(argv[i+1]);
            i++;
        }
        else if( !strcmp(argv[i], "--kartdir") && i+1<argc )
        {
            KartPropertiesManager::addKartSearchDir(argv[i+1]);
            i++;
        }
#if !defined(WIN32) && !defined(__CYGWIN)
        else if ( !strcmp(argv[i], "--fullscreen") || !strcmp(argv[i], "-f"))
        {
            // Check that current res is not blacklisted
            std::ostringstream o;
            o << UserConfigParams::m_width << "x" << UserConfigParams::m_height;
            std::string res = o.str();
            if (std::find(UserConfigParams::m_blacklist_res.begin(),
                          UserConfigParams::m_blacklist_res.end(),res) == UserConfigParams::m_blacklist_res.end())
                UserConfigParams::m_fullscreen = true;
            else
                fprintf ( stdout, "Resolution %s has been blacklisted, so it is not available!\n", res.c_str());
        }
        else if ( !strcmp(argv[i], "--windowed") || !strcmp(argv[i], "-w"))
        {
            UserConfigParams::m_fullscreen = false;
        }
#endif
        else if( !strcmp(argv[i], "--renderer") && (i+1 < argc)  )
        {
            std::cout << "You chose renderer " << atoi(argv[i+1]) << std::endl;
            UserConfigParams::m_renderer = atoi(argv[i+1]);
            i++;
        }
        else if ( (!strcmp(argv[i], "--screensize") || !strcmp(argv[i], "-s") )
             && i+1<argc)
        {
            //Check if fullscreen and new res is blacklisted
            int width, height;
            if (sscanf(argv[i+1], "%dx%d", &width, &height) == 2)
            {
                std::ostringstream o;
                o << width << "x" << height;
                std::string res = o.str();
                if (!UserConfigParams::m_fullscreen || std::find(UserConfigParams::m_blacklist_res.begin(),
                                                            UserConfigParams::m_blacklist_res.end(),res) == UserConfigParams::m_blacklist_res.end())
                {
                    UserConfigParams::m_prev_width = UserConfigParams::m_width = width;
                    UserConfigParams::m_prev_height = UserConfigParams::m_height = height;
                    fprintf ( stdout, "You choose to be in %dx%d.\n", (int)UserConfigParams::m_width,
                             (int)UserConfigParams::m_height );
                }
                else
                    fprintf ( stdout, "Resolution %s has been blacklisted, so it is not available!\n", res.c_str());
                i++;
            }
            else
            {
                fprintf(stderr, "Error: --screensize argument must be given as WIDTHxHEIGHT\n");
                exit(EXIT_FAILURE);
            }
        }
        else if( !strcmp(argv[i], "--version") ||  !strcmp(argv[i], "-v") )
        {
            printf("==============================\n");
#ifdef VERSION
            fprintf ( stdout, "SuperTuxKart, %s.\n", VERSION ) ;
#endif
#ifdef SVNVERSION
            fprintf ( stdout, "SuperTuxKart, SVN revision number '%s'.\n", SVNVERSION ) ;
#endif
#if !defined(VERSION) && !defined(SVNVERSION)
            fprintf ( stdout, "SuperTuxKart, unknown version\n" ) ;
#endif
            printf("==============================\n");
            exit(0);
        }
    }
    return 0;
}

// ============================================================================
/** Handles command line options.
 *  \param argc Number of command line options
 *  \param argv Command line options.
 */
int handleCmdLine(int argc, char **argv)
{
    int n;
    char s[80];

    for(int i=1; i<argc; i++)
    {

        if(!strcmp(argv[i], "--gamepad-debug"))
        {
            UserConfigParams::m_gamepad_debug=true;
        }
        else if(sscanf(argv[i], "--track-debug=%d",&n)==1)
        {
            UserConfigParams::m_track_debug=n;
        }
        else if(!strcmp(argv[i], "--track-debug"))
        {
            UserConfigParams::m_track_debug=1;
        }
        else if(!strcmp(argv[i], "--material-debug"))
        {
            UserConfigParams::m_material_debug = true;
        }
        else if(!strcmp(argv[i], "--camera-debug"))
        {
            UserConfigParams::m_camera_debug=1;
        }
        else if(!strcmp(argv[i], "--kartsize-debug"))
        {
            for(unsigned int i=0; i<kart_properties_manager->getNumberOfKarts();
                i++)
            {
                const KartProperties *km = kart_properties_manager->getKartById(i);
                 printf("%s:\t%swidth: %f length: %f height: %f mesh-buffer count %d\n",
                       km->getIdent().c_str(),
                       (km->getIdent().size()<7) ? "\t" : "",
                       km->getMasterKartModel().getWidth(),
                       km->getMasterKartModel().getLength(),
                       km->getMasterKartModel().getHeight(),
                       km->getMasterKartModel().getModel()->getMeshBufferCount());
            }
        }
        else if(!strcmp(argv[i], "--check-debug"))
        {
            UserConfigParams::m_check_debug=true;
        }
        else if(sscanf(argv[i], "--server=%d",&n)==1)
        {
            network_manager->setMode(NetworkManager::NW_SERVER);
            UserConfigParams::m_server_port = n;
        }
        else if( !strcmp(argv[i], "--server") )
        {
            network_manager->setMode(NetworkManager::NW_SERVER);
        }
        else if( sscanf(argv[i], "--port=%d", &n) )
        {
            UserConfigParams::m_server_port=n;
        }
        else if( sscanf(argv[i], "--client=%s", s) )
        {
            network_manager->setMode(NetworkManager::NW_CLIENT);
            UserConfigParams::m_server_address=s;
        }
        else if( (!strcmp(argv[i], "--kart") && i+1<argc ))
        {
            std::string filename=file_manager->getKartFile(std::string(argv[i+1])+".tkkf");
            if(filename!="")
            {
                UserConfigParams::m_default_kart = argv[i+1];

                // if a player was added with -N, change its kart. Otherwise, nothing to do,
                // kart choice will be picked up upon player creation.
                if (StateManager::get()->activePlayerCount() > 0)
                {
                    race_manager->setNumLocalPlayers(1);
                    race_manager->setLocalKartInfo(0, argv[i+1]);
                }
                fprintf ( stdout, "You chose to use kart '%s'.\n", argv[i+1] ) ;
                i++;
            }
            else
            {
                fprintf(stdout, "Kart '%s' not found, ignored.\n",
                        argv[i+1]);
            }
        }
        else if( (!strcmp(argv[i], "--mode") && i+1<argc ))
        {
            switch (atoi(argv[i+1]))
            {
            case 1:
                race_manager->setDifficulty(RaceManager::RD_EASY);
                break;
            case 2:
                race_manager->setDifficulty(RaceManager::RD_MEDIUM);
                break;
            case 3:
                race_manager->setDifficulty(RaceManager::RD_HARD);
                break;
            }
            i++;
        }
        else if( (!strcmp(argv[i], "--track") || !strcmp(argv[i], "-t"))
                 && i+1<argc                                              )
        {
            if (!unlock_manager->isLocked(argv[i+1]))
            {
                race_manager->setTrack(argv[i+1]);
                fprintf ( stdout, "You choose to start in track: %s.\n", argv[i+1] ) ;
            }
            else
            {
                fprintf(stdout, "Track %s has not been unlocked yet. \n", argv[i+1]);
                fprintf(stdout, "Use --list-tracks to list available tracks.\n\n");
                return 0;
            }
            i++;
        }
        else if( (!strcmp(argv[i], "--gp")) && i+1<argc)
        {
            race_manager->setMajorMode(RaceManager::MAJOR_MODE_GRAND_PRIX);
            const GrandPrixData *gp = grand_prix_manager->getGrandPrix(argv[i+1]);
            race_manager->setGrandPrix(*gp);
            i++;
        }
        else if( (!strcmp(argv[i], "--numkarts") || !strcmp(argv[i], "-k")) &&
                 i+1<argc )
        {
            UserConfigParams::m_num_karts = atoi(argv[i+1]);
            if(UserConfigParams::m_num_karts > stk_config->m_max_karts)
            {
                fprintf(stdout, "Number of karts reset to maximum number %d\n",
                                  stk_config->m_max_karts);
                UserConfigParams::m_num_karts = stk_config->m_max_karts;
            }
            race_manager->setNumKarts( UserConfigParams::m_num_karts );
            fprintf(stdout, "%d karts will be used.\n",  (int)UserConfigParams::m_num_karts);
            i++;
        }
        else if( !strcmp(argv[i], "--list-tracks") || !strcmp(argv[i], "-l") )
        {

            fprintf ( stdout, "  Available tracks:\n" );
            for (size_t i = 0; i != track_manager->getNumberOfTracks(); i++)
            {
                const Track *track = track_manager->getTrack(i);
                if (!unlock_manager->isLocked(track->getIdent()))
                {
                    fprintf ( stdout, "\t%10s: %ls\n",
                              track->getIdent().c_str(),
                              track->getName().c_str());
                }
            }

            fprintf ( stdout, "Use --track N to choose track.\n\n");
        }
        else if( !strcmp(argv[i], "--list-karts") )
        {
            fprintf ( stdout, "  Available karts:\n" );
            for (unsigned int i = 0; NULL != kart_properties_manager->getKartById(i); i++)
            {
                const KartProperties* KP= kart_properties_manager->getKartById(i);
                fprintf (stdout, "\t%10s: %ls\n", KP->getIdent().c_str(), KP->getName().c_str());
            }
            fprintf ( stdout, "\n" );
        }
        else if (    !strcmp(argv[i], "--no-start-screen")
                     || !strcmp(argv[i], "-N")                )
        {
            UserConfigParams::m_no_start_screen = true;
            //FIXME} else if ( !strcmp(argv[i], "--reverse") ) {
            //FIXME:fprintf ( stdout, "Enabling reverse mode.\n" ) ;
            //FIXME:raceSetup.reverse = 1;
        }
        else if ( !strcmp(argv[i], "--mirror") )
        {
#ifdef SSG_BACKFACE_COLLISIONS_SUPPORTED
            fprintf ( stdout, "Enabling mirror mode.\n" ) ;
            //raceSetup.mirror = 1;
#else
            //raceSetup.mirror = 0 ;
#endif

        }
        else if ( !strcmp(argv[i], "--laps") && i+1<argc )
        {
            fprintf ( stdout, "You choose to have %d laps.\n", atoi(argv[i+1]) ) ;
            race_manager->setNumLaps(atoi(argv[i+1]));
            i++;
        }
        /* FIXME:
        else if ( !strcmp(argv[i], "--players") && i+1<argc ) {
          raceSetup.numPlayers = atoi(argv[i+1]);

          if ( raceSetup.numPlayers < 0 || raceSetup.numPlayers > 4) {
        fprintf ( stderr,
        "You choose an invalid number of players: %d.\n",
        raceSetup.numPlayers );
        cmdLineHelp(argv[0]);
        return 0;
          }
          fprintf ( stdout, "You choose to have %d players.\n", atoi(argv[i+1]) ) ;
          i++;
        }
        */
        else if( !strcmp(argv[i], "--log=terminal"))
        {
            UserConfigParams::m_log_errors=false;
        }
        else if( !strcmp(argv[i], "--log=file"))
        {
            UserConfigParams::m_log_errors=true;
        } else if( sscanf(argv[i], "--profile=%d",  &n)==1)
        {
            if(n<0)
            {
                printf("Profiling %d laps\n",-n);
                ProfileWorld::setProfileModeLaps(-n);
                race_manager->setNumLaps(-n);
            }
            else
            {
                printf("Profiling: %d seconds.\n", n);
                ProfileWorld::setProfileModeTime((float)n);
                race_manager->setNumLaps(999999); // profile end depends on time
            }
        }
        else if( !strcmp(argv[i], "--profile") )
        {
            printf("Profiling: %d seconds.\n", n);
            ProfileWorld::setProfileModeTime(20);
            race_manager->setNumLaps(999999); // profile end depends on time
        }
        else if ( !strcmp(argv[i], "-v") && i+1<argc )
        {
            UserConfigParams::m_verbosity = atoi(argv[i+1]);
            i++;
        }
        else if( sscanf(argv[i], "--history=%d",  &n)==1)
        {
            history->doReplayHistory( (History::HistoryReplayMode)n);
            // Force the no-start screen flag, since this initialises
            // the player structures correctly.
            UserConfigParams::m_no_start_screen = true;

        }
        else if( !strcmp(argv[i], "--history") )
        {
            history->doReplayHistory(History::HISTORY_POSITION);
            // Force the no-start screen flag, since this initialises
            // the player structures correctly.
            UserConfigParams::m_no_start_screen = true;

        }
        else if( !strcmp(argv[i], "--item") && i+1<argc )
        {
            item_manager->setUserFilename(argv[i+1]);
            i++;
        }
        // these commands are already processed in handleCmdLinePreliminary, but repeat this
        // just so that we don't get error messages about unknown commands
        else if( !strcmp(argv[i], "--stk-config")&& i+1<argc ) { i++; }
        else if( !strcmp(argv[i], "--trackdir")  && i+1<argc ) { i++; }
        else if( !strcmp(argv[i], "--kartdir")   && i+1<argc ) { i++; }
        else if( !strcmp(argv[i], "--renderer")  && i+1<argc ) { i++; }
        else if( !strcmp(argv[i], "--screensize") || !strcmp(argv[i], "-s")) {i++;}
        else if( !strcmp(argv[i], "--fullscreen") || !strcmp(argv[i], "-f")) {}
        else if( !strcmp(argv[i], "--windowed")   || !strcmp(argv[i], "-w")) {}
        else if( !strcmp(argv[i], "--version")    || !strcmp(argv[i], "-v")) {}
#ifdef __APPLE__
        // on OS X, sometimes the Finder will pass a -psn* something parameter to the application
        else if( strncmp(argv[i], "-psn", 3) == 0) {}
#endif
        else
        {
            fprintf ( stderr, "Invalid parameter: %s.\n\n", argv[i] );
            cmdLineHelp(argv[0]);
            return 0;
        }
    }   // for i <argc
    if(ProfileWorld::isProfileMode())
    {
        UserConfigParams::m_sfx = false;  // Disable sound effects
        UserConfigParams::m_music = false;// and music when profiling
    }

    return 1;
}   // handleCmdLine

//=============================================================================
/** Initialises the minimum number of managers to get access to user_config.
 */
void initUserConfig(char *argv[])
{
	DBG_LINE;
    file_manager            = new FileManager(argv);
    translations            = new Translations();   // needs file_manager
    user_config             = new UserConfig();     // needs file_manager
    stk_config              = new STKConfig();      // in case of --stk-config

    DBG_LINE;
    // command line parameters
}   // initUserConfig

extern IrrlichtDevice* device ;
//=============================================================================
void initRest()
{
	ALOGV("main.cpp initRest begin");

	DBG_LINE;

    stk_config->load(file_manager->getConfigFile("stk_config.xml"));
    DBG_LINE;
    irr_driver              = new IrrDriver();
    DBG_LINE;
    // Init GUI
     device = irr_driver->getDevice();
    video::IVideoDriver* driver = device->getVideoDriver();
    GUIEngine::init(device, driver, StateManager::get());

    DBG_LINE;

    music_manager           = new MusicManager();DBG_LINE;

    sfx_manager             = new SFXManager();DBG_LINE;
    // The order here can be important, e.g. KartPropertiesManager needs
    // defaultKartProperties, which are defined in stk_config.
    history                 = new History              ();DBG_LINE;
    material_manager        = new MaterialManager      ();DBG_LINE;
    track_manager           = new TrackManager         ();DBG_LINE;
    kart_properties_manager = new KartPropertiesManager();DBG_LINE;
    projectile_manager      = new ProjectileManager    ();DBG_LINE;
    powerup_manager         = new PowerupManager       ();DBG_LINE;
    item_manager            = new ItemManager          ();DBG_LINE;
    attachment_manager      = new AttachmentManager    ();DBG_LINE;
    highscore_manager       = new HighscoreManager     ();DBG_LINE;
    network_manager         = new NetworkManager       ();DBG_LINE;
#ifdef ADDONS_MANAGER
    network_http            = new NetworkHttp          ();
    addons_manager          = new Addons               ();

    KartPropertiesManager::addKartSearchDir(file_manager->getAddonsDir() + "/data/karts/");
	track_manager->addTrackSearchDir(file_manager->getAddonsDir() + "/data/tracks/");
    std::cout << "addons dir:" << file_manager->getAddonsDir() + "/data/karts/" << std::endl;
#endif
    track_manager->loadTrackList();
    music_manager->addMusicToTracks();
    DBG_LINE;
    GUIEngine::addLoadingIcon( irr_driver->getTexture(file_manager->getTextureFile("notes.png")) );
    DBG_LINE;
    grand_prix_manager      = new GrandPrixManager     ();
    // Consistency check for challenges, and enable all challenges
    // that have all prerequisites fulfilled
    grand_prix_manager->checkConsistency();
    GUIEngine::addLoadingIcon( irr_driver->getTexture(file_manager->getTextureFile("cup_gold.png")) );
    DBG_LINE;
    race_manager            = new RaceManager          ();
    // default settings for Quickstart
    race_manager->setNumPlayers(1);
    race_manager->setNumLaps   (3);
    race_manager->setMajorMode (RaceManager::MAJOR_MODE_SINGLE);
    race_manager->setMinorMode (RaceManager::MINOR_MODE_NORMAL_RACE);
    race_manager->setDifficulty((RaceManager::Difficulty)(int)UserConfigParams::m_difficulty);
    // race_manager->setDifficulty(RaceManager::RD_HARD);

    //menu_manager= new MenuManager();
    DBG_LINE;
	ALOGV("main.cpp initRest end");
}   // initRest

//=============================================================================
void cleanTuxKart()
{
    //delete in reverse order of what they were created in.
    //see InitTuxkart()
    if(race_manager)            delete race_manager;
    if(network_manager)         delete network_manager;
    if(grand_prix_manager)      delete grand_prix_manager;
    if(highscore_manager)       delete highscore_manager;
    if(attachment_manager)      delete attachment_manager;
    if(item_manager)            delete item_manager;
    if(powerup_manager)         delete powerup_manager;
    if(projectile_manager)      delete projectile_manager;
    if(kart_properties_manager) delete kart_properties_manager;
    if(stk_config)              delete stk_config;
    if(track_manager)           delete track_manager;
    if(material_manager)        delete material_manager;
    if(history)                 delete history;
    if(sfx_manager)             delete sfx_manager;
    if(music_manager)           delete music_manager;
    if(user_config)             delete user_config;
    if(unlock_manager)          delete unlock_manager;
    if(translations)            delete translations;
    if(file_manager)            delete file_manager;
    if(irr_driver)              delete irr_driver;
}   // cleanTuxKart

//=============================================================================

//int main(int argc, char *argv[] )
int stk_main(int argc, char *argv[] )
{
	DBG_LINE;
   // try {
        // Init the minimum managers so that user config exists, then
        // handle all command line options that do not need (or must
        // not have) other managers initialised:
        initUserConfig(argv); // argv passed so config file can be
                              // found more reliably

        DBG_LINE;
        handleCmdLinePreliminary(argc, argv);
        DBG_LINE;
        initRest();

        DBG_LINE;

        if (UserConfigParams::m_log_errors) //Enable logging of stdout and stderr to logfile
        {
            std::string logoutfile = file_manager->getLogFile("stdout.log");
            std::string logerrfile = file_manager->getLogFile("stderr.log");
            std::cout << "Error messages and other text output will be logged to " ;
            std::cout << logoutfile << " and "<<logerrfile;
            if(freopen (logoutfile.c_str(),"w",stdout)!=stdout)
            {
                fprintf(stderr, "Can not open log file '%s'. Writing to stdout instead.\n",
                        logoutfile.c_str());
            }
            if(freopen (logerrfile.c_str(),"w",stderr)!=stderr)
            {
                fprintf(stderr, "Can not open log file '%s'. Writing to stderr instead.\n",
                        logerrfile.c_str());
            }
        }
        DBG_LINE;
        input_manager = new InputManager ();

        // Get into menu mode initially.
        input_manager->setMode(InputManager::MENU);
        DBG_LINE;
        main_loop = new MainLoop();
        material_manager        -> loadMaterial    ();

		//����ͼƬ �������Ļ,����
        GUIEngine::addLoadingIcon( irr_driver->getTexture(file_manager->getGUIDir() + "/options_video.png") );
        kart_properties_manager -> loadAllKarts    ();
        unlock_manager          = new UnlockManager();
		//����ͼƬ �������Ļ,����
        GUIEngine::addLoadingIcon( irr_driver->getTexture(file_manager->getTextureFile("gui_lock.png")) );
        projectile_manager      -> loadData        ();
        DBG_LINE;
        // Both item_manager and powerup_manager load models and therefore
        // textures from the model directory. To avoid reading the
        // materials.xml twice, we do this here once for both:
        file_manager->pushTextureSearchPath(file_manager->getModelFile(""));
        const std::string materials_file = file_manager->getModelFile("materials.xml");
        if(materials_file!="")
            material_manager->pushTempMaterial(materials_file);
        {
            powerup_manager         -> loadAllPowerups ();
            item_manager            -> loadDefaultItems();
        }
        DBG_LINE;
        if(materials_file!="")
            material_manager->popTempMaterial();
        DBG_LINE;
		//����ͼƬ �������Ļ,����
        GUIEngine::addLoadingIcon( irr_driver->getTexture(file_manager->getGUIDir() + "/gift.png") );
        DBG_LINE;
        file_manager->popTextureSearchPath();
        DBG_LINE;
        attachment_manager      -> loadModels      ();
        DBG_LINE;
		//����ͼƬ �������Ļ,����
        GUIEngine::addLoadingIcon( irr_driver->getTexture(file_manager->getGUIDir() + "/banana.png") );
        DBG_LINE;
        //handleCmdLine() needs InitTuxkart() so it can't be called first
//        if(!handleCmdLine(argc, argv)){
//        	 DBG_LINE;
//        	 exit(0);
//        }
        DBG_LINE;
        ALOGV("UserConfigParams::m_no_start_screen");
        if(!UserConfigParams::m_no_start_screen)
        {
            StateManager::get()->pushScreen(MainMenuScreen::getInstance());
        }
        else
        {
        	 DBG_LINE;
            InputDevice *device;
            ALOGV("input_manager->getDeviceList()->getKeyboard(0");
            // Use keyboard 0 by default in --no-start-screen
            device = input_manager->getDeviceList()->getKeyboard(0);

            // Create player and associate player with keyboard
			//������Ҳ�������ҵ�����
            StateManager::get()->createActivePlayer( UserConfigParams::m_all_players.get(0), device );

            ALOGV("������Ҳ�������ҵ�����");
            // Set up race manager appropriately
            race_manager->setNumLocalPlayers(1);
            race_manager->setLocalKartInfo(0, UserConfigParams::m_default_kart);

            ALOGV("getDeviceList()->setAssignMode(ASSIGN");
            // ASSIGN should make sure that only input from assigned devices
            // is read.
            input_manager->getDeviceList()->setAssignMode(ASSIGN);

            ALOGV("StateManager::get()->enterGameState();");
            // Go straight to the race
            StateManager::get()->enterGameState();
        }

        // Replay a race
        // =============
//        if(history->replayHistory())
//        {
//            // This will setup the race manager etc.
//            history->Load();
//            network_manager->setupPlayerKartInfo();
//            race_manager->startNew();
//            main_loop->run();
//            // well, actually run() will never return, since
//            // it exits after replaying history (see history::GetNextDT()).
//            // So the next line is just to make this obvious here!
//            exit(-3);
//        }

        ALOGV("network_manager->initialiseConnections()");

        DBG_LINE;
        // Initialise connection in case that a command line option was set
        // configuring a client or server. Otherwise this function does nothing
        // here (and will be called again from the network gui).
        if(!network_manager->initialiseConnections())
        {
        	ALOGV( "Problems initialising network connections Running in non-network mode ");
        }
        DBG_LINE;
        // On the server start with the network information page for now
        if(network_manager->getMode()==NetworkManager::NW_SERVER)
        {
            // TODO - network menu
            //menu_manager->pushMenu(MENUID_NETWORK_GUI);
        }
        DBG_LINE;
        ALOGV("ProfileWorld::isProfileMode()");
        // Not replaying
        // =============
        if(!ProfileWorld::isProfileMode())
        {
        	 ALOGV("ProfileWorld::isProfileMode()22222222222222222");
            if(UserConfigParams::m_no_start_screen)
            {
                // Quickstart (-N)
                // ===============
                // all defaults are set in InitTuxkart()
                network_manager->setupPlayerKartInfo();
                race_manager->startNew();
            }
        }
        else  // profile
        {
        	 ALOGV("ProfileWorld::isProfileMode()3333333333333333333");
            // Profiling
            // =========
            race_manager->setMajorMode (RaceManager::MAJOR_MODE_SINGLE);
            race_manager->setMinorMode (RaceManager::MINOR_MODE_NORMAL_RACE);
            race_manager->setDifficulty(RaceManager::RD_HARD);
            network_manager->setupPlayerKartInfo();
            race_manager->startNew();
        }
        DBG_LINE;
       // main_loop->run();

 //   }  // try
 //   catch (std::exception &e)
    {
  //      fprintf(stderr,"Exception caught : %s\n",e.what());
 //       fprintf(stderr,"Aborting SuperTuxKart\n");
    }

    ALOGV("main load ok ");


    // Program closing...

//    if(user_config)
//    {
//        // In case that abort is triggered before user_config exists
//        if (UserConfigParams::m_crashed) UserConfigParams::m_crashed = false;
//        user_config->saveConfig();
//    }
//    if(input_manager) delete input_manager;  // if early crash avoid delete NULL
//
//    if (user_config && UserConfigParams::m_log_errors) //close logfiles
//    {
//        fclose(stderr);
//        fclose(stdout);
//    }
//
//    cleanTuxKart();

    return 0 ;
}

