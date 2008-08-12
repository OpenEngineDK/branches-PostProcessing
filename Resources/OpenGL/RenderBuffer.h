#ifndef __RENDERBUFFER_H__
#define __RENDERBUFFER_H__

#include <Resources/IRenderBuffer.h>
#include <Resources/PPEResourceException.h>
#include <Logging/Logger.h>
#include <Core/Exceptions.h>

#include <Meta/OpenGL.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

namespace OpenEngine {
namespace Resources {

using namespace OpenEngine::Core;

/** Represents a 2D rendebuffer which can be created dynamically
 *  @author Bjarke N. Laustsen
 */
class RenderBuffer : public IRenderBuffer {

  private:

    GLuint rbID;

    void CreateOrModifyRB(int width, int height, PixelFormat format);
    void CheckGLErrors (const char *label);

    GLint   GetGLInternalFormat(PixelFormat format);
    GLsizei GetGLWidth(int width);
    GLsizei GetGLHeight(int height);
    PixelFormat GetOEInternalFormat(GLint glInternalFormat);

    GLint savedRbID;
    void GuardedBind();
    void GuardedUnbind();

  public:

    RenderBuffer(int width, int height, PixelFormat format);
    ~RenderBuffer(); // <- note: relatively slow operation

    int  GetID();

    void Bind();
    void Unbind(); // note: no need to unbind between bind calls!

    PixelFormat GetFormat();

    int GetWidth();
    int GetHeight();
    int GetZDepth(); // <-. to make it work with ITextureResource, we can't call it getDepth :(
    int GetDepth(); // <- not z-depth, but bit-depth

    void Resize(int width, int height);
    void Resize(int width, int height, PixelFormat format);

    unsigned char* GetData();
    float* GetFloatData();
    void SetData(unsigned char* data);
    void SetFloatData(float* data);

    ImageType GetImageType();         // texture2D, renderbuffer, ...
};

} // NS Resources
} // NS OpenEngine

#endif

