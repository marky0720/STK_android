//  $Id: string_utils.cpp 6180 2010-10-01 11:00:22Z hikerstk $
//
//  SuperTuxKart - a fun racing game with go-kart
//  Copyright (C) 2004 Steve Baker <sjbaker1@airmail.net>,
//                     Ingo Ruhnke <grumbel@gmx.de>
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

#include "utils/string_utils.hpp"

#include "coreutil.h"

#include "math.h"
#include <algorithm>
#include <cstring>
#include <stdio.h>
#include <exception>
#include <assert.h>

namespace StringUtils
{
    bool hasSuffix(const std::string& lhs, const std::string rhs)
    {
        if (lhs.length() < rhs.length())
            return false;
        else
            // While this is basically correct, it fails with older
            // g++ versions (at least 2.95.3), which have a wrong template. To
            // avoid this issue, a more C-traditional way is used.
            return strcmp(lhs.c_str()+(lhs.length()-rhs.length()), rhs.c_str())==0;
    }   // hasSuffix
    
    //--------------------------i-----------------------------------------------
    /** Returns the path of a filename, i.e. everything till the last '/'.
     */
    std::string getPath(const std::string& filename)
    {
        for(int i = int(filename.size()) - 1; i >= 0; --i)
        {
            if (filename[i]=='/' || filename[i]=='\\')
            {
                return filename.substr(0,i);
            }
        }
        return "";
    }   // getPath
    
    //-------------------------------------------------------------------------
    /** Returns the basename of a filename, i.e. everything after the last "/".
     */
    std::string getBasename(const std::string& filename)
    {
        for(int i = int(filename.size()) - 1; i >= 0; --i)
        {
            if (filename[i]=='/' || filename[i]=='\\')
            {
                return filename.substr(i+1);
            }
        }
        return filename;
    }   // getBasename
    
    //-------------------------------------------------------------------------
    /** Removes the extension, i.e. everything after the last ".".
     */
    std::string removeExtension(const std::string& filename)
    {
        for(int i = int(filename.size()) - 1; i >= 0; --i)
        {
            if (filename[i] == '.')
            {
                return filename.substr(0, i);
            }
        }
        return filename;
    }   // removeExtension
    
    //-------------------------------------------------------------------------
    /** Returns the extension, i.e. everything after the last "."
     */
    std::string getExtension(const std::string& filename)
    {
        for(int i = int(filename.size()) - 1; i >= 0; --i)
        {
            if (filename[i] == '.')
            {
                return filename.substr(i+1);
            }
        }
        return filename;
    }   // getExtension
    
    //-------------------------------------------------------------------------
    /** Returns a string converted to upper case.
     */
#include <ctype.h>
    std::string toUpperCase(const std::string& str)
    {
        std::string name = str;
        std::transform(name.begin(), name.end(), name.begin(),  toupper);
        return name;
    }   // toUpperCase
    
    //-----------------------------------------------------------------------------
    /** Splits a string into substrings separated by a certain character, and 
     *  returns a std::vector of all those substring. E.g.:
     *  split("a b=c d=e",' ')  --> ["a", "b=c", "d=e"]
     *  \param s The string to split.
     *  \param c The character  by which the string is split.
     */
    std::vector<std::string> split(const std::string& s, char c, bool keepSplitChar)
    {
        std::vector<std::string> result;
        
        try
        {
            std::string::size_type start=0;
            while(start!=std::string::npos)
            {
                std::string::size_type i=s.find(c, start);
                if (i!=std::string::npos)
                {
                    if (keepSplitChar)
                    {
                        int from = start-1;
                        if (from < 0) from = 0;

                        result.push_back(std::string(s, from, i-from));
                    }
                    else result.push_back(std::string(s,start, i-start));
                    
                    start=i+1;
                } 
                else   // end of string reached
                {
                    if (keepSplitChar) result.push_back(std::string(s,start-1));
                    else result.push_back(std::string(s,start));
                    start = i;
                }
            }
            return result;
        }
        catch (std::exception& e)
        {
            (void)e;  // avoid warning message about unused variable
            fprintf(stderr, "Fatal error in split(std::string) : %s @ line %i\n", __FILE__, __LINE__);
            printf("Splitting %s\n", s.c_str());
            
            for (int n=0; n<(int)result.size(); n++)
            {
                printf("Split : %s\n", result[n].c_str());
            }
            
            exit(1);
        }
    }   // split
    
    /** Same as above, but for wide strings */
    std::vector<irr::core::stringw> split(const irr::core::stringw& s, char c, bool keepSplitChar)
    {
        try
        {
            std::vector<irr::core::stringw> result;
            
            irr::s32 start = 0;
            while (start < (irr::s32)s.size())
            {
                irr::s32 i = s.findNext(c, start);
                if (i != -1)
                {
                    if (keepSplitChar)
                    {
                        int from = start-1;
                        if (from < 0) from = 0;
                        result.push_back( s.subString(from, i-from) );
                    }
                    else result.push_back( s.subString(start, i-start) );
                    start = i+1;
                } 
                else
                {
                    if (keepSplitChar) result.push_back( s.subString(start - 1, s.size()-start + 1) );
                    else result.push_back( s.subString(start, s.size()-start) );
                    
                    return result;
                    //start = i+1;
                }
            }
            return result;
        }
        catch (std::exception& e)
        {
            (void)e;  // avoid warning about unused variable
            fprintf(stderr, "Fatal error in split(stringw) : %s @ line %i\n", __FILE__, __LINE__);
            exit(1);
        }
    }   // split
    
    
    // ------------------------------------------------------------------------
    /** Splits a : separated string (like PATH) into its individual components.
     *  It especially handles Windows-style paths (c:/mydir1:d:/mydir2)
     *  correctly, and removes a trailing "/" which can cause a problem with
     *  windows' stat function.
     *  \param path The string to split.
     */
    std::vector<std::string> splitPath(const std::string& path)
    {
        try
        {
            std::vector<std::string> dirs=StringUtils::split(path,':');
            for(int i=(int)dirs.size()-1; i>=0; i--)
            {
                // Remove '/' at the end of paths, since this can cause
                // problems with windows when using stat()
                while(dirs[i].size()>=1 && dirs[i][dirs[i].size()-1]=='/')
                {
                    dirs[i]=dirs[i].substr(0, dirs[i].size()-1);
                }
                // remove empty entries
                if(dirs[i].size()==0)
                {
                    dirs.erase(dirs.begin()+i);
                    continue;
                }
            }   // for i
#ifdef WIN32
            // Handle filenames like d:/dir, which becomes ["d","/dir"]
            for(int i=(int)dirs.size()-1; i>=0; i--)
            {
                if(dirs[i].size()>1) continue;
                if(i==dirs.size()-1)    // last element
                {
                    dirs[i]+=":";      // turn "c" back into "c:"
                }
                else
                {
                    dirs[i]+=":"+dirs[i+1]; // restore "d:/dir" back
                    dirs.erase(dirs.begin()+i+1);
                }
            }   // for i
#endif
            return dirs;
        }
        catch (std::exception& e)
        {
            (void)e;  // avoid warning about unused variable
            fprintf(stderr, "Fatal error in splitPath : %s @ line %i\n", __FILE__, __LINE__);
            exit(1);
        }
    }   // splitPath
    
    // ------------------------------------------------------------------------
    std::string insertValues(const std::string &s, std::vector<std::string>& all_vals)
    {
        try
        {
            std::vector<std::string> sv = StringUtils::split(s, '%', true);
            std::string new_string="";
            
            unsigned int insertValID = 0;
            
            const unsigned int item_count = sv.size();
            for (unsigned int i=0; i<item_count; i++)
            {
                if(sv[i][0] != '%')
                {
                    new_string += sv[i];
                }
                else
                {
                    if(sv[i][1]=='s' || sv[i][1]=='d' || sv[i][1]=='i')
                    {
                        assert(all_vals.size() > insertValID);
                        new_string += all_vals[insertValID] + sv[i].substr(2);
                    }
                    else if(sv[i][1]>='0' && sv[i][1]<= '9')
                    {
                        const unsigned int index = sv[i][1] - '0';
                        assert(all_vals.size() > index);
                        new_string += all_vals[index] + sv[i].substr(2);
                    }
                    else
                    {
                        new_string += sv[i];
                    }
                }
            }
            return new_string;
        }
        catch (std::exception& e)
        {
            (void)e;  // avoid warning about unused variable
            fprintf(stderr,"Fatal error in insertValues(std::string) : %s @ line %i\n", __FILE__, __LINE__);
            exit(1);
        }
    }
    
    // ------------------------------------------------------------------------
    irr::core::stringw insertValues(const irr::core::stringw &s, std::vector<irr::core::stringw>& all_vals)
    {
        try
        {
            unsigned int insertValID = 0;

            std::vector<irr::core::stringw> sv = StringUtils::split(s, '%', true);
            irr::core::stringw new_string="";
            for (unsigned int i=0; i<sv.size(); i++)
            {
                if(sv[i][0] != '%')
                {
                    new_string += sv[i];
                }
                else
                {
                    if (sv[i][1]=='s' || sv[i][1]=='d' || sv[i][1]=='i')
                    {
                        new_string += all_vals[insertValID].c_str();
                        new_string += sv[i].subString(2, sv[i].size()-2);
                        insertValID++;
                    }
                    else if(irr::core::isdigit(sv[i][1]))
                    {
                        const unsigned int index = irr::core::stringc(sv[i].c_str()).c_str()[1] - '0';
                        assert(all_vals.size() > index);
                        new_string += all_vals[index] + sv[i].subString(2, sv[i].size()-2);
                    }
                    else
                    {
                        new_string+=sv[i];
                    }
                }
            }
            return new_string;
        }
        catch (std::exception& e)
        {
            (void)e;  // avoid warning about unused variable
            fprintf(stderr,"Fatal error in insertValues(stringw) : %s @ line %i\n", __FILE__, __LINE__);
            exit(1);
        }
    }
    
    
    // ------------------------------------------------------------------------
    /** Converts a time in seconds into a string of the form mm:ss:hh (minutes,
     *  seconds, 1/100 seconds.
     *  \param time Time in seconds.
     */
    std::string timeToString(float time)
    {
        int int_time   = (int)(time*100.0f+0.5f);

        // Avoid problems if time is negative or way too large (which 
        // should only happen if something is broken in a track elsewhere, 
        // and an incorrect finishing time is estimated.
        if(int_time<0)
            return std::string("00:00:00");
        else if(int_time >= 10000*60)  // up to 9999:59.99
            return std::string("**:**:**");

        int min        = int_time / 6000;
        int sec        = (int_time-min*6000)/100;
        int hundredths = (int_time - min*6000-sec*100);
        // The proper c++ way would be:
        // std::ostringstream s;
        // s<<std::setw(2)<<std::setfill(' ')<<min<<":"
        //     <<std::setw(2)<<std::setfill('0')<<sec<<":"
        //     <<std::setw(2)<<std::setfill(' ')<<hundredths;
        // return s.str();
        // but that appears to be awfully complicated and slow, compared to
        // which admittedly only works for min < 100000 - which is about 68 
        // days - good enough.
        char s[12];
        sprintf ( s, "%02d:%02d:%02d", min,  sec,  hundredths) ;
        return std::string(s);
    }   // timeToString
    
} // namespace StringUtils

/* EOF */
