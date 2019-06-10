#include "egl_share_context.h"

#define LOG_TAG "EglShareContext"
EglShareContext* EglShareContext::instance_ = new EglShareContext();

EglShareContext::EglShareContext() {
	init();
}

void EglShareContext::init() {
	//EGL的目的就是为了搭建 opengl 的输出到屏幕的桥梁和上下文。
	LOGI("Create EGLContext EGLCore::init");
	sharedContext = EGL_NO_CONTEXT;
	EGLint numConfigs;
	sharedDisplay = EGL_NO_DISPLAY;
	EGLConfig config;
	const EGLint attribs[] = { EGL_BUFFER_SIZE, 32, EGL_ALPHA_SIZE, 8,
			EGL_BLUE_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_RED_SIZE, 8,
			EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT, EGL_SURFACE_TYPE,
			EGL_WINDOW_BIT, EGL_NONE };

	//获取绘制的目标屏幕
	if ((sharedDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY))
			== EGL_NO_DISPLAY) {
		LOGE("eglGetDisplay() returned error %d", eglGetError());
		return;
	}

	//初始化 egl，可以从 第二个参数获取到 EGL 的主版本号、 第三个参数可以获取到此版本号，不关系的话，填0
	if (!eglInitialize(sharedDisplay, 0, 0)) {
		LOGE("eglInitialize() returned error %d", eglGetError());
		return;
	}

	//接下来就需要准备配置选项了，一旦EGL有了Display之后，它就可
	//以将OpenGL ES的输出和设备的屏幕桥接起来，但是需要指定一些配置
	//项，类似于色彩格式、像素格式、RGBA的表示以及SurfaceType等
	if (!eglChooseConfig(sharedDisplay, attribs, &config, 1, &numConfigs)) {
		LOGE("eglChooseConfig() returned error %d", eglGetError());
		return;
	}

	EGLint eglContextAttributes[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
	//创建上下文
	if (!(sharedContext = eglCreateContext(sharedDisplay, config,
			NULL == sharedContext ? EGL_NO_CONTEXT : sharedContext,
			eglContextAttributes))) {
		LOGE("eglCreateContext() returned error %d", eglGetError());
		return;
	}
}

