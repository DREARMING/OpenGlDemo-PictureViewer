//
// Created by mvcoder on 2019/6/10.
//

#include "PictureViewer.h"

#define LOG_TAG "PictureViewer"

JNIEXPORT void onSurfaceCreate(JNIEnv* env, jobject obj, jlong context,jobject surface, jint width, jint height){
    LOGI("onSurfaceCreate");
    if(context != -1){
        PictureViewer* viewer = reinterpret_cast<PictureViewer *>(context);
        if(viewer != NULL){
            ANativeWindow* window = ANativeWindow_fromSurface(env, surface);
            viewer->onSurfaceCreate(window, width, height);
        }
    }
}

JNIEXPORT void onSurfaceDestroy(JNIEnv* env, jobject obj, jlong context){
    LOGI("onSurfaceDestroy");
    if(context != -1){
        PictureViewer* viewer = reinterpret_cast<PictureViewer *>(context);
        if(viewer != NULL){
            viewer->onSurfaceDestroy();
            delete viewer;
        }
    }
}



JNIEXPORT jlong nativeSetup(JNIEnv* env, jobject obj){
    LOGI("native setup");
    PictureViewer* viewer = new PictureViewer();
    return reinterpret_cast<jlong>(viewer);
}


JNIEXPORT void showPicture(JNIEnv* env, jobject obj, jlong context, jstring imagePath){
    LOGI("show Picture");
    if(context != -1){
        PictureViewer* viewer = reinterpret_cast<PictureViewer *>(context);
        if(viewer != NULL){
            char* url = const_cast<char *>(env->GetStringUTFChars(imagePath, NULL));
            viewer->showPicture(url);
            env->ReleaseStringUTFChars(imagePath, url);
        }
    }
}



static JNINativeMethod getMethods[] = {
        {"onSurfaceCreate","(JLandroid/view/Surface;II)V",(void*)onSurfaceCreate},
        {"onSurfaceDestroy","(J)V",(void*)onSurfaceDestroy},
        {"nativeSetup","()J",(void*)nativeSetup},
        {"showPicture","(JLjava/lang/String;)V",(void*)showPicture}
};

//此函数通过调用RegisterNatives方法来注册我们的函数
static int registerNativeMethods(JNIEnv* env, const char* className,JNINativeMethod* getMethods,int methodsNum){
    jclass clazz;
    //找到声明native方法的类
    clazz = env->FindClass(className);
    if(clazz == NULL){
        return JNI_FALSE;
    }
    //注册函数 参数：java类 所要注册的函数数组 注册函数的个数
    if(env->RegisterNatives(clazz,getMethods,methodsNum) < 0){
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

static int registerNatives(JNIEnv* env){
    //指定类的路径，通过FindClass 方法来找到对应的类
    const char* className  = "com/mvcoder/opengldemo/PicViewer";
    return registerNativeMethods(env,className, getMethods, sizeof(getMethods)/ sizeof(getMethods[0]));
}
//回调函数
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved){
    JNIEnv* env = NULL;
    //获取JNIEnv
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }
    assert(env != NULL);
    //注册函数 registerNatives ->registerNativeMethods ->env->RegisterNatives
    if(!registerNatives(env)){
        return -1;
    }
    //返回jni 的版本
    return JNI_VERSION_1_6;
}


void PictureViewer::showPicture(char *imageUrl) {
    int width,height,channel;
    byte* imageData = stbi_load(imageUrl, &width, &height, &channel, 0);//SOIL_load_image(imageUrl, &width, &height, 0, SOIL_LOAD_RGB);
    LOGI("width : %d, height: %d ,channel : %d", width, height, channel);
    if(renderer != NULL && renderTexSurface != NULL){
        eglCore->makeCurrent(renderTexSurface);
        GLuint textureId = createTexture(imageData, width, height);

        renderer->renderToViewWithAutoFill(textureId, screenWidth, screenHeight, width, height);
        if (!eglCore->swapBuffers(renderTexSurface)) {
            LOGE("eglSwapBuffers(renderTexSurface) returned error %d", eglGetError());
        }
        eglCore->doneCurrent();
    }
    //SOIL_free_image_data(imageData);
    stbi_image_free(imageData);
}

GLuint PictureViewer::createTexture(byte* data, int width, int height){
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // 为当前绑定的纹理对象设置环绕、过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 加载并生成纹理
    if (data){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }else
    {
        LOGE("Failed to load texture");
    }
    return texture;
}

void PictureViewer::onSurfaceCreate(ANativeWindow *window, int screenWidth, int screenHeight) {
    this->screenHeight = screenHeight;
    this->screenWidth = screenWidth;
    if (eglCore == NULL) {
        bool ret = createEGLContext();
        if(!ret) {
            LOGE("create egl context fail!!");
            return;
        }
    }
    if(surfaceWindow == NULL){
        createWindowSurface(window);
    }
}

bool PictureViewer::createEGLContext() {
    LOGI("enter VideoOutput::createEGLContext");
    eglCore = new EGLCore();
    LOGI("enter VideoOutput use sharecontext");
    bool ret = eglCore->initWithSharedContext();
    if (!ret) {
        LOGI("create EGL Context failed...");
        return false;
    }
    return ret;
}

void PictureViewer::createWindowSurface(ANativeWindow *window) {
    LOGI("enter VideoOutput::createWindowSurface");
    this->surfaceWindow = window;

    renderTexSurface = eglCore->createWindowSurface(window);
    if (renderTexSurface != NULL){
        eglCore->makeCurrent(renderTexSurface);
        // must after makeCurrent
        renderer = new VideoGLSurfaceRender();
        bool isGLViewInitialized = renderer->init(screenWidth, screenHeight);// there must be right：1080, 810 for 4:3
        if (!isGLViewInitialized) {
            LOGI("GL View failed on initialized...");
        } else {
           // surfaceExists = true;
            //forceGetFrame = true;
        }
        eglCore->doneCurrent();    // must do this before share context in Huawei p6, or will crash
    }
    LOGI("Leave VideoOutput::createWindowSurface");
}

PictureViewer::PictureViewer() {
    eglCore = NULL;
    surfaceWindow = NULL;

}

PictureViewer::~PictureViewer() {
    LOGI("~PictureViewer");
}

void PictureViewer::onSurfaceDestroy() {
    LOGI("on native surface destroy");
    if (EGL_NO_SURFACE != renderTexSurface){
        if (renderer) {
            renderer->dealloc();
            delete renderer;
            renderer = NULL;
        }

        if (eglCore){
            eglCore->releaseSurface(renderTexSurface);
        }

        renderTexSurface = EGL_NO_SURFACE;
        if(NULL != surfaceWindow){
            LOGI("VideoOutput Releasing surfaceWindow");
            ANativeWindow_release(surfaceWindow);
            surfaceWindow = NULL;
        }
    }
}
