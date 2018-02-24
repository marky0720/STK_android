#ifndef HEADER_ABSTRACT_STATE_MANAGER_HPP
#define HEADER_ABSTRACT_STATE_MANAGER_HPP

#include <vector>
#include <string>

/**
  * \ingroup guiengine
  */
namespace GUIEngine
{
    class Widget;
    class Screen;
    
    /**
      * \ingroup guiengine
      */
    enum GameState
    {
        MENU,
        GAME,
        INGAME_MENU
    };
    
    /**
     * \brief Abstract base class you must override from to use the GUI engine
     * \ingroup guiengine
     */
    class AbstractStateManager
    {
    protected:
        /**
         * Whether we are in game mode
         */
        GameState m_game_mode;
        
        /**
         * This stack will contain menu names (e.g. main.stkgui), and/or 'race'.
         */
        std::vector<std::string> m_menu_stack;
        
        void pushMenu(std::string name);
        
        void setGameState(GameState state);
        
    public:
        
        /** inits an AbstractStateManager is MENU state */
        AbstractStateManager();
        
        virtual ~AbstractStateManager() { }
        
        /** \brief adds a menu at the top of the screens stack */
        void pushScreen(Screen* screen);
        
        /** \brief replaces the menu at the top of the screens stack
          * (i.e. pops the topmost screen and adds this one instead, but 
          * without displaying the second-topmost menu of the stack 
          * in-between)
          */
        void replaceTopMostScreen(Screen* screen);
        
        /**
          * \brief removes the menu at the top of the screens stack
          * If the stack becomes empty after performing the pop (i.e. if it 
          * contained only one item prior to the call), the game is aborted.
          * In other cases, the second-topmost screen is displayed.
          */
        void popMenu();
        
        /**
          * \brief clears the menu stack and starts afresh with a new stack 
          *  containing only the given screen
          */
        void resetAndGoToScreen(Screen* screen);
        
        /**
         * \brief Sets the whole menu stack.
         * Only the topmost screen will be inited/shown, but others remain
         * under for cases where the user wants to go back.
         * \param screens an array containing the menus that should go into 
         *  stack. The first item will be the bottom item in the stack, the 
         *  last item will be the stack top. Array must be NULL-terminated.
         */
        void resetAndSetStack(Screen* screens[]);
        
        /**
          * \brief call to make the state manager enter game mode.
          * Causes the menu stack to be cleared; all widgets shown on screen 
          * are removed
          */
        void enterGameState();
        
        /** \return the current state of the game */
        GameState getGameState();
        
        /** \brief to be called after e.g. a resolution switch */
        void reshowTopMostMenu();
        
        /*  ***********************************
         * methods to override in children *
         *********************************** */
        
        /**
          * \brief callback invoked whenever escape was pressed (or any 
          *  similar cancel operation)
          */
        virtual void escapePressed() = 0;
        
        /**
          * \brief callback invoked when game mode changes (e.g. goes from 
          *  "menu" to "in-game")
          */
        virtual void onGameStateChange(GameState new_state) = 0;
        
        /**
          * \brief callback invoked when the stack is emptied (all menus are 
          * popped out). This is essentially a request to close the 
          * application (since a game can't run without a state)
          */
        virtual void onStackEmptied() = 0;
        
        virtual void onTopMostScreenChanged() = 0;
        
    };
    
}
#endif
