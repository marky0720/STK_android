#include <android-receiver.h>
#include <android/log.h>


extern int gAppAlive;

bool AndroidEventReceiver::OnEvent(const SEvent& event)
{
    __android_log_print(ANDROID_LOG_INFO, "AndroidEventReceiver::OnEvent begin", "11");
    
    __android_log_print(ANDROID_LOG_INFO, " AndroidEventReceiver::OnEvent ", "EventType:%d  ",
    		  event.EventType  	);


    if (event.EventType == EET_GUI_EVENT)
    {
    	__android_log_print(ANDROID_LOG_INFO, "Irrlicht", " event.EventType == EET_GUI_EVENT  ");

    	if(event.GUIEvent.Caller==NULL)
    	{
    		__android_log_print(ANDROID_LOG_INFO, "Irrlicht", "  event.GUIEvent.Caller==NULL!!   ");
    		return false;
    	}

        s32 id = event.GUIEvent.Caller->getID();
        IGUIEnvironment* env = Context.device->getGUIEnvironment();

        __android_log_print(ANDROID_LOG_INFO, "Irrlicht", "Lolo 2: %d %d %d %d", event.GUIEvent.EventType, EGET_BUTTON_CLICKED, id , GUI_ID_QUIT_BUTTON);
        switch(event.GUIEvent.EventType)
        {
            case EGET_BUTTON_CLICKED:
                __android_log_print(ANDROID_LOG_INFO, "Irrlicht", "Lolo 3");
                switch(id)
                {
                    case GUI_ID_QUIT_BUTTON:
                    	{
                    	//	 IGUIButton* bt=( IGUIButton*)event.GUIEvent.Caller;
                    		//bt->setText(L"ddd");
                    		__android_log_print(ANDROID_LOG_INFO, "Irrlicht", "GUI_ID_QUIT_BUTTON onclick");
                    		//   gAppAlive = 0;
                    		return true;
                    	}

                }
        }
    }

    __android_log_print(ANDROID_LOG_INFO, "AndroidEventReceiver::OnEvent end", "un deal");
    return false;
}
