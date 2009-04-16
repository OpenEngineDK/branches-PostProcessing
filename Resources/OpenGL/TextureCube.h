#ifndef __TEXTURECUBE_H__
#define __TEXTURECUBE_H__

#include <Resources/ITextureCube.h>
#include <Resources/PPEResourceException.h>
#include <Logging/Logger.h>

#include <Meta/OpenGL.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>


namespace OpenEngine {
namespace Resources {

/** Represents a Cube texture which can be created dynamically
 *  @note assumes OpenGL2.0 as it doesn't check for power of 2 texture sizes. Also required the FBO extension for some operations.
 *  @note assumes that each of the 6 faces have same internal format and size for making it easier *FOR US* to code it!
 *        According to http://www.opengl.org/registry/specs/ARB/texture_cube_map.txt they must have same width/height (and int format?)
 *  @note according http://developer.nvidia.com/object/cube_map_ogl_tutorial.html all cube faces must be square
 *  @author Bjarke N. Laustsen
 */
class TextureCube : public ITextureCube {

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

    void CreateOrModifyTexture(int width, int height, TexelFormat format, TextureWrap wrapS, TextureWrap wrapT, TextureWrap wrapR, TextureFilter filterMag, TextureFilter filterMin);
    void CopyTexture(ITextureCubePtr destTexture);
    unsigned char* GetData(int face, GLenum type);
    void CheckGLErrors (const char *label);

    TextureCube() {}

  public:

    TextureCube(int width, int height, TexelFormat format, TextureWrap wrapS, TextureWrap wrapT, TextureWrap wrapR, TextureFilter filterMag, TextureFilter filterMin);
    ~TextureCube(); // <- note: relatively slow operation

    ITextureCubePtr Clone();
    void Clone(ITextureCubePtr texture);

    void Bind();
    void Unbind(); // note: no need to unbind between bind calls!

    int  GetID();
    void SetID(int texID); // to make OE happy

    void SetWrapS    (TextureWrap wrap);
    void SetWrapT    (TextureWrap wrap);
    void SetWrapR    (TextureWrap wrap);
    void SetMagFilter(TextureFilter filter);
    void SetMinFilter(TextureFilter filter);
    TextureWrap   GetWrapS();
    TextureWrap   GetWrapT();
    TextureWrap   GetWrapR();
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

    unsigned char* GetData(int face);
    float* GetFloatData(int face);
    void SetData(int face, unsigned char* data);
    void SetFloatData(int face, float* data);

    unsigned char* GetData()          {throw new PPEResourceException("makes no sense here without face the parameter! Reconsider the interface!");}
    float* GetFloatData()             {throw new PPEResourceException("makes no sense here without face the parameter! Reconsider the interface!");}
    void SetData(unsigned char* data) {throw new PPEResourceException("makes no sense here without face the parameter! Reconsider the interface!");}
    void SetFloatData(float* data)    {throw new PPEResourceException("makes no sense here without face the parameter! Reconsider the interface!");}

    ImageType GetImageType();         // TextureCube, renderbuffer, ...

    /* to make it work with textureresource */
    void Load();
    void Unload();
};

} // NS Resources
} // NS OpenEngine

#endif

