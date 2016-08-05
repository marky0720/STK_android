// Copyright (C) 2002-2009 Laurent Mallet & Maxime Biais
// This file is part of the "Irrlicht Android Port".
// For conditions of distribution and use, see copyright notice in irrlicht.h
#include <jni.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <android/log.h>
#include <stdint.h>
#include <android-receiver.h>
#include <android-logger.h>
#include <importgl.h>

#include <irrlicht.h>
#include <os.h>

using namespace irr;

using namespace os;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;






extern IrrlichtDevice *device;
extern IVideoDriver* driver;
extern stringc gSdCardPath;


ISceneManager* smgr = NULL;
IGUIEnvironment* guienv = NULL;
IAnimatedMesh* mesh = NULL;
ISceneNode* node = NULL;
IAnimatedMeshSceneNode *nodeSydney = NULL;
ICameraSceneNode* camera = NULL;
IGUIStaticText* diagnostics = NULL;
SAppContext context;
AndroidEventReceiver *receiver;
gui::IGUIFont* font= NULL;

IGUIButton * bt;

class EventReceiver: public irr::IEventReceiver
{
public:
	virtual bool OnEvent (const irr::SEvent& e)
	{

		__android_log_print(ANDROID_LOG_INFO, "EventReceiver::OnEvent", " EventType:%d "
				, e.EventType  );

		 if (e.EventType == EET_GUI_EVENT)
		  {

			 	 if(e.GUIEvent.EventType == EGET_BUTTON_CLICKED)
			 	 {
			 		s32 id = e.GUIEvent.Caller->getID();

			 		 __android_log_print(ANDROID_LOG_INFO, "EventReceiver::OnEvent", "EGET_BUTTON_CLICKED  id:%d",id);


			 		if(id== bt->getID()){
			 			// gui::IGUIButton* bt =(gui::IGUIButton*)e.GUIEvent.Caller;
			 			core::stringw x=L"ff";
			 			x+=(char*)e.GUIEvent.Caller;
			 			bt->setText(x.c_str());
			 		}


			 	 }


		  }

		return false;
	}
};

EventReceiver eReceiver;

static long _getTime(void)
{
    struct timeval  now;

    gettimeofday(&now, NULL);
    return (long)(now.tv_sec*1000 + now.tv_usec/1000);
}


void initSydney()
{
	device->setEventReceiver(&eReceiver );//设置事件接收器

    smgr = device->getSceneManager();
    guienv = device->getGUIEnvironment();
 
    stringc myDir = gSdCardPath;
    myDir += "/Irrlicht";
    device->getFileSystem()->changeWorkingDirectoryTo(myDir.c_str());

    //font = guienv->getBuiltInFont();
   //	font = device->getGUIEnvironment()->getFont("/Irrlicht/fonthaettenschweiler.bmp");
    //font = guienv->getBuiltInFont();
	font = device->getGUIEnvironment()->getFont(
			(gSdCardPath+"/Irrlicht/fonthaettenschweiler.bmp").c_str() );


   	//IGUIButton * b=guienv->addButton( core::rect<s32>(0,150,200,300), 0, GUI_ID_QUIT_BUTTON, L"close" );
	  bt= guienv->addButton(rect<s32>(10,10,110,10 + 32), 0, GUI_ID_QUIT_BUTTON,
                            L"Quit", L"Exits Program");
    
    stringc sydneyFilename = "/Irrlicht/sydney.md2";
	mesh = smgr->getMesh( (gSdCardPath+sydneyFilename).c_str() );
	        if (!mesh)
	        {
	                device->drop();
					__android_log_print(ANDROID_LOG_INFO, "Irrlicht", "cannot getMesh");
					return;
	        }
	nodeSydney = smgr->addAnimatedMeshSceneNode( mesh );
	
	if (nodeSydney)
	        {
	                nodeSydney->setMaterialFlag(EMF_LIGHTING, false);
	                //--// no animation
	                //--nodeSydney->setMD2Animation(scene::EMAT_STAND);
	                // with animation
                    nodeSydney->setMD2Animation(scene::EMAT_RUN);
                    stringc sydneyTextureFilename = "/Irrlicht/sydney.bmp";
	                nodeSydney->setMaterialTexture( 0, driver->getTexture((gSdCardPath+sydneyTextureFilename).c_str()) );

	        }
	
	smgr->addCameraSceneNode(0, vector3df(0,10,-40), vector3df(0,5,0));
	
	// Then create the event receiver, giving it that context structure.
    receiver = new AndroidEventReceiver(context);

    // And tell the device to use our custom event receiver.
   // device->setEventReceiver(receiver);


}


static int counter = 0;

int fps=0;
extern int pos_x,pos_y;
extern bool LMOUSE_PRESSED_DOWN;
/* Call to render the next GL frame */
void nativeDrawIterationSydney()
{
    device->run();
    if (counter==0) {
        initSydney();
    }
	
	// SColor is the background color
	driver->beginScene(true, true, SColor(255,255,0,0));

	smgr->drawAll();
	guienv->drawAll();


	 core::position2d<s32> m = device->getCursorControl()->getPosition();
	//得到鼠标的坐标

	// draw some other text
	core::stringw tx=L"fps:";
	tx+=fps;
	tx+= "  x:";
	tx+= pos_x;
	tx+=" y:";
	tx+= pos_y;
	tx+=" state:";
	tx+=   LMOUSE_PRESSED_DOWN ;
	font->draw(  tx ,
						core::rect<s32>(130,10,300,50),
						video::SColor(255,255,255,255));

	driver->endScene();

	counter ++ ;


	fps = driver->getFPS();


  //  __android_log_print(ANDROID_LOG_INFO, "Irrlicht", "Sydney fps=%d", fps);

}

#include "main_loop.hpp"
void nativeDrawIteration()
{
	DBG_LINE;
	//MainLoop* main_loop
	main_loop->run();
	//ALOGV("nativeDrawIteration thread id %d",gettid());
   //  nativeDrawIterationSydney();
}

