#ifndef __TEXTURE2D_H__
#define __TEXTURE2D_H__

#include <Resources/ITexture2D.h>
#include <Resources/PPEResourceException.h>
#include <Logging/Logger.h>

#include <Meta/OpenGL.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>


namespace OpenEngine {
namespace Resources {

/** Represents a 2D texture which can be created dynamically
 *  @note assumes OpenGL2.0 as it doesn't check for power of 2 texture sizes. Also required the FBO extension for some operations.
 *  @author Bjarke N. Laustsen
 */
class Texture2D : public ITexture2D {

  private:

    GLuint texID;

    GLint   GetGLInternalFormat(TexelFormat format);
    GLenum  GetGLFormat(TexelFormat format);
    GLint   GetGLWrap(TextureWrap wrap);
    GLint   GetGLFilter(TextureFilter filter);
    GLsizei GetGLWidth(int width);
    GLsizei GetGLHeight(int height);

    TexelFormat   GetOEInternalFormat(GLint glInternalFormat);
    TextureWrap   GetOEWrap(GLint glWrap);
    TextureFilter GetOEFilter(GLint glFilter);

    void CreateOrModifyTexture(int width, int height, TexelFormat format, TextureWrap wrapS, TextureWrap wrapT, TextureFilter filterMag, TextureFilter filterMin);
    void CopyTexture(ITexture2DPtr destTexture);
    unsigned char* GetData(GLenum type);
    void CheckGLErrors (const char *label);

    Texture2D() {}

  public:

    Texture2D(int width, int height, TexelFormat format, TextureWrap wrapS, TextureWrap wrapT, TextureFilter filterMag, TextureFilter filterMin);
    ~Texture2D(); // <- note: relatively slow operation

    ITexture2DPtr Clone();
    void Clone(ITexture2DPtr texture);

    void Bind();
    void Unbind(); // note: no need to unbind between bind calls!

    int  GetID();
    void SetID(int texID); // to make OE happy

    void SetWrapS    (TextureWrap wrap);
    void SetWrapT    (TextureWrap wrap);
    void SetMagFilter(TextureFilter filter);
    void SetMinFilter(TextureFilter filter);
    TextureWrap   GetWrapS();
    TextureWrap   GetWrapT();
    TextureFilter GetMagFilter();
    TextureFilter GetMinFilter();

    TexelFormat GetFormat();

    unsigned int GetWidth();
    unsigned int GetHeight();
    int GetZDepth(); // <-. to make it work with ITextureResource, we can't call it getDepth :(
    unsigned int GetDepth(); // <- not z-depth, but bit-depth
    ColorFormat GetColorFormat();

    int GetNumComponents();

    void Resize(int width, int height);
    void Resize(int width, int height, TexelFormat format);

    unsigned char* GetData();
    float* GetFloatData();
    void SetData(unsigned char* data);
    void SetFloatData(float* data);

    ImageType GetImageType();         // texture2D, renderbuffer, ...

    /* to make it work with textureresource */
    void Load();
    void Unload();
};

} // NS Resources
} // NS OpenEngine

#endif

// todo: lav en SetBorderColor-metode
// todo: allow mipmapping
