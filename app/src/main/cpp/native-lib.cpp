#include <jni.h>
#include <string>
#include <android/log.h>
#include "SOIL.h"

#define LOG_TAG "native-lib"

#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

extern "C" {
JNIEXPORT jstring JNICALL Java_com_mvcoder_opengldemo_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

JNIEXPORT void JNICALL Java_com_mvcoder_opengldemo_MainActivity_converJpgToRGBTexture(
        JNIEnv *env,
        jobject obj,
        jstring path) {
    char *imagePath = const_cast<char *>(env->GetStringUTFChars(path, NULL));
    int width, height;
    unsigned char *image = SOIL_load_image(imagePath, &width, &height, 0, SOIL_LOAD_RGB);
    LOGI("width : %d:height : %d", width, height);
    SOIL_free_image_data(image);
    env->ReleaseStringUTFChars(path, imagePath);

}
}

