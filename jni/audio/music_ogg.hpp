 
#ifndef HEADER_MUSICOGG_HPP
#define HEADER_MUSICOGG_HPP

#include <string>
#include <iostream>
 
 
 
#include "audio/music.hpp"

#include <stdio.h>



 
/**
  * \brief ogg files based implementation of the Music interface
  * \ingroup audio
  */
class MusicOggStream : public Music
{
public:
    MusicOggStream();
    virtual ~MusicOggStream();

    virtual void update();
    virtual void updateFading(float percent);
    virtual void updateFaster(float percent, float max_pitch);

    virtual bool load(const std::string& filename);

    virtual bool playMusic();
    virtual bool stopMusic();
    virtual bool pauseMusic();
    virtual bool resumeMusic();
    virtual void volumeMusic (float gain);
    virtual bool isPlaying();
    
protected:
    bool empty();
    bool check(const char* what);
 


// ISoundEngine* engine;
//
//	  ISoundSource* ballSound;

private:
    bool release();
 

    std::string     m_fileName;
    
    bool            m_error;

    bool            m_playing;
    
    

    bool m_pausedMusic;
 
};

#endif // HEADER_MUSICOGG_HPP
