#include "FramebufferObject.h"

/* @author Bjarke N. Laustsen
 */
namespace OpenEngine {
namespace Resources {

const GLenum FramebufferObject::colorAttachmentEnums[] = {GL_COLOR_ATTACHMENT0_EXT , GL_COLOR_ATTACHMENT1_EXT , GL_COLOR_ATTACHMENT2_EXT,
					                  GL_COLOR_ATTACHMENT3_EXT , GL_COLOR_ATTACHMENT4_EXT , GL_COLOR_ATTACHMENT5_EXT,
					    		  GL_COLOR_ATTACHMENT6_EXT , GL_COLOR_ATTACHMENT7_EXT , GL_COLOR_ATTACHMENT8_EXT,
					    		  GL_COLOR_ATTACHMENT9_EXT , GL_COLOR_ATTACHMENT10_EXT, GL_COLOR_ATTACHMENT11_EXT,
					    		  GL_COLOR_ATTACHMENT12_EXT, GL_COLOR_ATTACHMENT13_EXT, GL_COLOR_ATTACHMENT14_EXT,
					    		  GL_COLOR_ATTACHMENT15_EXT};

/** Create a new FramebufferObject
 */
FramebufferObject::FramebufferObject() {
    this->fboID      = 0; // <- must be done before guarded bind
    this->savedFboID = 0; // <- must be done before guarded bind
    GuardedBind();

    // init fboID
    glGenFramebuffersEXT(1, &fboID);			// <- get an avaliable handle
    Bind();						// <- create and bind
    Unbind();						// <- unbind again

    // init maxNumColorAttachments
    glGetIntegerv(GL_MAX_DRAW_BUFFERS, &maxNumColorAttachments);
    if (maxNumColorAttachments > 16) throw PPEResourceException("specs have changed");

    // init list of attached objects
    for (int i=0; i<16; i++) colorAttachments[i].reset();
    depthAttachment.reset();
    stencilAttachment.reset();

    GuardedUnbind();
}

/** delete this framebuffer object
 */
FramebufferObject::~FramebufferObject() {
    glDeleteFramebuffersEXT(1, &fboID);
}

/** bind this framebuffer object
 */
void FramebufferObject::Bind() {
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboID);
}

/** unbind any framebuffer object (not only this one)
 *  @note: no need to call between bind-calls!
 */
void FramebufferObject::Unbind() {
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

/** Attach renderbuffer with color-format as a color attachment at the supplied attachment point
 * @param rb the renderbuffer to attach
 * @param attachmentPoint the attachment point to attach the renderbuffer
 * @exception PPEResourceException thrown if illegal attachmentPoint (less than 0 or grater than FramebufferObject::GetMaxNumColorAttachments)
 * @exception PPEResourceException thrown if the supplied renderbuffer doesn't doesn't have a color internal format
 */
void FramebufferObject::AttachColorRenderBuffer(IRenderBufferPtr rb, int attachmentPoint) {
    if (attachmentPoint < 0 || attachmentPoint >= maxNumColorAttachments) throw PPEResourceException("illegal attachmentPoint");
    if (rb->GetFormat()!=RB_RGB && rb->GetFormat()!=RB_RGBA) throw PPEResourceException("non-color renderbuffers can't be attached as color attachments");

    GuardedBind();
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, colorAttachmentEnums[attachmentPoint], GL_RENDERBUFFER_EXT, rb->GetID());

    // check if it really got attached (i'm not quite sure that all FBO-errors means that it didn't get attached, so that's why i'm not simply checking for errors here)
    if (GetAttachmentType(colorAttachmentEnums[attachmentPoint]) != GL_RENDERBUFFER_EXT || GetAttachmentID(colorAttachmentEnums[attachmentPoint]) != rb->GetID())
	{GuardedUnbind(); throw PPEResourceException("attaching failed");} // maybe it should not cast an exception..
    else
        colorAttachments[attachmentPoint] = rb;

    GuardedUnbind();
}

/** Attach renderbuffer with depth-format as a depth attachment
 * @param rb the renderbuffer to attach
 * @exception PPEResourceException thrown if the supplied renderbuffer doesn't doesn't have a depth internal format
 */
void FramebufferObject::AttachDepthRenderBuffer(IRenderBufferPtr rb) {
    if (rb->GetFormat()!=RB_DEPTH) throw PPEResourceException("non-depth renderbuffers can't be attached as depth attachments");

    GuardedBind();
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, rb->GetID());

    // check if it really got attached (i'm not quite sure that all FBO-errors means that it didn't get attached, so that's why i'm not simply checking for errors here)
    if (GetAttachmentType(GL_DEPTH_ATTACHMENT_EXT) != GL_RENDERBUFFER_EXT || GetAttachmentID(GL_DEPTH_ATTACHMENT_EXT) != rb->GetID())
	{GuardedUnbind(); throw PPEResourceException("attaching failed");}// maybe it should not cast an exception..
    else
        depthAttachment = rb;

    GuardedUnbind();
}

/** Attach renderbuffer with stencil-format as a stencil attachment
 * @param rb the renderbuffer to attach
 * @exception PPEResourceException thrown if the supplied renderbuffer doesn't doesn't have a stencil internal format
 */
void FramebufferObject::AttachStencilRenderBuffer(IRenderBufferPtr rb) {
    if (rb->GetFormat()!=RB_STENCIL) throw PPEResourceException("non-stencil renderbuffers can't be attached as stencil attachments");

    GuardedBind();
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, rb->GetID());

    // check if it really got attached (i'm not quite sure that all FBO-errors means that it didn't get attached, so that's why i'm not simply checking for errors here)
    if (GetAttachmentType(GL_STENCIL_ATTACHMENT_EXT) != GL_RENDERBUFFER_EXT || GetAttachmentID(GL_STENCIL_ATTACHMENT_EXT) != rb->GetID())
	{GuardedUnbind(); throw PPEResourceException("attaching failed");}// maybe it should not cast an exception..
    else
        stencilAttachment = rb;

    GuardedUnbind();
}

/** Attach texture with color-format as a color attachment at the supplied attachment point
 * @param tex the texture to attach
 * @param attachmentPoint the attachment point to attach the texture
 * @exception PPEResourceException thrown if illegal attachmentPoint (less than 0 or grater than FramebufferObject::GetMaxNumColorAttachments)
 * @exception PPEResourceException thrown if the supplied texture doesn't doesn't have a color internal format
 */
void FramebufferObject::AttachColorTexture(ITexture2DPtr tex, int attachmentPoint) {
    if (attachmentPoint < 0 || attachmentPoint >= maxNumColorAttachments) throw PPEResourceException("illegal attachmentPoint");
    if (tex->GetFormat()!=TEX_RGB && tex->GetFormat()!=TEX_RGBA && tex->GetFormat()!=TEX_RGB_FLOAT && tex->GetFormat()!=TEX_RGBA_FLOAT)
        throw PPEResourceException("non-color textures can't be attached as color attachments");

    GuardedBind();
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, colorAttachmentEnums[attachmentPoint], GL_TEXTURE_2D, tex->GetID(), 0);

    // check if it really got attached (i'm not quite sure that all FBO-errors means that it didn't get attached, so that's why i'm not simply checking for errors here)
    if (GetAttachmentType(colorAttachmentEnums[attachmentPoint]) != GL_TEXTURE || GetAttachmentID(colorAttachmentEnums[attachmentPoint]) != tex->GetID())
	{GuardedUnbind(); throw PPEResourceException("attaching failed");}// maybe it should not cast an exception..
    else
        colorAttachments[attachmentPoint] = tex;

    GuardedUnbind();
}

/** Attach texture with depth-format as a depth attachment
 * @param tex the texture to attach
 * @exception PPEResourceException thrown if the supplied texture doesn't doesn't have a depth internal format
 */
void FramebufferObject::AttachDepthTexture(ITexture2DPtr tex) {
    if (tex->GetFormat()!=TEX_DEPTH && tex->GetFormat()!=TEX_DEPTH_STENCIL) throw PPEResourceException("non-depth textures can't be attached as depth attachments");

    GuardedBind();
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, tex->GetID(), 0);

    // check if it really got attached (i'm not quite sure that all FBO-errors means that it didn't get attached, so that's why i'm not simply checking for errors here)
    if (GetAttachmentType(GL_DEPTH_ATTACHMENT_EXT) != GL_TEXTURE || GetAttachmentID(GL_DEPTH_ATTACHMENT_EXT) != tex->GetID())
	{GuardedUnbind(); throw PPEResourceException("attaching failed");}// maybe it should not cast an exception..
    else
        depthAttachment = tex;

    GuardedUnbind();
}

/** Attach texture with stencil-format as a stencil attachment
 * @param tex the texture to attach
 * @exception PPEResourceException thrown if the supplied texture doesn't doesn't have a stencil internal format
 */
void FramebufferObject::AttachStencilTexture(ITexture2DPtr tex) {
    if (tex->GetFormat()!=TEX_DEPTH_STENCIL) throw PPEResourceException("non-stencil textures can't be attached as stencil attachments");

    GuardedBind();
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_TEXTURE_2D, tex->GetID(), 0);

    // check if it really got attached (i'm not quite sure that all FBO-errors means that it didn't get attached, so that's why i'm not simply checking for errors here)
    if (GetAttachmentType(GL_STENCIL_ATTACHMENT_EXT) != GL_TEXTURE || GetAttachmentID(GL_STENCIL_ATTACHMENT_EXT) != tex->GetID())
	{GuardedUnbind(); throw PPEResourceException("attaching failed");}// maybe it should not cast an exception..
    else
        stencilAttachment = tex;

    GuardedUnbind();
}

/** Detach color attachment at the supplied attachmentpoint
 * @exception PPEResourceException thrown if illegal attachmentPoint (less than 0 or grater than FramebufferObject::GetMaxNumColorAttachments)
 */
void FramebufferObject::DetachColorAttachment(int attachmentPoint) {
    if (attachmentPoint < 0 || attachmentPoint >= maxNumColorAttachments) throw PPEResourceException("illegal attachmentPoint");
    GuardedBind();
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, colorAttachmentEnums[attachmentPoint], GL_RENDERBUFFER_EXT, 0);
    colorAttachments[attachmentPoint].reset();
    GuardedUnbind();
}

/** Detach depth attachment
 */
void FramebufferObject::DetachDepthAttachment() {
    GuardedBind();
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, 0);
    depthAttachment.reset();
    GuardedUnbind();
}

/** Detach stencil attachment
 */
void FramebufferObject::DetachStencilAttachment() {
    GuardedBind();
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, 0);
    stencilAttachment.reset();
    GuardedUnbind();
}

/** Get maximum number of color-attachments allowed for FBOs on this gfx-card.
 *  (legal attachment-points are 0,1,...,FramebufferObject::GetMaxNumColorAttachments() - 1)
 *  @returns max number of color-attachments on this gfx-card
 */
int FramebufferObject::GetMaxNumColorAttachments() {
    return (int)maxNumColorAttachments;
}

/** Get OpenGL handle of this FBO
 *  @returns the handle
 */
int FramebufferObject::GetID() {
    return (int)fboID;
}


/* returns true on ok, false on fail */
bool FramebufferObject::CheckFrameBufferStatus() {
    GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    switch (status) {
	case GL_FRAMEBUFFER_COMPLETE_EXT:
	    return true;
	    break;
	case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
	    /* hardware dependent error - choose different formats */
	    //throw PPEResourceException("FramebufferObject: framebuffer unsupported! choose different formats (hardware dependent)");
	    return false;
	    break;
	default:
	    /* programming error; will fail on all hardware */
	    throw PPEResourceException("FramebufferObject: error!");
	    return false;
	    break;
    }
}




// private, so it doesn't avoid side effects (no push/pop attrib). Returns: GL_NONE | GL_TEXTURE | GL_RENDERBUFFER_EXT
GLenum FramebufferObject::GetAttachmentType(GLenum attachmentEnum) {
    Bind();
    GLint type;
    glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER_EXT, attachmentEnum, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_EXT, &type); // se spec linie 4842
    return (GLenum)type;
}


// private, so it doesn't avoid side effects (no push/pop attrib). Returns: handle (=name) of curr bound texture or renderbuffer (0 if none)
GLuint FramebufferObject::GetAttachmentID(GLenum attachmentEnum) {
    Bind();
    GLint attID;
    glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER_EXT, attachmentEnum, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_EXT, &attID); // se spec linie 4847
    return (GLuint)attID;
}



/** Get Color attachment attached to this FBO at the supplied attachment point (NULL (wrapped in a smart-pointer) if none)
 * @exception PPEResourceException thrown if illegal attachmentPoint (less than 0 or grater than FramebufferObject::GetMaxNumColorAttachments)
 */
IImagePtr FramebufferObject::GetColorAttachment(int attachmentPoint) {
    if (attachmentPoint < 0 || attachmentPoint >= maxNumColorAttachments) throw PPEResourceException("illegal attachmentPoint");
    return colorAttachments[attachmentPoint];
}

/** Get Depth attachment attached to this FBO (NULL (wrapped in a smart-pointer) if none)
 */
IImagePtr FramebufferObject::GetDepthAttachment() {
    return depthAttachment;
}

/** Get Stencil attachment attached to this FBO (NULL (wrapped in a smart-pointer) if none)
 */
IImagePtr FramebufferObject::GetStencilAttachment() {
    return stencilAttachment;
}




/** sets default (=all attached buffers are targets in order (incl. GL_NONE) (buf0=att0, buf1=att1, ...)) */
// (note: hører til fbo'en, så den behøver ikke sættes hele tiden (efter hver bind). Se specs linie 5358)
void FramebufferObject::SelectDrawBuffers() {
    GuardedBind();

    GLenum* drawbuffers = new GLenum[maxNumColorAttachments];
    for (int i=0; i<maxNumColorAttachments; i++) {
	GLuint attID = GetAttachmentID(colorAttachmentEnums[i]);

	if (attID != 0) drawbuffers[i] = colorAttachmentEnums[i];
	else            drawbuffers[i] = GL_NONE;
    }

    glDrawBuffers(maxNumColorAttachments, drawbuffers);

    delete drawbuffers;
    GuardedUnbind();
}

/** sets only one user specified color buffer as the target, all other to GL_NONE */
// (evt. også hav version for array)
// (note: hører til fbo'en, så den behøver ikke sættes hele tiden (efter hver bind). Se specs linie 5358)
void FramebufferObject::SelectDrawBuffers(int attachmentPoint) {
    if (attachmentPoint < 0 || attachmentPoint >= maxNumColorAttachments) throw PPEResourceException("illegal attachmentPoint");

    GuardedBind();

    if (GetAttachmentID(colorAttachmentEnums[attachmentPoint]) == 0) {GuardedUnbind(); throw PPEResourceException("nothing attached at attachmentPoint");}

    GLenum* drawbuffers = new GLenum[maxNumColorAttachments];
    for (int i=0; i<maxNumColorAttachments; i++) {
	if (i == attachmentPoint) drawbuffers[i] = colorAttachmentEnums[i];
	else                      drawbuffers[i] = GL_NONE;
    }

    glDrawBuffers(maxNumColorAttachments, drawbuffers);

    delete drawbuffers;
    GuardedUnbind();

}


void FramebufferObject::GuardedBind()  {
    if (savedFboID != 0) throw PPEResourceException("internal error"); // to prevent recursive-ish routines messing up the guardedBind
    glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, &savedFboID);
    if (fboID != (GLuint)savedFboID) // no need to bind if this FBO is already bound
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboID);
}

void FramebufferObject::GuardedUnbind() {
    if (fboID != (GLuint)savedFboID)
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, (GLuint)savedFboID);
    savedFboID = 0;
}

} // NS Resources
} // NS OpenEngine
