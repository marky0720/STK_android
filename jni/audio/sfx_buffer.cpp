//  $Id: sfx_buffer.cpp 6245 2010-10-11 02:13:45Z auria $
//
//  SuperTuxKart - a fun racing game with go-kart
//  Copyright (C) 2010 Marianne Gagnon
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

#include "audio/sfx_buffer.hpp"
#include "audio/sfx_manager.hpp"
#include "io/file_manager.hpp"
#include "utils/constants.hpp"
 
// 
//
////----------------------------------------------------------------------------
//
//SFXBuffer::SFXBuffer(const std::string& file,
//                     bool  positional,
//                     float rolloff,
//                     float gain)
//{
//    m_buffer     = 0;
//    m_gain       = 1.0f;
//    m_rolloff    = 0.1f;
//    m_positional = false;
//    m_loaded     = false;
//    m_file       = file;
//    
//    m_rolloff    = rolloff;
//    m_positional = positional;
//    m_gain       = gain;
//}
//
////----------------------------------------------------------------------------
//
//SFXBuffer::SFXBuffer(const std::string& file,
//                     const XMLNode* node)
//{
//    m_buffer     = 0;
//    m_gain       = 1.0f;
//    m_rolloff    = 0.1f;
//    m_positional = false;
//    m_loaded     = false;
//    m_file       = file;
//    
//    node->get("rolloff",     &m_rolloff    );
//    node->get("positional",  &m_positional );
//    node->get("volume",      &m_gain       );
//}
//
////----------------------------------------------------------------------------
//
//bool SFXBuffer::load()
//{
//    
//    return true;
//}
//
////----------------------------------------------------------------------------
//
//void SFXBuffer::unload()
//{
//   
//}
//
////----------------------------------------------------------------------------
///** Load a vorbis file into an OpenAL buffer
// *  based on a routine by Peter Mulholland, used with permission (quote : 
// *  "Feel free to use")
// */
//bool SFXBuffer::loadVorbisBuffer(const std::string &name, ALuint buffer)
//{
//    const int ogg_endianness = (IS_LITTLE_ENDIAN ? 0 : 1);
//    
//    
//    
//    return 1;
//}
//
