#pragma once

#include <jni.h>
#include <android/log.h>
#include <irrlicht.h>

#include <sys/types.h>
#include <unistd.h>


using namespace irr;

using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

int importGLInit();
void importGLDeinit();
void nativeDrawIteration();

// global variables
int  gWindowWidth  = 320;
int  gWindowHeight = 480;
int  gAppAlive   = 1;
stringc gSdCardPath = "/sdcard/";


IrrlichtDevice *device = NULL;
IVideoDriver* driver = NULL;
int pos_x,pos_y;
bool LMOUSE_PRESSED_DOWN=false;

