#include "video_gl_water_surface_render.h"

#define LOG_TAG "VideoGLSurfaceRender"

VideoGLWaterSurfaceRender::VideoGLWaterSurfaceRender() {
	mVertexShader = OUTPUT_VIEW_VERTEX_SHADER1;
	mFragmentShader = OUTPUT_VIEW_FRAG_SHADER1;
}

VideoGLWaterSurfaceRender::~VideoGLWaterSurfaceRender() {
}

bool VideoGLWaterSurfaceRender::init(int width, int height) {
	this->_backingLeft = 0;
	this->_backingTop = 0;
	this->_backingWidth = width;
	this->_backingHeight = height;
	mGLProgId = loadProgram(mVertexShader, mFragmentShader);
	if (!mGLProgId) {
		LOGE("Could not create program.");
		return false;
	}
	mGLVertexCoords = glGetAttribLocation(mGLProgId, "position");
	checkGlError("glGetAttribLocation vPosition");
	mGLTextureCoords = glGetAttribLocation(mGLProgId, "texcoord");
	checkGlError("glGetAttribLocation vTexCords");
	mGLUniformTexture = glGetUniformLocation(mGLProgId, "yuvTexSampler");
	checkGlError("glGetAttribLocation yuvTexSampler");
    mWaterUniformTexture = glGetUniformLocation(mGLProgId, "waterTexSampler");
    checkGlError("glGetAttribLocation yuvTexSampler");

	mIsInitialized = true;
	return true;
}

void VideoGLWaterSurfaceRender::renderToViewWithAutoFill(GLuint texID, GLuint waterTexId,int screenWidth, int screenHeight, int texWidth, int texHeight) {
	glViewport(0, 0, screenWidth, screenHeight);

	if (!mIsInitialized) {
		LOGE("ViewRenderEffect::renderEffect effect not initialized!");
		return;
	}

	float textureAspectRatio = (float)texHeight / (float)texWidth;
	float viewAspectRatio = (float)screenHeight / (float)screenWidth;
	float xOffset = 0.0f;
	float yOffset = 0.0f;
	/**
	 * 这里的算法感觉有问题
	 *
	 * 下面是自己的理解算法：维持视频的比例算法
	 * 当 textureAspectRatio > viewAspectRatio 时，说明图片的高/宽 > view的高/宽，
	 * 也就是说：如果宽相等的话，图片的高将会超出界面。所以为了能够正确显示，应该假设高==view的高，将宽进行等比缩放，视频的比例将不会发生变化。
	 * 这种假设是基于图片的高> view的高的，如果view的高大于图片的高呢？将会放大，会失真。那么仍需要处理一下这种情况，将图片置于view的中心。
	 *
	 * 当textureAspectRation < viewAspectRatio时，说明图片的高/宽 < view的高/宽，
	 * 也就说，如果高相等，图片的宽将会 > view的宽，所以为了能够按照比例正常显示，应该假设宽==view的宽，高度进行等比缩放。视频的比例将不会发生变化
	 * 这种假设是基于图片的宽>view的宽，否则将会宽度变宽，从而失真。那么就需要将图片放置于view中心了
	 *
	 */

	if(textureAspectRatio > viewAspectRatio){
		//Update Y Offset
		int expectedHeight = (int)((float)texHeight*screenWidth/(float)texWidth+0.5f);
		yOffset = (float)(expectedHeight-screenHeight)/(2*expectedHeight);
//		LOGI("yOffset is %.3f", yOffset);
	} else if(textureAspectRatio < viewAspectRatio){
		//Update X Offset
		int expectedWidth = (int)((float)(texHeight * screenWidth) / (float)screenHeight + 0.5);
		xOffset = (float)(texWidth - expectedWidth)/(2*texWidth);
//		LOGI("xOffset is %.3f", xOffset);
	}

	glUseProgram(mGLProgId);
    //位置分别是：左下、右下、左上、右上
	static const GLfloat _vertices[] = { -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f };
    //绑定顶点数据到指定顶点属性：第一个参数是顶点属性在顶点shader的位置，然后分别是顶点的大小，类型，是否经常变化(涉及到选择何种缓冲区),每个顶点之间的偏移量（0代表紧密相连),
    //顶点的数据
	glVertexAttribPointer(mGLVertexCoords, 2, GL_FLOAT, 0, 0, _vertices);
	glEnableVertexAttribArray(mGLVertexCoords);
    static const GLfloat texCoords[] = { xOffset, (GLfloat)(1.0 - yOffset), (GLfloat)(1.0 - xOffset), (GLfloat)(1.0 - yOffset), xOffset, yOffset,
                                         (GLfloat)(1.0 - xOffset), yOffset };
	glVertexAttribPointer(mGLTextureCoords, 2, GL_FLOAT, 0, 0, texCoords);
    //启用指定顶点属性
	glEnableVertexAttribArray(mGLTextureCoords);

	/* Binding the input texture */
	glActiveTexture (GL_TEXTURE0);
    //激活纹理0
	glBindTexture(GL_TEXTURE_2D, texID);
    //将uniform的属性定位到位置0，本身是 纹理属性，所以就是对应上面的 GL_TEXTURE0
	glUniform1i(mGLUniformTexture, 0);

    /* Binding the input texture */
    glActiveTexture (GL_TEXTURE1);
    //激活纹理0
    glBindTexture(GL_TEXTURE_2D, waterTexId);
    //将uniform的属性定位到位置1，本身是 纹理属性，所以就是对应上面的 GL_TEXTURE1
    glUniform1i(mWaterUniformTexture, 1);


    //绘制、一张图片是矩形，所以画4个点
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	//关闭顶点、纹理属性和解绑纹理0 -- 因为OpenGL上下文就是一个巨大的状态机，这里设置了状态、用完就要清空
	glDisableVertexAttribArray(mGLVertexCoords);
	glDisableVertexAttribArray(mGLTextureCoords);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void VideoGLWaterSurfaceRender::checkGlError(const char* op){
	for (GLint error = glGetError(); error; error = glGetError()) {
		LOGE("after %s() glError (0x%x)\n", op, error);
	}
}

void VideoGLWaterSurfaceRender::dealloc() {
	mIsInitialized = false;
	glDeleteProgram(mGLProgId);
}

void VideoGLWaterSurfaceRender::resetRenderSize(int left, int top, int width, int height){
	this->_backingLeft = left;
	this->_backingTop = top;
	this->_backingWidth = width;
	this->_backingHeight = height;
}

GLuint VideoGLWaterSurfaceRender::loadProgram(char* pVertexSource, char* pFragmentSource) {
	GLuint vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSource);
	if (!vertexShader) {
		return 0;
	}
	GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSource);
	if (!pixelShader) {
		return 0;
	}
	GLuint program = glCreateProgram();
	if (program) {
		glAttachShader(program, vertexShader);
		checkGlError("glAttachShader");
		glAttachShader(program, pixelShader);
		checkGlError("glAttachShader");
		glLinkProgram(program);
		GLint linkStatus = GL_FALSE;
		glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
		if (linkStatus != GL_TRUE) {
			GLint bufLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
			if (bufLength) {
				char* buf = (char*) malloc(bufLength);
				if (buf) {
					glGetProgramInfoLog(program, bufLength, NULL, buf);
					LOGI("Could not link program:\n%s\n", buf);
					free(buf);
				}
			}
			glDeleteProgram(program);
			program = 0;
		}
	}
	return program;
}

GLuint VideoGLWaterSurfaceRender::loadShader(GLenum shaderType, const char* pSource) {
	GLuint shader = glCreateShader(shaderType);
	if (shader) {
		glShaderSource(shader, 1, &pSource, NULL);
		glCompileShader(shader);
		GLint compiled = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		if (!compiled) {
			GLint infoLen = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
			if (infoLen) {
				char* buf = (char*) malloc(infoLen);
				if (buf) {
					glGetShaderInfoLog(shader, infoLen, NULL, buf);
					LOGI("Could not compile shader %d:\n%s\n", shaderType, buf);
					free(buf);
				}
			} else {
				LOGI( "Guessing at GL_INFO_LOG_LENGTH size\n");
				char* buf = (char*) malloc(0x1000);
				if (buf) {
					glGetShaderInfoLog(shader, 0x1000, NULL, buf);
					LOGI("Could not compile shader %d:\n%s\n", shaderType, buf);
					free(buf);
				}
			}
			glDeleteShader(shader);
			shader = 0;
		}
	}
	return shader;
}
