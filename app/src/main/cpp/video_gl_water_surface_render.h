#ifndef VIDEO_PLAYER_GL_WATER_SURFACE_RENDER_H
#define VIDEO_PLAYER_GL_WATER_SURFACE_RENDER_H

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include "CommonTools.h"

static char* OUTPUT_VIEW_VERTEX_SHADER1 =
		"attribute vec4 position;    \n"
		"attribute vec2 texcoord;   \n"
		"varying vec2 v_texcoord;     \n"
		"void main(void)               \n"
		"{                            \n"
		"   gl_Position = position;  \n"
		"   v_texcoord = texcoord;  \n"
		"}                            \n";

static char* OUTPUT_VIEW_FRAG_SHADER1 =
	    "varying highp vec2 v_texcoord;\n"
	    "uniform sampler2D yuvTexSampler;\n"
        "uniform sampler2D waterTexSampler;\n"
		"void main() {\n"
        "  vec4 imageColor = texture2D(yuvTexSampler,v_texcoord);\n"
        "  vec4 waterColor = texture2D(waterTexSampler,v_texcoord);\n"
        "  float r = waterColor.r + imageColor.r;\n"
        "  float g = waterColor.g + imageColor.g;\n"
        "  float b = waterColor.b + imageColor.b;\n"
		"  gl_FragColor = vec4(r,g,b,1.0);\n"
		"}\n";

/**
 * Video OpenGL View
 */
class VideoGLWaterSurfaceRender {
public:
    VideoGLWaterSurfaceRender();
	virtual ~VideoGLWaterSurfaceRender();
    void resetRenderSize(int left, int top, int width, int height);
	bool init(int width, int height);
	void renderToViewWithAutoFill(GLuint texID,GLuint waterTexID, int screenWidth, int screenHeight, int texWidth, int texHeight);
	void dealloc();

protected:
	GLint _backingLeft;
	GLint _backingTop;
	GLint _backingWidth;
	GLint _backingHeight;

    char* mVertexShader;
    char* mFragmentShader;

    bool mIsInitialized;

    GLuint mGLProgId;
    GLuint mGLVertexCoords;
    GLuint mGLTextureCoords;
    GLint mGLUniformTexture;
    GLint mWaterUniformTexture;
    void checkGlError(const char* op);
    GLuint loadProgram(char* pVertexSource, char* pFragmentSource);
    GLuint loadShader(GLenum shaderType, const char* pSource);

    float calcCropRatio(int screenWidth, int screenHeight, int texWidth, int texHeight);
};

#endif //VIDEO_PLAYER_GL_SURFACE_RENDER_H
