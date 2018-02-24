#include <assert.h>

#include "config/device_config.hpp"

//==== D E V I C E C O N F I G =================================================

irr::core::stringw DeviceConfig::getBindingAsString (const PlayerAction action) const
{
    irr::core::stringw returnString = "";

    if ((action < PA_COUNT) && (action >= 0))
    {
        returnString += Input::getInputAsString(m_bindings[action].type, 
                                                m_bindings[action].id,
                                                m_bindings[action].dir);
    }

    return returnString;
}

//------------------------------------------------------------------------------

irr::core::stringw DeviceConfig::getMappingIdString (const PlayerAction action) const
{
    irr::core::stringw returnString = "";
    
    if ((action < PA_COUNT) && (action >= 0))
    {
        const Input::InputType type = m_bindings[action].type;
        const int id = m_bindings[action].id;
        const Input::AxisDirection dir = m_bindings[action].dir;
        
        switch (type)
        {
            case Input::IT_KEYBOARD:
                returnString += "keyb_";
                returnString += id;
                break;
                
            case Input::IT_STICKMOTION:
                returnString += "stkmo_";
                returnString += id;
                returnString += "$";
                returnString += dir;
                break;
                
            case Input::IT_STICKBUTTON:
                returnString += "stkbt_";
                returnString += id;
                break;
                
            case Input::IT_STICKHAT:
                returnString += "stkht_";
                returnString += id;
                break;
                
            case Input::IT_MOUSEMOTION:
                returnString += "mousemo_";
                returnString += id;
                returnString += "$";
                returnString += dir;
                break;
                
            case Input::IT_MOUSEBUTTON:
                returnString += "mousebtn_";
                returnString += id;
                break;
                
            default:
                assert(false);
                returnString += type;
                returnString += "_";
                returnString += id;
                returnString += "$";
                returnString += dir;
        }
    }
    
    return returnString;
}


//------------------------------------------------------------------------------

irr::core::stringw DeviceConfig::toString ()
{
    irr::core::stringw returnString = "";
    for (int n = 0; n < PA_COUNT; n++)
    {
        returnString += KartActionStrings[n].c_str();
        returnString += ": ";
        returnString += Input::getInputAsString(m_bindings[n].type, 
                                                m_bindings[n].id,
                                                m_bindings[n].dir);
        returnString += "\n";
    }
    return returnString;
}

//------------------------------------------------------------------------------

void DeviceConfig::setBinding ( const PlayerAction      action, 
                                const Input::InputType  type,
                                const int               id,
                                Input::AxisDirection    direction )
{
    m_bindings[action].type = type;
    m_bindings[action].id = id;
    m_bindings[action].dir = direction;
}

//------------------------------------------------------------------------------

// Don't call this directly unless you are KeyboardDevice or GamepadDevice
bool DeviceConfig::getGameAction(Input::InputType    type,
                                 const int           id,
                                 const int           value,
                                 PlayerAction*       action /* out */ )
{
    return doGetAction(type, id, value, PA_FIRST_GAME_ACTION, PA_LAST_GAME_ACTION, action);
}

//------------------------------------------------------------------------------

// Don't call this directly unless you are KeyboardDevice or GamepadDevice
bool DeviceConfig::getMenuAction(Input::InputType    type,
                                 const int           id,
                                 const int           value,
                                 PlayerAction*       action /* out */ )
{
    return doGetAction(type, id, value, PA_FIRST_MENU_ACTION, PA_LAST_MENU_ACTION, action);
}

//------------------------------------------------------------------------------

bool DeviceConfig::doGetAction(Input::InputType    type,
                               const int           id,
                               const int           value,
                               const PlayerAction  firstActionToCheck,
                               const PlayerAction  lastActionToCheck,
                               PlayerAction*       action /* out */ )
{
    bool success = false;
    int  n;

    for (n = firstActionToCheck; ((n <= lastActionToCheck) && (!success)); n++)
    {
        if ((m_bindings[n].type == type) && (m_bindings[n].id == id))
        {

            if (type == Input::IT_STICKMOTION)
            {
                if ( ((m_bindings[n].dir == Input::AD_POSITIVE) && (value > 0)) ||
                     ((m_bindings[n].dir == Input::AD_NEGATIVE) && (value < 0)) )
                {
                    success = true;
                   *action = (PlayerAction)n;
                }
            }
            else
            {
                success = true;
               *action = (PlayerAction)n;
            }
        } 
    } // end for n

    return success;
}

//------------------------------------------------------------------------------

void DeviceConfig::serialize (std::ofstream& stream)
{
    for(int n = 0; n < PA_COUNT; n++) // Start at 0?
    {
        stream << "    "
               << "<action "
                   << "name=\""      << KartActionStrings[n] << "\" "
                   << "id=\""        << m_bindings[n].id     << "\" "
                   << "event=\""     << m_bindings[n].type   << "\" ";
        
        // Only serialize the direction for stick motions
        if (m_bindings[n].type == Input::IT_STICKMOTION)
        {
            stream << "direction=\"" << m_bindings[n].dir    << "\" ";
        }

        stream   << "/>\n";
    }
}

//------------------------------------------------------------------------------

bool DeviceConfig::deserializeAction(irr::io::IrrXMLReader* xml)
{
    bool                 success = false;
    int                  binding_id = -1;
    int                  id;
    Input::InputType     type;
    Input::AxisDirection dir;

    // Never hurts to check ;)
    if (xml == NULL)
    {
        fprintf(stderr, "Error: null pointer (DeviceConfig::deserializeAction)\n");
    }
    else
    {
    // Read tags from XML
        const char *name_string     = xml->getAttributeValue("name");
        const char *id_string       = xml->getAttributeValue("id");
        const char *event_string    = xml->getAttributeValue("event");
        const char *dir_string      = xml->getAttributeValue("direction");
    
        // Proceed only if neccesary tags were found
        if ((name_string != NULL) && (id_string != NULL) && 
            (event_string != NULL))
        {
            // Convert strings to string tags to integer types
            type = (Input::InputType)atoi(event_string);
            id = atoi(id_string);

            // Try to determine action # for verbose action name
            for (int n = 0; ((n < PA_COUNT) && (binding_id == -1)); n++)
            {
                if (strcmp(name_string, KartActionStrings[n].c_str()) == 0)
                    binding_id = n;
            }

            // If action # was found then store the bind
            if (binding_id != -1)
            {
                // If the action is not a stick motion (button or key)
                if (type != Input::IT_STICKMOTION)
                {
                    setBinding((PlayerAction)binding_id, type, id);
                    success = true;
                }

                // If the action is a stick motion & a direction is defined
                else if (dir_string != NULL)
                {
                    dir = (Input::AxisDirection)atoi(dir_string);
                    setBinding((PlayerAction)binding_id, type, id, dir);
                    success = true;
                }
                else
                {
                    printf("WARNING: IT_STICKMOTION without direction, ignoring.\n");
                }
            } // end if binding_id != -1
            else
            {
                printf("WARNING: DeviceConfig::deserializeAction : action '%s' is unknown\n", name_string);
            }
            
        } // end if name_string != NULL ...
    } // end if xml == NULL ... else

    return success;
}


//  KeyboardConfig & GamepadConfig classes really should be in a separate cpp
//  file but they are so small that we'll just leave them here for now.

//==== K E Y B O A R D C O N F I G =============================================

void KeyboardConfig::serialize (std::ofstream& stream)
{
    stream << "<keyboard>\n\n";
    DeviceConfig::serialize(stream);
    stream << "</keyboard>\n\n\n";
}

//------------------------------------------------------------------------------

void KeyboardConfig::setDefaultBinds()
{
    setBinding(PA_NITRO,       Input::IT_KEYBOARD, KEY_KEY_N);
    setBinding(PA_ACCEL,       Input::IT_KEYBOARD, KEY_UP);
    setBinding(PA_BRAKE,       Input::IT_KEYBOARD, KEY_DOWN);
    setBinding(PA_STEER_LEFT,  Input::IT_KEYBOARD, KEY_LEFT);
    setBinding(PA_STEER_RIGHT, Input::IT_KEYBOARD, KEY_RIGHT);
    setBinding(PA_DRIFT,       Input::IT_KEYBOARD, KEY_KEY_V);
    setBinding(PA_RESCUE,      Input::IT_KEYBOARD, KEY_BACK);
    setBinding(PA_FIRE,        Input::IT_KEYBOARD, KEY_SPACE);
    setBinding(PA_LOOK_BACK,   Input::IT_KEYBOARD, KEY_KEY_B);
    
    
    setBinding(PA_MENU_UP,     Input::IT_KEYBOARD, KEY_UP);
    setBinding(PA_MENU_DOWN,   Input::IT_KEYBOARD, KEY_DOWN);
    setBinding(PA_MENU_LEFT,   Input::IT_KEYBOARD, KEY_LEFT);
    setBinding(PA_MENU_RIGHT,  Input::IT_KEYBOARD, KEY_RIGHT);
    setBinding(PA_MENU_SELECT, Input::IT_KEYBOARD, KEY_RETURN);
    setBinding(PA_MENU_CANCEL, Input::IT_KEYBOARD, KEY_BACK);
}

//------------------------------------------------------------------------------

KeyboardConfig::KeyboardConfig() : DeviceConfig(DEVICE_CONFIG_TYPE_KEYBOARD)
{
    m_name = "Keyboard";
    setPlugged(true);
    setDefaultBinds();
}

//==== G A M E P A D C O N F I G ===============================================

void GamepadConfig::serialize (std::ofstream& stream)
{
    stream << "<gamepad name =\"" << m_name.c_str() << "\">\n\n";
    DeviceConfig::serialize(stream);
    stream << "</gamepad>\n\n\n";
}

//------------------------------------------------------------------------------

void GamepadConfig::setDefaultBinds ()
{
    setBinding(PA_STEER_LEFT,   Input::IT_STICKMOTION, 0, Input::AD_NEGATIVE);
    setBinding(PA_STEER_RIGHT,  Input::IT_STICKMOTION, 0, Input::AD_POSITIVE);
    setBinding(PA_ACCEL,        Input::IT_STICKMOTION, 1, Input::AD_NEGATIVE);
    setBinding(PA_BRAKE,        Input::IT_STICKMOTION, 1, Input::AD_POSITIVE);
    setBinding(PA_FIRE,         Input::IT_STICKBUTTON, 0);
    setBinding(PA_NITRO,        Input::IT_STICKBUTTON, 1);
    setBinding(PA_DRIFT,        Input::IT_STICKBUTTON, 2);
    setBinding(PA_RESCUE,       Input::IT_STICKBUTTON, 3);
    setBinding(PA_LOOK_BACK,    Input::IT_STICKBUTTON, 4);
    
    setBinding(PA_MENU_UP,      Input::IT_STICKMOTION, 1, Input::AD_NEGATIVE);
    setBinding(PA_MENU_DOWN,    Input::IT_STICKMOTION, 1, Input::AD_POSITIVE);
    setBinding(PA_MENU_LEFT,    Input::IT_STICKMOTION, 0, Input::AD_NEGATIVE);
    setBinding(PA_MENU_RIGHT,   Input::IT_STICKMOTION, 0, Input::AD_POSITIVE);
    setBinding(PA_MENU_SELECT,  Input::IT_STICKBUTTON, 0);
    setBinding(PA_MENU_CANCEL,  Input::IT_STICKBUTTON, 3);
}

//------------------------------------------------------------------------------

GamepadConfig::GamepadConfig   ( const std::string      name,
                                 const int              axis_count,
                                 const int              btnCount ) : DeviceConfig( DEVICE_CONFIG_TYPE_GAMEPAD )
{
    m_name = name;
    m_axis_count = axis_count;
    m_button_count = btnCount;
    setPlugged(false);
    setDefaultBinds();
}

//------------------------------------------------------------------------------

GamepadConfig::GamepadConfig(irr::io::IrrXMLReader* xml) : DeviceConfig( DEVICE_CONFIG_TYPE_GAMEPAD )
{
    const char* name_string = xml->getAttributeValue("name");
    if(name_string == NULL)
    {
        printf("ERROR: Unnamed joystick in config file\n");
    }
    else 
    {
        m_name = name_string;
    }
    setPlugged(false);
    setDefaultBinds();
}

//------------------------------------------------------------------------------

irr::core::stringw GamepadConfig::toString ()
{
    irr::core::stringw returnString = "";
    returnString += getName().c_str();
    returnString += "\n";
    returnString += DeviceConfig::toString();
    return returnString;
}

//------------------------------------------------------------------------------

bool DeviceConfig::hasBindingFor(const int buttonID) const
{
    for (int n=0; n<PA_COUNT; n++)
    {
        if (m_bindings[n].id == buttonID) return true;
    }
    return false;
}
