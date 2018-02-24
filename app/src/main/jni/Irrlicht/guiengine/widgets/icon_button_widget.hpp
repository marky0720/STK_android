//  SuperTuxKart - a fun racing game with go-kart
//  Copyright (C) 2009 Marianne Gagnon
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



#ifndef HEADER_IBTN_HPP
#define HEADER_IBTN_HPP

#include <irrlicht.h>

#include "guiengine/widget.hpp"
#include "utils/ptr_vector.hpp"

namespace GUIEngine
{
    /** \brief A button widget with an icon and optionnaly a label beneath
      * \ingroup widgets
      */
    class IconButtonWidget : public Widget
    {
    public:
        enum ScaleMode
        {
            SCALE_MODE_STRETCH,
            SCALE_MODE_KEEP_TEXTURE_ASPECT_RATIO,
            SCALE_MODE_KEEP_CUSTOM_ASPECT_RATIO
        };
        enum IconPathType
        {
            ICON_PATH_TYPE_ABSOLUTE,
            /** relative to the data dir */
            ICON_PATH_TYPE_RELATIVE,
            /** not a valid value per se, but can be passed as argument to leave
              * the path type as it currently is */
            ICON_PATH_TYPE_NO_CHANGE
        };
        
    protected:
        
        IconPathType m_icon_path_type;
        
        friend class Skin;
        
        irr::gui::IGUIStaticText* m_label;
        irr::video::ITexture* m_texture;
        int m_texture_w, m_texture_h;
        
        ScaleMode m_scale_mode;
        float m_custom_aspect_ratio;
        
    public:

        /** Whether to make the widget included in keyboard navigation order when adding */
        bool m_tab_stop;

        IconButtonWidget(ScaleMode scale_mode=SCALE_MODE_KEEP_TEXTURE_ASPECT_RATIO, const bool tab_stop=true,
                         const bool focusable=true, IconPathType pathType=ICON_PATH_TYPE_RELATIVE);
        virtual ~IconButtonWidget() {}
        
        /** Callback called when this widget needs to be added (see base class Widget) */
        virtual void add();
        
        /** Call this if scale mode is SCALE_MODE_KEEP_CUSTOM_ASPECT_RATIO.
          * \param custom_aspect_ratio  The width/height aspect ratio */
        void setCustomAspectRatio(float custom_aspect_ratio) { m_custom_aspect_ratio = custom_aspect_ratio; }
        
        /** Change the text label if there is a label (label won't be added if there initially wasn't one) */
        void setLabel(irr::core::stringw new_label);
        
        /** Change the texture used for this icon. The path is relative to the data directory, just like PROP_ICON. */
        void setImage(const char* path_to_texture, IconPathType pathType=ICON_PATH_TYPE_NO_CHANGE);
        
        /** Change the texture used for this icon. */
        void setImage(irr::video::ITexture* texture);
    };
}

#endif
