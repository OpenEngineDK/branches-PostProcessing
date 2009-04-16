#ifndef __IRENDERBUFFER_H__
#define __IRENDERBUFFER_H__

#include <Resources/IImage.h>

#include <boost/shared_ptr.hpp>

namespace OpenEngine {
namespace Resources {

enum PixelFormat {RB_DEPTH, RB_RGB, RB_RGBA, RB_STENCIL}; // se 4.4.4 for alle mulige renderbuffer internal formats.
						          // weird bug: selvom der står GL_STENCIL_INDEX er supported for rb'er, brokker den sig

/** Interface for RenderBuffer classes
 *  @author Bjarke N. Laustsen
 */
class IRenderBuffer : public IImage {

  public:

    virtual int  GetID() = 0;

    virtual void Bind() = 0;  // skal disse bruges til noget udefra? Dvs. skal de være public?
    virtual void Unbind() = 0;

    virtual PixelFormat GetFormat() = 0;

    virtual unsigned int GetWidth() = 0;
    virtual unsigned int GetHeight() = 0;
    virtual int GetZDepth() = 0; // <-. to make it work with ITextureResource, we can't call it getDepth :(
    virtual unsigned int GetDepth() = 0; // <- this is NOT the image-depth, but instead the bit-depth of each pixel. :( (to make OE happy)

    // lav også en resize der kopiere billedet med.
    /* destructing resizes - on resize image content will be destructed and has to be reloaded */
    virtual void Resize(int width, int height) = 0; // gldelete og lav igen, men genbrug texID. (is a slow operation)
    virtual void Resize(int width, int height, PixelFormat format) = 0; // <- lav setFormat istedet for...

    virtual unsigned char* GetData() = 0;
    virtual float* GetFloatData() = 0;
    virtual void SetData(unsigned char* data) = 0;
    virtual void SetFloatData(float* data) = 0;

    virtual ImageType GetImageType() = 0;         // texture2D, renderbuffer, ...
};

/**
 * IImage resource smart pointer.
 */
typedef boost::shared_ptr<IRenderBuffer> IRenderBufferPtr;

} // NS Resources
} // NS OpenEngine

#endif
