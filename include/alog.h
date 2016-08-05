 
 #ifndef __LOG_H__
#define __LOG_H__

#include <stdlib.h>
#include <stdio.h>


#include <jni.h> 
  
 
#include <android/log.h>
#include <stdio.h>
 
 
#include <math.h>
 
 


#define DEBUG 1

#define LOG_TAG "irr_JNI"

#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)


#define DBG_LINE  	__android_log_print(ANDROID_LOG_INFO, "irrlicht", "file[%s]  fun[%s]  line[%d]",  __FILE__  , __FUNCTION__ , __LINE__ )

//#define DBG_LINE(s)  	__android_log_print(ANDROID_LOG_INFO, "irrlicht", "line[%d] fun[%s] [%s]",__LINE__,__FUNCTION__,s)


#if DEBUG

	#define ALOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)

#else
	#define ALOGV(...)
#endif











#endif
