#ifndef __ITEXTURE2D_H__
#define __ITEXTURE2D_H__

#include <Resources/ITexture.h>
#include <boost/shared_ptr.hpp>

namespace OpenEngine {
namespace Resources {

class ITexture2D;

/**
 * ITexture2D resource smart pointer.
 */
typedef boost::shared_ptr<ITexture2D> ITexture2DPtr;

/**
 * Interface for the Texture2D class
 * @author Bjarke N. Laustsen
 */
class ITexture2D : public ITexture {


  public:

    virtual ITexture2DPtr Clone() = 0; // @todo: evt. option til at få width, size OG FORMAT med - det kan gøres uden at ændre copy koden!
    virtual void Clone(ITexture2DPtr texture) = 0; // genbruger texID fra texture (skal man gldelete også?)

    virtual void Bind() = 0;
    virtual void Unbind() = 0; // note: no need to unbind between bind calls!

    virtual int  GetID() = 0;
    virtual void SetID(int texID) = 0; // to make OE happy

    virtual void SetWrapS    (TextureWrap wrap) = 0;
    virtual void SetWrapT    (TextureWrap wrap) = 0;
    virtual void SetMagFilter(TextureFilter filter) = 0;
    virtual void SetMinFilter(TextureFilter filter) = 0;
    virtual TextureWrap   GetWrapS() = 0;
    virtual TextureWrap   GetWrapT() = 0;
    virtual TextureFilter GetMagFilter() = 0;
    virtual TextureFilter GetMinFilter() = 0;

    virtual TexelFormat GetFormat() = 0;

    virtual unsigned int GetWidth() = 0;
    virtual unsigned int GetHeight() = 0;
    virtual int GetZDepth() = 0; // <-. to make it work with ITextureResource, we can't call it getDepth :(
    virtual unsigned int GetDepth() = 0; // <- this is NOT the image-depth, but instead the bit-depth of each pixel. :( (to make OE happy)

    virtual int GetNumComponents() = 0;

    // lav også en resize der kopiere billedet med. (men der skal også være een der ikke gør, for performance)
    /* destructing resizes - on resize image content will be destructed and has to be reloaded */
    virtual void Resize(int width, int height) = 0; // gldelete og lav igen, men genbrug texID. (is a slow operation)
    virtual void Resize(int width, int height, TexelFormat format) = 0; // <- lav setFormat istedet for...? Eller... langsommere at gøre 2 gange!

    virtual unsigned char* GetData() = 0;
    virtual float* GetFloatData() = 0;
    virtual void SetData(unsigned char* data) = 0;
    virtual void SetFloatData(float* data) = 0;

    virtual ImageType GetImageType() = 0;         // texture2D, renderbuffer, ...
    //FBOAttachmentBufferType getAttachmentBufferType();   // color, depth, stencil
};

} // NS Resources
} // NS OpenEngine

#endif

