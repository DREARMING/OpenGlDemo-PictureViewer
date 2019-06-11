//
// Created by mvcoder on 2019/6/10.
//

#ifndef OPENGLDEMO_PICTUREVIEWER_H
#define OPENGLDEMO_PICTUREVIEWER_H

#include <jni.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <assert.h>
#include "egl_core.h"
#include "egl_share_context.h"
#include "video_gl_surface_render.h"
#include "video_gl_water_surface_render.h"
#include "SOIL.h"
//#include "stb_image.h"

class PictureViewer {
private:
    EGLCore* eglCore;
    EGLSurface renderTexSurface;
    VideoGLSurfaceRender* renderer;
    VideoGLWaterSurfaceRender* waterRender;
    ANativeWindow* surfaceWindow;
    int screenWidth;
    int screenHeight;
    bool createEGLContext();
    void createWindowSurface(ANativeWindow* window);
    GLuint createTexture(byte* data, int width, int height);
public:
    PictureViewer();
    virtual ~PictureViewer();
    void onSurfaceCreate(ANativeWindow* nativeWindow, int width, int height);
    void onSurfaceDestroy();
    void showPicture(char* imageUrl);
    void showWaterPicture(char* imageUrl, byte* waterData, int width, int height);

};


#endif //OPENGLDEMO_PICTUREVIEWER_H
