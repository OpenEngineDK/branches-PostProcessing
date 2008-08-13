#include "Texture2D.h"

#include <Utils/Convert.h>

using OpenEngine::Utils::Convert;

/* @author Bjarke N. Laustsen
 */
namespace OpenEngine {
namespace Resources {

/** Create a new 2D texture
 * @param width the textuer width
 * @param height the texture height
 * @param format the texture format
 * @param wrapS the wrap_s setting
 * @param wrapT the wrap_t setting
 * @param filterMag the mag filter setting
 * @param filterMin the min filter setting
 */
Texture2D::Texture2D(int width, int height, TexelFormat format, TextureWrap wrapS, TextureWrap wrapT, TextureFilter filterMag, TextureFilter filterMin) {
    this->texID = 0; // must be done before calling createOrModifyTexture!

    CreateOrModifyTexture(width, height, format, wrapS, wrapT, filterMag, filterMin);
}

/** delete this texture
 */
Texture2D::~Texture2D() {
    glDeleteTextures(1, &texID);
}

/** clone this texture
 *  @return a copy of this texture
 */
ITexture2DPtr Texture2D::Clone() {
    ITexture2DPtr destTexture = ITexture2DPtr(new Texture2D(GetWidth(), GetHeight(), GetFormat(), GetWrapS(), GetWrapT(), GetMagFilter(), GetMinFilter()));
    CopyTexture(destTexture);
    return destTexture;
}

/** clone this texture, by replacing the argument texture with the clone
 *  @param destTexture the texture to replace with a copy of this texture
 */
void Texture2D::Clone(ITexture2DPtr destTexture) {
    CopyTexture(destTexture);
}

/** bind this texture
 */
void Texture2D::Bind() {
    glBindTexture(GL_TEXTURE_2D, texID);
}

/** unbind any texture (not only this one)
 *  @note no need to call this method between bind calls
 */
void Texture2D::Unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}

/** get OpenGL handle of this texture
 */
int Texture2D::GetID() {
    return (int)texID;
}

/** set OpenGL handle of this texture
 *  @param texID handle
 */
void Texture2D::SetID(int texID) {
    if (texID <= 0) throw new PPEResourceException("texID was <= 0");
    this->texID = texID;
}

/** set wrapS of this texture
 *  @param wrap the wrap setting
 */
void Texture2D::SetWrapS(TextureWrap wrap) {
    glPushAttrib(GL_TEXTURE_BIT); // remember currently bound 2D-texture (so that this method doesn't give any side effects)
    Bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GetGLWrap(wrap));
    glPopAttrib();
}

/** set wrapT of this texture
 *  @param wrap the wrap setting
 */
void Texture2D::SetWrapT(TextureWrap wrap) {
    glPushAttrib(GL_TEXTURE_BIT); // remember currently bound 2D-texture (so that this method doesn't give any side effects)
    Bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GetGLWrap(wrap));
    glPopAttrib();
}

/** set mag filter of this texture
 *  @param filter the filter setting
 */
void Texture2D::SetMagFilter(TextureFilter filter) {
    glPushAttrib(GL_TEXTURE_BIT); // remember currently bound 2D-texture (so that this method doesn't give any side effects)
    Bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GetGLFilter(filter));
    glPopAttrib();
}

/** set min filter of this texture
 *  @param filter the filter setting
 */
void Texture2D::SetMinFilter(TextureFilter filter) {
    glPushAttrib(GL_TEXTURE_BIT); // remember currently bound 2D-texture (so that this method doesn't give any side effects)
    Bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GetGLFilter(filter));
    glPopAttrib();
}


/** get wrap s of this texture
 *  @returns wrap s
 */
TextureWrap Texture2D::GetWrapS() {
    glPushAttrib(GL_TEXTURE_BIT); // remember currently bound 2D-texture (so that this method doesn't give any side effects)
    GLint wrap_s;
    this->Bind();
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &wrap_s);
    glPopAttrib();
    return GetOEWrap(wrap_s);
}

/** get wrap t of this texture
 *  @returns wrap t
 */
TextureWrap Texture2D::GetWrapT() {
    glPushAttrib(GL_TEXTURE_BIT); // remember currently bound 2D-texture (so that this method doesn't give any side effects)
    GLint wrap_t;
    this->Bind();
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, &wrap_t);
    glPopAttrib();
    return GetOEWrap(wrap_t);
}

/** get mag filter of this texture
 *  @returns mag filter
 */
TextureFilter Texture2D::GetMagFilter() {
    glPushAttrib(GL_TEXTURE_BIT); // remember currently bound 2D-texture (so that this method doesn't give any side effects)
    GLint filter_mag;
    this->Bind();
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &filter_mag);
    glPopAttrib();
    return GetOEFilter(filter_mag);
}

/** get min filter of this texture
 *  @returns min filter
 */
TextureFilter Texture2D::GetMinFilter() {
    glPushAttrib(GL_TEXTURE_BIT); // remember currently bound 2D-texture (so that this method doesn't give any side effects)
    GLint filter_min;
    this->Bind();
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &filter_min);
    glPopAttrib();
    return GetOEFilter(filter_min);
}

/** get the internal format of this texture
 *  @returns the internal format
 */
TexelFormat Texture2D::GetFormat() {
    glPushAttrib(GL_TEXTURE_BIT); // remember currently bound 2D-texture (so that this method doesn't give any side effects)
    GLint internalFormat;
    this->Bind();
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalFormat);
    glPopAttrib();
    return GetOEInternalFormat(internalFormat);
}

/** get the width of this texture
 *  @returns the width
 */
int Texture2D::GetWidth() {
    glPushAttrib(GL_TEXTURE_BIT); // remember currently bound 2D-texture (so that this method doesn't give any side effects)
    GLsizei width;
    this->Bind();
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
    glPopAttrib();
    return (int)width;
}

/** get the height of this texture
 *  @returns the height
 */
int Texture2D::GetHeight() {
    glPushAttrib(GL_TEXTURE_BIT); // remember currently bound 2D-texture (so that this method doesn't give any side effects)
    GLsizei height;
    this->Bind();
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
    glPopAttrib();
    return (int)height;
}

/** get the depth of this texture (always 0)
 *  @returns the depth
 */
int Texture2D::GetZDepth() {
    return 0;
}

/** get the bit-depth of this texture (not z-depth!!!)
 *  @returns the bit-depth
 */
int Texture2D::GetDepth() {
    switch (GetFormat()) {
	case TEX_DEPTH:           return 24; // could be 16 as well.. can this be detected?
	case TEX_DEPTH_STENCIL:   return 32;
	case TEX_LUMINANCE:       return 8;
	case TEX_RGB:	          return 8*3;
	case TEX_RGBA:	          return 8*4;
	case TEX_LUMINANCE_FLOAT: return 16;
	case TEX_RGB_FLOAT:	  return 16*3;
	case TEX_RGBA_FLOAT:	  return 16*4;
	default:                  throw new PPEResourceException("GetDepth: illegal format");
    }
}

/** Resize this texture (destructive resize - content will not be preserved)
 * @param width the new width
 * @param height the new height
 *
 * @note: content of textures will be trashed
 * @todo: keep the contents somehow. This will probably trash performance (when it's not needed), so make it optional.
 */
void Texture2D::Resize(int width, int height) { // gldelete og lav igen, men genbrug texID. (is a slow operation)
    CreateOrModifyTexture(width, height, GetFormat(), GetWrapS(), GetWrapT(), GetMagFilter(), GetMinFilter());
}

/** Resize this texture and change internal format (destructive resize - content will not be preserved)
 * @param width the new width
 * @param height the new height
 * @param format the new internal format
 *
 * @note: content of textures will be trashed
 * @todo: keep the contents somehow. This will probably trash performance (when it's not needed), so make it optional.
 */
void Texture2D::Resize(int width, int height, TexelFormat format) { // gldelete og lav igen, men genbrug texID. (is a slow operation)
    CreateOrModifyTexture(width, height, format, GetWrapS(), GetWrapT(), GetMagFilter(), GetMinFilter());
}

/* these returns opengl stuff depending on the instance vars */
GLint Texture2D::GetGLInternalFormat(TexelFormat format) {
    switch (format) {
	case TEX_DEPTH:           return GL_DEPTH_COMPONENT;
	case TEX_DEPTH_STENCIL:   return GL_DEPTH24_STENCIL8_EXT;
	case TEX_LUMINANCE:       return GL_LUMINANCE8;
	case TEX_RGB:	          return GL_RGB8;
	case TEX_RGBA:	          return GL_RGBA8;
	case TEX_LUMINANCE_FLOAT: return GL_LUMINANCE16F_ARB;
	case TEX_RGB_FLOAT:	  return GL_RGB16F_ARB;
	case TEX_RGBA_FLOAT:	  return GL_RGBA16F_ARB;
	default:                  throw new PPEResourceException("getGLInternalFormat: illegal format");
    }
}

GLenum Texture2D::GetGLFormat(TexelFormat format) {
    switch (format) {
	case TEX_DEPTH:           return GL_DEPTH_COMPONENT;
	case TEX_DEPTH_STENCIL:   return GL_DEPTH_STENCIL_EXT;
	case TEX_LUMINANCE:       return GL_LUMINANCE;
	case TEX_RGB:	          return GL_RGB;
	case TEX_RGBA:	          return GL_RGBA;
	case TEX_LUMINANCE_FLOAT: return GL_LUMINANCE;
	case TEX_RGB_FLOAT:	  return GL_RGB;
	case TEX_RGBA_FLOAT:	  return GL_RGBA;
	default:                  throw new PPEResourceException("getGLFormat: illegal format");
    }
}

GLint Texture2D::GetGLWrap(TextureWrap wrap) {
    switch (wrap) {
	case TEX_CLAMP:           return GL_CLAMP;
	case TEX_CLAMP_TO_EDGE:   return GL_CLAMP_TO_EDGE;
	case TEX_CLAMP_TO_BORDER: return GL_CLAMP_TO_BORDER;
	case TEX_REPEAT:	  return GL_REPEAT;
	case TEX_MIRRORED_REPEAT: return GL_MIRRORED_REPEAT;
	default:                  throw new PPEResourceException("illegal wrap");
    }
}

GLint Texture2D::GetGLFilter(TextureFilter filter) {
    switch (filter) {
	case TEX_NEAREST: return GL_NEAREST;
	case TEX_LINEAR:  return GL_LINEAR;
	default:          throw new PPEResourceException("illegal filter");
    }
}

GLsizei Texture2D::GetGLWidth(int width) {
    return (GLsizei)width;
}

GLsizei Texture2D::GetGLHeight(int height) {
    return (GLsizei)height;
}


/* hej */
TexelFormat Texture2D::GetOEInternalFormat(GLint glInternalFormat) {
    switch (glInternalFormat) {
	case GL_DEPTH_COMPONENT:      return TEX_DEPTH;
	case GL_DEPTH24_STENCIL8_EXT: return TEX_DEPTH_STENCIL;
	case GL_LUMINANCE8:           return TEX_LUMINANCE;
	case GL_RGB8:                 return TEX_RGB;
	case GL_RGBA8:                return TEX_RGBA;
	case GL_LUMINANCE16F_ARB:     return TEX_LUMINANCE_FLOAT;
	case GL_RGB16F_ARB:           return TEX_RGB_FLOAT;
	case GL_RGBA16F_ARB:          return TEX_RGBA_FLOAT;
    default:                      throw new PPEResourceException(("getOEInternalFormat: illegal format: " + Convert::ToString(glInternalFormat)).c_str());
    }
}

TextureWrap Texture2D::GetOEWrap(GLint glWrap) {
    switch (glWrap) {
	case GL_CLAMP:           return TEX_CLAMP;
	case GL_CLAMP_TO_EDGE:   return TEX_CLAMP_TO_EDGE;
	case GL_CLAMP_TO_BORDER: return TEX_CLAMP_TO_BORDER;
	case GL_REPEAT:	         return TEX_REPEAT;
	case GL_MIRRORED_REPEAT: return TEX_MIRRORED_REPEAT;
	default:                 throw new PPEResourceException("illegal wrap");
    }
}

TextureFilter Texture2D::GetOEFilter(GLint glFilter) {
    switch (glFilter) {
	case GL_NEAREST: return TEX_NEAREST;
	case GL_LINEAR:  return TEX_LINEAR;
	default:         throw new PPEResourceException("illegal filter");
    }
}


// either creates a new texture or modifies an existing 2D-texture (modify = reuse texID).
// Generates a new texture if texID=0, otherwise it just changes the parameters of the existing texture corresponding to texID.
// NOTE: when mofifying trashes everything that was previously in the texture.
void Texture2D::CreateOrModifyTexture(int width, int height, TexelFormat format, TextureWrap wrapS, TextureWrap wrapT, TextureFilter filterMag, TextureFilter filterMin) {
    glPushAttrib(GL_TEXTURE_BIT); // to avoid side effects
    if (texID == 0) glGenTextures(1, &texID); // if not already created, then create texture-handle for this texture
    Bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GetGLWrap(wrapS)); // (important to remember to set the texture parameters for it to work!)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GetGLWrap(wrapT));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GetGLFilter(filterMag));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GetGLFilter(filterMin));
    glTexImage2D(GL_TEXTURE_2D, 0, GetGLInternalFormat(format), GetGLWidth(width), GetGLHeight(height), 0, GetGLFormat(format), GL_FLOAT, NULL);
    //unbind(); // unbind again
    glPopAttrib();
}

/** copy the content and settings of this texture to another texture
 *  @param destTex the destination texture
 *  @note seems OpenGL doesn't have build-in support for this, so we use FBOs to achieve it
 */
void Texture2D::CopyTexture(ITexture2DPtr destTex) {

    if (destTex.get() == NULL) throw new PPEResourceException("destTex was NULL");
    if (GetFormat() == TEX_DEPTH_STENCIL) throw new PPEResourceException("depth_stencil texture copy not implemented");

    glPushAttrib(GL_ALL_ATTRIB_BITS); // to avoid side effects
    GLint savedFboID;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, &savedFboID);

    // fbo used for texture copying (shared among all Texture2D instances) @todo: use FBO class when it is made!
    static GLuint texCopyFboID = 0;

    // create fbo used for tesxture-copying, if not already made
    if (texCopyFboID == 0) {
	glGenFramebuffersEXT(1, &texCopyFboID);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, texCopyFboID);	// <- create and bind
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);		// <- unbind again
    }

    // get parameters of the source texture, so that the destination texture will have the same
    GLint   wrap_s;
    GLint   wrap_t;
    GLint   filter_mag;
    GLint   filter_min;
    GLsizei width;
    GLsizei height;
    GLint   internalFormat;

    this->Bind();
    glGetTexParameteriv     (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S            , &wrap_s);
    glGetTexParameteriv     (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T            , &wrap_t);
    glGetTexParameteriv     (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER        , &filter_mag);
    glGetTexParameteriv     (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER        , &filter_min);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH          , &width);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT         , &height);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalFormat);
    this->Unbind();

    // throw exception if the supplied destination texture wasn't 2D (if outTexID != bound2DTexture)
    GLint bound2DTex; // <- used to check if supplied destination texture is GL_TEXTURE_2D or not
    glBindTexture(GL_TEXTURE_2D, (GLuint)destTex->GetID());
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &bound2DTex);
    glBindTexture(GL_TEXTURE_2D, 0);
    if ((GLuint)destTex->GetID() != (GLuint)bound2DTex) throw new PPEResourceException("outTexID must be a 2D texture!");

    // set which attachment (color or depth) we should attach the source-texture to, depending on its internal-format
    GLenum attachment;
    if (internalFormat == GL_DEPTH_COMPONENT) {
	attachment = GL_DEPTH_ATTACHMENT_EXT;
    } else {
	attachment = GL_COLOR_ATTACHMENT0_EXT;
    }

    // bind copy-fbo
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, texCopyFboID);


    // select read and draw buffers
    if (internalFormat == GL_DEPTH_COMPONENT) {
        glReadBuffer(GL_NONE);
        glDrawBuffer(GL_NONE);
    } else {
	glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
    }

    // attach source-texture as a buffer
    if (internalFormat == GL_DEPTH_COMPONENT) {
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT , GL_TEXTURE_2D, this->texID, 0);
    } else {
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, this->texID, 0);
    }

    // modify the parameters of the destination texture
    glBindTexture(GL_TEXTURE_2D, (GLuint)destTex->GetID());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s); // (important to remember to set the texture parameters for it to work!)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter_mag);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter_min);

    // copy from the buffer (i.e. the source texture) to the textination texture (instead of the usual glTexImage2D)
    glCopyTexImage2D(GL_TEXTURE_2D, 0, internalFormat, 0, 0, width, height, 0);

    // unbind output-texture, attachment and FBO again
    if (internalFormat == GL_DEPTH_COMPONENT)
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT , GL_TEXTURE_2D, 0, 0);
    else
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, 0, 0);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, (GLuint)savedFboID);
    glPopAttrib();

    // check if something messed up
    CheckGLErrors("copyTexture");
}


/**
 * Checks for OpenGL errors.
 * Extremely useful debugging function: When developing,
 * make sure to call this after almost every GL call.
 */
void Texture2D::CheckGLErrors (const char *label) {
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


/** Get image type. This is IMG_TEXTURE_2D.
 *  @returns IMG_TEXTURE_2D
 */
ImageType Texture2D::GetImageType() {         // texture2D, renderbuffer, ...
    return IMG_TEXTURE_2D;
}

/** get number of components of each texel
 */
int Texture2D::GetNumComponents() {
    switch (GetFormat()) {
	case TEX_DEPTH:           return 1;
	case TEX_LUMINANCE:       return 1;
	case TEX_RGB:	          return 3;
	case TEX_RGBA:	          return 4;
	case TEX_LUMINANCE_FLOAT: return 1;
	case TEX_RGB_FLOAT:	  return 3;
	case TEX_RGBA_FLOAT:	  return 4;
	default:                  throw new PPEResourceException("GetNumComponents: illegal format");
    }
}


/** private method which gets the texture data into an array of specified type (needs cast afterwards)
 *  @note seems OpenGL doesn't have build-in support for this, so we use FBOs to achieve it
 */
unsigned char* Texture2D::GetData(GLenum type) {

    glPushAttrib(GL_ALL_ATTRIB_BITS); // to avoid side effects
    GLint savedFboID;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, &savedFboID);

    int arrayElemByteDepth;
    switch (type) {
	case GL_FLOAT        : arrayElemByteDepth = 4; break;
	case GL_UNSIGNED_BYTE: arrayElemByteDepth = 1; break;
	default: throw new PPEResourceException("GetData: internal error");
    }

    // fbo used for texure reading (shared among all Texture2D instances) @todo: use FBO class when it is made!
    static GLuint texReadFboID = 0;

    // create fbo used for tesxture-copying, if not already made
    if (texReadFboID == 0) {
	glGenFramebuffersEXT(1, &texReadFboID);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, texReadFboID);	// <- create and bind
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);		// <- unbind again
    }

    GLint internalFormat = GetGLInternalFormat(GetFormat());
    GLint format         = GetGLFormat(GetFormat());

    // bind copy-fbo
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, texReadFboID);

    // select which color-buffer to read from (for depth, we specify "none")
    glDrawBuffer(GL_NONE);
    if (internalFormat == GL_DEPTH_COMPONENT) {
        glReadBuffer(GL_NONE);
    } else {
	glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
    }

    // attach input-texture as a buffer
    if (internalFormat == GL_DEPTH_COMPONENT) {
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT , GL_TEXTURE_2D, this->texID, 0);
    } else {
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, this->texID, 0);
    }

    // allocate array of correct size to read into
    unsigned char* data = new unsigned char[GetWidth() * GetHeight() * GetNumComponents() * arrayElemByteDepth];

    // read the data from the texture into the array
    glReadPixels(0, 0, GetWidth(), GetHeight(), format, type, data);

    // remove attachments and unbind FBO again
    if (internalFormat == GL_DEPTH_COMPONENT)
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT , GL_TEXTURE_2D, 0, 0);
    else
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, 0, 0);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, (GLuint)savedFboID);
    glPopAttrib();

    CheckGLErrors("GetData");

    return data;
}


/** returns an array of unsigned chars of size width*height*numcomponents
 *  you should delete the array with "delete" when you're done with it.
 *  (returns arrays instead of vectors, because this is how it is in ITextureResource)
 */
unsigned char* Texture2D::GetData() {
    return GetData(GL_UNSIGNED_BYTE);
}

/** returns an array of floats of size width*height*numcomponents
 *  you should delete the array with "delete" when you're done with it.
 *  (returns arrays instead of vectors, because this is how it is in ITextureResource)
 */
float* Texture2D::GetFloatData() {
    return (float*)GetData(GL_FLOAT);
}

/** expected array of the same size as the texture multiplied by num components: GetWidth()*GetHeight()*numcomp
 */
void Texture2D::SetData(unsigned char* data) {
    glPushAttrib(GL_TEXTURE_BIT); // to avoid side effects
    Bind();
    glTexImage2D(GL_TEXTURE_2D, 0, GetGLInternalFormat(GetFormat()), GetGLWidth(GetWidth()), GetGLHeight(GetHeight()), 0,
                 GetGLFormat(GetFormat()), GL_UNSIGNED_BYTE, data);
    glPopAttrib();
}

/** expected array of the same size as the texture multiplied by num components: GetWidth()*GetHeight()*numcomp
 */
void Texture2D::SetFloatData(float* data) {
    glPushAttrib(GL_TEXTURE_BIT); // to avoid side effects
    Bind();
    //glTexImage2D(GL_TEXTURE_2D, 0, GetGLInternalFormat(format), GetGLWidth(width), GetGLHeight(height), 0, GetGLFormat(format), GL_FLOAT, data);
    glTexImage2D(GL_TEXTURE_2D, 0, GetGLInternalFormat(GetFormat()), GetGLWidth(GetWidth()), GetGLHeight(GetHeight()), 0,
                 GetGLFormat(GetFormat()), GL_FLOAT, data);
    glPopAttrib();
}

/** this method does not make sense for this type of resource.
 *  It is here to stay compatible with the ITextureResource interface.
 */
void Texture2D::Load() {
}

/** this method does not make sense for this type of resource.
 *  It is here to stay compatible with the ITextureResource interface.
 */
void Texture2D::Unload() {
}

} // NS Resources
} // NS OpenEngine
