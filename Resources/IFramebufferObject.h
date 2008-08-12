#ifndef __IFRAMEBUFFEROBJECT_H__
#define __IFRAMEBUFFEROBJECT_H__

#include "IRenderBuffer.h"
#include "ITexture2D.h"
#include "IImage.h"
#include <Resources/PPEResourceException.h>

namespace OpenEngine {
namespace Resources {

/** Interface for Framebuffer Object classes
 *  @author Bjarke N. Laustsen
 */
class IFramebufferObject {

  public:

    virtual ~IFramebufferObject() {}

    virtual void Bind() = 0;
    virtual void Unbind() = 0; // note: no need to unbind between bind calls!

    // todo: use IImagePtr as argument to the attach-methods instead, to only have 3 functions.
    virtual void AttachColorRenderBuffer  (IRenderBufferPtr rb, int attachmentPoint) = 0;
    virtual void AttachDepthRenderBuffer  (IRenderBufferPtr rb) = 0;
    virtual void AttachStencilRenderBuffer(IRenderBufferPtr rb) = 0;

    virtual void AttachColorTexture(ITexture2DPtr tex, int attachmentPoint) = 0;
    virtual void AttachDepthTexture(ITexture2DPtr tex) = 0;
    virtual void AttachStencilTexture(ITexture2DPtr tex) = 0;

    virtual void DetachColorAttachment  (int attachmentPoint) = 0; // ok bare at brug rb'er? Eller skal man detache for den resource type der er bundet?
    virtual void DetachDepthAttachment  () = 0;
    virtual void DetachStencilAttachment() = 0;

    /* must be called after all color attachments you want to render to are attached, and before drawing, to select which color bufs to render to */
    /* the selected buffers will be remembered through bind, undbind (etc) calls, and applies to this FBO only! */
    virtual void SelectDrawBuffers() = 0; // <- sets default (=all attached buffers are targets in order (incl. GL_NONE) (buf0=att0, buf1=att1, ...) - so COLOR0 in the shader corresponds to ATT0, etc)
    virtual void SelectDrawBuffers(int attachmentPoint) = 0; // <- sets one color buffer as the target (evt. også hav version for array)

    virtual int GetMaxNumColorAttachments() = 0;

    virtual int GetID() = 0;

    // check framebuffer errors
    virtual bool CheckFrameBufferStatus() = 0;

    // get objects currently attached to this FBO
    virtual IImagePtr GetColorAttachment(int attachmentPoint) = 0; // smart-pointer NULL if none
    virtual IImagePtr GetDepthAttachment() = 0; // smart-pointer NULL if none
    virtual IImagePtr GetStencilAttachment() = 0; // smart-pointer NULL if none
};

} // NS Resources
} // NS OpenEngine

#endif


