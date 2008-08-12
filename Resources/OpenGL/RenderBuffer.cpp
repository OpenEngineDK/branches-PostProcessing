#include "RenderBuffer.h"

/* @author Bjarke N. Laustsen
 */
namespace OpenEngine {
namespace Resources {

/** Create a new 2D renderbuffer
 * @param width the renderbuffer width
 * @param height the renderbuffer height
 * @param format the renderbuffer format
 */
RenderBuffer::RenderBuffer(int width, int height, PixelFormat format) {
    this->rbID = 0; // must be done before calling createOrModifyRB!

    CreateOrModifyRB(width, height, format);
}

/** delete this renderbuffer
 */
RenderBuffer::~RenderBuffer() {
    glDeleteRenderbuffersEXT(1, &rbID);
}


/** bind this renderbuffer
 */
void RenderBuffer::Bind() {
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, rbID);
}

/** unbind any renderbuffer (not only this one)
 *  @note no need to call this method between bind calls
 */
void RenderBuffer::Unbind() {
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
}

/** get OpenGL handle of this renderbuffer
 */
int RenderBuffer::GetID() {
    return (int)rbID;
}

/** get the width of this renderbuffer
 *  @returns the width
 */
int RenderBuffer::GetWidth() {
    GuardedBind(); // avoid side effects
    GLsizei width;
    glGetRenderbufferParameterivEXT(GL_RENDERBUFFER_EXT, GL_RENDERBUFFER_WIDTH_EXT, &width);
    GuardedUnbind();
    return (int)width;
}

/** get the height of this renderbuffer
 *  @returns the height
 */
int RenderBuffer::GetHeight() {
    GuardedBind(); // avoid side effects
    GLsizei height;
    glGetRenderbufferParameterivEXT(GL_RENDERBUFFER_EXT, GL_RENDERBUFFER_HEIGHT_EXT, &height);
    GuardedUnbind();
    return (int)height;
}


/** get the depth of this renderbuffer (always 0)
 *  @returns the depth
 */
int RenderBuffer::GetZDepth() {
    return 0;
}

/** get the bit-depth of this renderbuffer (not z-depth!!!)
 *  @returns the bit-depth
 */
int RenderBuffer::GetDepth() {
    throw new NotImplemented();
}

/** get the internal format of this renderbuffer
 *  @returns the internal format
 */
PixelFormat RenderBuffer::GetFormat() {
    GuardedBind(); // avoid side effects
    GLint internalFormat;
    glGetRenderbufferParameterivEXT(GL_RENDERBUFFER_EXT, GL_RENDERBUFFER_INTERNAL_FORMAT_EXT, &internalFormat);
    GuardedUnbind();
    return GetOEInternalFormat(internalFormat);
}


/** Resize this renderbuffer (destructive resize - content will not be preserved)
 * @param width the new width
 * @param height the new height
 *
 * @note: content of renderbuffer will be trashed
 * @todo: keep the contents somehow. This will probably trash performance (when it's not needed), so make it optional.
 */
void RenderBuffer::Resize(int width, int height) {
    CreateOrModifyRB(width, height, GetFormat());
}

/** Resize this renderbuffer and change internal format (destructive resize - content will not be preserved)
 * @param width the new width
 * @param height the new height
 * @param format the new internal format
 *
 * @note: content of renderbuffer will be trashed
 * @todo: keep the contents somehow. This will probably trash performance (when it's not needed), so make it optional.
 */
void RenderBuffer::Resize(int width, int height, PixelFormat format) {
    CreateOrModifyRB(width, height, format);
}


/** returns an array of unsigned chars of size width*height*numcomponents
 *  you should delete the array with "delete" when you're done with it.
 *  (returns arrays instead of vectors, because this is how it is in ITextureResource)
 */
unsigned char* RenderBuffer::GetData() {
    throw new NotImplemented();
}

/** returns an array of floats of size width*height*numcomponents
 *  you should delete the array with "delete" when you're done with it.
 *  (returns arrays instead of vectors, because this is how it is in ITextureResource)
 */
float* RenderBuffer::GetFloatData() {
    throw new NotImplemented();
}

/** expected array of the same size as the texture multiplied by num components: GetWidth()*GetHeight()*numcomp
 */
void RenderBuffer::SetData(unsigned char* data) {
    throw new NotImplemented();
}

/** expected array of the same size as the texture multiplied by num components: GetWidth()*GetHeight()*numcomp
 */
void RenderBuffer::SetFloatData(float* data) {
    throw new NotImplemented();
}

void RenderBuffer::CreateOrModifyRB(int width, int height, PixelFormat format) {
    if (rbID == 0) glGenRenderbuffersEXT(1, &rbID);
    GuardedBind();
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GetGLInternalFormat(format), GetGLWidth(width), GetGLHeight(height));
    CheckGLErrors("createOrModifyRB");
    GuardedUnbind();
}

/* these returns opengl stuff depending on the instance vars */
GLint RenderBuffer::GetGLInternalFormat(PixelFormat format) {
    switch (format) {
	case RB_DEPTH:   return GL_DEPTH_COMPONENT;
	case RB_STENCIL: return GL_STENCIL_INDEX;
	case RB_RGB:	 return GL_RGB;
	case RB_RGBA:	 return GL_RGBA;
	default:         throw new PPEResourceException("illegal format");
    }
}

GLsizei RenderBuffer::GetGLWidth(int width) {
    return (GLsizei)width;
}

GLsizei RenderBuffer::GetGLHeight(int height) {
    return (GLsizei)height;
}

PixelFormat RenderBuffer::GetOEInternalFormat(GLint glInternalFormat) {
    switch (glInternalFormat) {
	case GL_DEPTH_COMPONENT: return RB_DEPTH;
	case GL_STENCIL_INDEX:   return RB_STENCIL;
	case GL_RGB:             return RB_RGB;
	case GL_RGBA:            return RB_RGBA;
	default:                 throw new PPEResourceException("illegal format2");
    }
}

/**
 * Checks for OpenGL errors.
 * Extremely useful debugging function: When developing,
 * make sure to call this after almost every GL call.
 */
void RenderBuffer::CheckGLErrors (const char *label) {
    GLenum errCode;
    const GLubyte *errStr;

    if ((errCode = glGetError()) != GL_NO_ERROR) {
	errStr = gluErrorString(errCode);
	logger.error << "OpenGL ERROR: ";
	if (errStr != NULL) logger.error << errStr << "<errCode=" << errCode << ">";
	else                logger.error << "<NULL - errCode=" << errCode << ">";
	logger.error << "(Label: " << label << ")\n." << logger.end;
    }

    //framebuffer specific stuff... nice 2 have
    GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    switch (status) {
	case GL_FRAMEBUFFER_COMPLETE_EXT: break;
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT : logger.error << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT (label:" << label << ")" << logger.end; break;
	case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT : logger.error << "GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT (label:" << label << ")" << logger.end; break;
	case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT    : logger.error << "GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT (label:" << label << ")" << logger.end; break;
	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT: logger.error << "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT (label:" << label << ")" << logger.end; break;
	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT: logger.error << "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT (label:" << label << ")" << logger.end; break;
	case GL_FRAMEBUFFER_UNSUPPORTED_EXT	      : logger.error << "GL_FRAMEBUFFER_UNSUPPORTED_EXT (label:" << label << ")" << logger.end; break; /* you gotta choose different formats */
	case GL_INVALID_FRAMEBUFFER_OPERATION_EXT     : logger.error << "GL_INVALID_FRAMEBUFFER_OPERATION_EXT (label:" << label << ")" << logger.end; break;
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT: logger.error << "GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT (label:" << label << ")" << logger.end; break;
	default:				 	logger.error << "UNKNOWN ERROR:" << status << logger.end; break;
    }
}


/** Get image type. This is IMG_RENDERBUFFER.
 *  @returns IMG_RENDERBUFFER
 */
ImageType RenderBuffer::GetImageType() {         // texture2D, renderbuffer, ...
    return IMG_RENDERBUFFER;
}

void RenderBuffer::GuardedBind()  {
    if (savedRbID != 0) throw new PPEResourceException("internal error"); // to prevent recursive-ish routines messing up the guardedBind
    glGetIntegerv(GL_RENDERBUFFER_BINDING_EXT, &savedRbID);
    if (rbID != (GLuint)savedRbID) // no need to bind if this RB is already bound
        glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, rbID);
}

void RenderBuffer::GuardedUnbind() {
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, (GLuint)savedRbID);
    savedRbID = 0;
}

} // NS Resources
} // NS OpenEngine
