#ifndef __FRAMEBUFFEROBJECT_H__
#define __FRAMEBUFFEROBJECT_H__

#include <Resources/IFramebufferObject.h>
#include <Resources/IRenderBuffer.h>
#include <Resources/ITexture2D.h>
//#include "RenderBuffer.h"
//#include "Texture2D.h"
#include <Resources/PPEResourceException.h>
#include <Logging/Logger.h>

#include <Meta/OpenGL.h>

namespace OpenEngine {
namespace Resources {

/** Objects of this class represents a Framebuffer object
 *  @author Bjarke N. Laustsen
 */
class FramebufferObject : public IFramebufferObject {

  private:

    GLuint fboID;

    // max number of color-attachments this gfx-card allows
    GLint maxNumColorAttachments;

    // attached renderbuffers/textures (remembered, so they can be returned through the get functions)
    IImagePtr colorAttachments[16];
    IImagePtr depthAttachment;
    IImagePtr stencilAttachment;

    // helper array - contains all the enums for the color attachment points
    static const GLenum colorAttachmentEnums[];

    GLenum GetAttachmentType(GLenum attachmentEnum);
    GLuint GetAttachmentID(GLenum attachmentEnum);

    /* Since glPushAttrib/glPopAttrib doesn't work for FBOs and
    since the only setting we wish to preserve unchanged across methodcalls
    is the currently bound FBO (the methodcalls shouldnt have the sideeffect of changing
    this value of the OpenGL state), we use this way instead (See spec item (81))*/
    GLint savedFboID;
    void GuardedBind();  // to avoid unwanted side effects
    void GuardedUnbind();// to avoid unwanted side effects

  public:

    FramebufferObject();
    ~FramebufferObject();

    void Bind();
    void Unbind(); // note: no need to unbind between bind calls!

    // todo: use IImagePtr as argument to the attach-methods instead, to only have 3 functions.
    void AttachColorRenderBuffer  (IRenderBufferPtr rb, int attachmentPoint);
    void AttachDepthRenderBuffer  (IRenderBufferPtr rb);
    void AttachStencilRenderBuffer(IRenderBufferPtr rb);

    void AttachColorTexture(ITexture2DPtr tex, int attachmentPoint);
    void AttachDepthTexture(ITexture2DPtr tex);
    void AttachStencilTexture(ITexture2DPtr tex);

    void DetachColorAttachment  (int attachmentPoint); // ok bare at brug rb'er? Eller skal man detache for den resource type der er bundet?
    void DetachDepthAttachment  ();
    void DetachStencilAttachment();

    /* must be called after all color attachments you want to render to are attached, and before drawing, to select which color bufs to render to */
    /* the selected buffers will be remembered through bind, undbind (etc) calls, and applies to this FBO only! */
    void SelectDrawBuffers(); // <- sets default (=all attached buffers are targets in order (incl. GL_NONE) (buf0=att0, buf1=att1, ...) - so COLOR0 in the shader corresponds to ATT0, etc)
    void SelectDrawBuffers(int attachmentPoint); // <- sets one color buffer as the target (evt. også hav version for array)

    int GetMaxNumColorAttachments();

    int GetID();

    // check framebuffer errors
    bool CheckFrameBufferStatus();

    // get objects currently attached to this FBO
    IImagePtr GetColorAttachment(int attachmentPoint); // smart-pointer NULL if none
    IImagePtr GetDepthAttachment(); // smart-pointer NULL if none
    IImagePtr GetStencilAttachment(); // smart-pointer NULL if none
};

} // NS Resources
} // NS OpenEngine

#endif


