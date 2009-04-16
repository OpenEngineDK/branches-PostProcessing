#ifndef __IIMAGE_H__
#define __IIMAGE_H__

#include <boost/shared_ptr.hpp>

namespace OpenEngine {
namespace Resources {

enum ImageType {IMG_TEXTURE_2D, IMG_TEXTURE_CUBE, IMG_RENDERBUFFER}; /* IMG_TEXTURE_1D, IMG_TEXTURE_3D, IMG_TEXTURE_CUBE, IMG_TEXTURE_RECT */

/** The image concept represents a picture - i.e. the various texture types and renderbuffers.
 *  The reason for this interface is so that textures and renderbuffers can be treated the same
 *  @author Bjarke N. Laustsen
 */
class IImage {

  public:

    virtual ~IImage() {}

    // returns the type of this object (so you know what you should cast to, if you get a pointer to IFBOAttachment. Is there a better way?)
    virtual ImageType GetImageType() = 0;         // texture2D, renderbuffer, ...

    virtual void Bind() = 0;
    virtual void Unbind() = 0;

    virtual int GetID() = 0; // <- note different image types can share the same ID. So an ID is not unique over all image types.

    virtual unsigned int GetWidth() = 0;
    virtual unsigned int GetHeight() = 0;
    virtual int GetZDepth() = 0; // <-. to make it work with ITextureResource, we can't call it getDepth
    virtual unsigned int GetDepth() = 0;  // <- This is NOT the image-depth, but instead the bit-depth of each pixel.
    			 	 //    Will give problems when adding 3D-textures, but this is to keep compatible with ITextureResource.
    virtual unsigned char* GetData() = 0;
    virtual float* GetFloatData() = 0;
    virtual void SetData(unsigned char* data) = 0;
    virtual void SetFloatData(float* data) = 0;

};

/**
 * IImage resource smart pointer.
 */
typedef boost::shared_ptr<IImage> IImagePtr;

} // NS Resources
} // NS OpenEngine

#endif
