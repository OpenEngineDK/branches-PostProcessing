#ifndef __ITEXTURE_H__
#define __ITEXTURE_H__

#include <Resources/IImage.h>
#include <Resources/ITextureResource.h>

/* husk at 1D, 3D, RECT og CUBE textures også kan attaches som FBO-attachments! */


/* vigtigt den nedarver fra ITextureResource, så man kan binde normale textures tilet pass. Hvad med cube, etc? */

namespace OpenEngine {
namespace Resources {

// bliver nødt til at lave prefix for at undgå ambiguity (se: http://www.informit.com/guides/content.aspx?g=cplusplus&seqNum=289&rl=1 )
enum TexelFormat   {TEX_DEPTH, TEX_DEPTH_STENCIL, TEX_LUMINANCE, TEX_RGB, TEX_RGBA, TEX_LUMINANCE_FLOAT, TEX_RGB_FLOAT, TEX_RGBA_FLOAT};
enum TextureWrap   {TEX_CLAMP, TEX_CLAMP_TO_EDGE, TEX_CLAMP_TO_BORDER, TEX_REPEAT, TEX_MIRRORED_REPEAT};
enum TextureFilter {TEX_NEAREST, TEX_LINEAR};

/** Interface for all texture classes (1D, 2D, 3D, CUBE, RECT, ...)
 * @author Bjarke N. Laustsen
 */
class ITexture : public IImage, public ITextureResource {

};

/**
 * ITexture resource smart pointer.
 */
typedef boost::shared_ptr<ITexture> ITexturePtr;

} // NS Resources
} // NS OpenEngine

#endif
