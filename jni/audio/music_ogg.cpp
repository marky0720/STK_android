//  $Id$
//
//  SuperTuxKart - a fun racing game with go-kart
//  Copyright (C) 2007 Damien Morel <divdams@free.fr>
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
#define HAVE_OGGVORBIS 1

#if HAVE_OGGVORBIS

#include "audio/music_ogg.hpp"

#include <stdexcept>
 

#include "audio/music_manager.hpp"
#include "config/user_config.hpp"
#include "utils/constants.hpp"
 

MusicOggStream::MusicOggStream()
{
    //m_oggStream= NULL;
  
    m_pausedMusic     = true;
    m_playing         = false;

//	//创建个声音引擎
//	engine = createIrrKlangDevice();
//	if (!engine)
//	{
//		printf("Could not startup engine\n");
//		return ;
//	}

}   // MusicOggStream

//-----------------------------------------------------------------------------
MusicOggStream::~MusicOggStream()
{
    if(stopMusic() == false)
        fprintf(stderr, "WARNING: problems while stopping music.\n");
}   // ~MusicOggStream

//-----------------------------------------------------------------------------
bool MusicOggStream::load(const std::string& filename)
{
    if (isPlaying()) stopMusic();
    
    m_error = true;
    m_fileName = filename;
    if(m_fileName=="") return false;  

//   FILE* m_oggFile = fopen(m_fileName.c_str(), "rb");
//
//    if(!m_oggFile)
//    {
//        printf("Loading Music: %s failed (fopen returned NULL)\n", m_fileName.c_str());
//        return false;
//    }
//
//
//   fclose(m_oggFile);
        
   // preload both sound effects 加载音效
 // ballSound = engine->getSoundSource(m_fileName.c_str());
       
     
 

    m_error=false;
    return true;
}   // load

//-----------------------------------------------------------------------------
bool MusicOggStream::empty()
{
    
    return true;
}   // empty

//-----------------------------------------------------------------------------
bool MusicOggStream::release()
{
    if (m_fileName == "")
    {
        // nothing is loaded
        return true;
    }

    pauseMusic();
    m_fileName= "";
    
    empty();
    
//	 if (ballSound)
//	{
//			     ballSound->drop(); // don't forget to release the pointer once it is no longer needed by you
//			     ballSound = 0;
//	}
//
    m_playing = false;
    
    return true;
}   // release

//-----------------------------------------------------------------------------
bool MusicOggStream::playMusic()
{
    if(isPlaying())
        return true;


	//engine->play2D(ballSound);
 
    m_pausedMusic = false;
    m_playing = true;

    return true;
}   // playMusic

//-----------------------------------------------------------------------------
bool MusicOggStream::isPlaying()
{
    return m_playing;
    
    /*
    if (m_soundSource == -1) return false;
    
    ALenum state;
    alGetSourcei(m_soundSource, AL_SOURCE_STATE, &state);

    return (state == AL_PLAYING);
     */
}   // isPlaying

//-----------------------------------------------------------------------------
bool MusicOggStream::stopMusic()
{
    m_playing = false;

	//engine->stopAllSounds();

    return (release());
}   // stopMusic

//-----------------------------------------------------------------------------
bool MusicOggStream::pauseMusic()
{
    m_playing = false;
    if (m_fileName == "")
    {
        // nothing is loaded
        return true;
    }
	
 
    m_pausedMusic= true;
    return true;
}   // pauseMusic

//-----------------------------------------------------------------------------
bool MusicOggStream::resumeMusic()
{
    m_playing = true;
    
    if (m_fileName == "")
    {
        // nothing is loaded
        return true;
    }

 
    m_pausedMusic= false;
    return true;
}   // resumeMusic

//-----------------------------------------------------------------------------
void MusicOggStream::volumeMusic(float gain)
{
    if (gain > 1.0f)
    {
        gain = 1.0f;
        fprintf(stderr, "WARNING: MusicOggStream::volumeMusic(%f) is out of acceptable [0, 1] range\n", gain);
    }
    if (gain < 0.0f)
    {
        gain = 0.0f;
        fprintf(stderr, "WARNING: MusicOggStream::volumeMusic(%f) is out of acceptable [0, 1] range\n", gain);
    }

	//调节音量
	//engine->setSoundVolume(gain);
	 
 
} // volumeMusic

//-----------------------------------------------------------------------------
void MusicOggStream::updateFading(float percent)
{
    
    update();
}   // updateFading

//-----------------------------------------------------------------------------
void MusicOggStream::updateFaster(float percent, float max_pitch)
{
     
    update();
}   // updateFaster

//-----------------------------------------------------------------------------
void MusicOggStream::update()
{

    if (m_pausedMusic )
    {
        // nothing todo
        return;
    }

 
    
    
}   // update

 
//-----------------------------------------------------------------------------
bool MusicOggStream::check(const char* what)
{
    

    return true;
}   // check

//-----------------------------------------------------------------------------
 
#endif // HAVE_OGGVORBIS
