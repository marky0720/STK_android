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

#include "states_screens/dialogs/confirm_dialog.hpp"

#include "guiengine/engine.hpp"
#include "guiengine/screen.hpp"
#include "guiengine/widgets/label_widget.hpp"
#include "states_screens/state_manager.hpp"

using namespace GUIEngine;

// ------------------------------------------------------------------------------------------------------

ConfirmDialog::ConfirmDialog(irr::core::stringw msg, IConfirmDialogListener* listener) :
    ModalDialog(0.6f, 0.6f)
{    
    loadFromFile("confirm_dialog.stkgui");

    m_listener = listener;
    
    LabelWidget* message = getWidget<LabelWidget>("title");
    message->setText( msg.c_str() );
}

// ------------------------------------------------------------------------------------------------------

void ConfirmDialog::onEnterPressedInternal()
{
}

// ------------------------------------------------------------------------------------------------------

GUIEngine::EventPropagation ConfirmDialog::processEvent(const std::string& eventSource)
{
    
    if (eventSource == "cancel")
    {           
        m_listener->onCancel();
        
        return GUIEngine::EVENT_BLOCK;
    }
    else if (eventSource == "confirm")
    {        
        m_listener->onConfirm();
        
        return GUIEngine::EVENT_BLOCK;
    }
    
    return GUIEngine::EVENT_LET;
}

// ------------------------------------------------------------------------------------------------------

void ConfirmDialog::IConfirmDialogListener::onCancel()
{
    ModalDialog::dismiss();
}
