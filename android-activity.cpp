#include <jni.h>
#include "dstring.h"
#include <android/log.h>
#include "dreamCompileConfig.h"
#include "SDreamCreationParameters.h"
#include "IVideoDriver.h"
#include "IFileSystem.h"
#include "IReadFile.h"
#include "ITexture.h"
#include "os.h"
#include "DreamDevice.h"
#include <stdio.h>
extern "C" int importGLInit();
extern "C" void importGLDeinit();
void nativeDrawIteration();

// global variables
int  gWindowWidth  = 320;
int  gWindowHeight = 480;
int  gAppAlive   = 1;
stringc gSdCardPath = "mnt/sdcard/dream/";


DreamDevice *device = NULL;
IVideoDriver* driver = NULL;
IFileSystem*  fs = NULL;
ITexture* t = NULL;
extern DreamDevice* createDevice(E_DRIVER_TYPE driverType,
		const dimension2d<u32>& windowSize,
		u32 bits, bool fullscreen,
		bool stencilbuffer, bool vsync);

/* For JNI: C++ compiler need this */
extern "C" {

/** Activity onCreate */
void Java_com_changer_dream_DreamTest_nativeOnCreate( JNIEnv*  env )
{
    
}

/** Activity onPause */
void Java_com_changer_dream_DreamTest_nativeOnPause( JNIEnv*  env )
{

}

/** Activity onResume */
void Java_com_changer_dream_DreamTest_nativeOnResume( JNIEnv*  env )
{
    
}

/** Activity onDestroy */
void Java_com_changer_dream_DreamTest_nativeOnDestroy( JNIEnv*  env )
{
    importGLDeinit();
	device->releaseRef();
}

void Java_com_changer_dream_DreamTest_nativeInitGL( JNIEnv*  env )
{
    importGLInit();
    device = createDevice( EDT_OGLES2, dimension2d<u32>(gWindowWidth, gWindowHeight), 16, false, false, false);
    driver = device->getVideoDriver();
    
	__android_log_print(ANDROID_LOG_INFO, "Dream", "createDevice r=%d w=%d h=%d", device, gWindowWidth, gWindowHeight);
	__android_log_print(ANDROID_LOG_INFO, "Dream", "getVideoDriver r=%d", driver);

    if (!device)
        __android_log_print(ANDROID_LOG_INFO, "Dream", "No device");
    if (!driver)
        __android_log_print(ANDROID_LOG_INFO, "Dream", "No driver");
	fs = device->getFileSystem();	
	fs->addFileArchive(gSdCardPath+"source");
	IReadFile* bmp = fs->createAndOpenFile("as.bmp");
	
	if(bmp) {
		t = driver->getTexture(bmp);
		if(t==NULL)
			Printer::log("we load Texture failed",ELL_ERROR);
		bmp->releaseRef();
	}
	
}

void Java_com_changer_dream_DreamTest_nativeResize( JNIEnv*  env, jobject  thiz, jint w, jint h )
{
    __android_log_print(ANDROID_LOG_INFO, "Dream", "resize w=%d h=%d", w, h);
    gWindowWidth  = w;
    gWindowHeight = h;
    dimension2d<unsigned int> size;
    size.Width = w;
    size.Height = h; 
    device->getVideoDriver()->OnResize(size);
}

void Java_com_changer_dream_DreamTest_nativeSendEvent( JNIEnv*  env, jobject defaultObj, jobject event) 
{
   
}

void Java_com_changer_dream_DreamTest_nativeGetStatus(JNIEnv*  env, jobject defaultObj, jobject status) 
{
    if (gAppAlive==0) {
        jclass classStatus = env->GetObjectClass(status);
        jfieldID fieldStatus = env->GetFieldID(classStatus, "mQuit", "Z");
        env->SetBooleanField(status, fieldStatus, true);
    }    
}


void Java_com_changer_dream_DreamTest_nativeEnvJ2C(JNIEnv*  env, jobject defaultObj, jstring jsdcardPath) 
{
    char ligne[1024+1];
    const char *msg = env->GetStringUTFChars(jsdcardPath,0);
    gSdCardPath = msg;
    __android_log_print(ANDROID_LOG_INFO, "Dream", "not handled %s", gSdCardPath.c_str());
    env->ReleaseStringUTFChars(jsdcardPath,msg);
}

void Java_com_changer_dream_DreamTest_nativeDrawIteration( JNIEnv*  env ) 
{
    nativeDrawIteration();   
}


}
void nativeDrawIteration() {
	driver->beginScene(true, true, SColor(255,100,101,140));
	if(t)
		driver->draw2DImage(t,position2di(0,0));
	driver->endScene();
}