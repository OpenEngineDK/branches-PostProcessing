#include "TextureCube.h"

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
TextureCube::TextureCube(int width, int height, TexelFormat format, TextureWrap wrapS, TextureWrap wrapT, TextureWrap wrapR, TextureFilter filterMag, TextureFilter filterMin) {
    this->texID = 0; // must be done before calling createOrModifyTexture!
    CreateOrModifyTexture(width, height, format, wrapS, wrapT, wrapR, filterMag, filterMin);
}

/** delete this texture
 */
TextureCube::~TextureCube() {
    glDeleteTextures(1, &texID);
}

/** clone this texture
 *  @return a copy of this texture
 */
ITextureCubePtr TextureCube::Clone() {
    ITextureCubePtr destTexture = ITextureCubePtr(new TextureCube(GetWidth(), GetHeight(), GetFormat(), GetWrapS(), GetWrapT(), GetWrapR(), GetMagFilter(), GetMinFilter()));
    CopyTexture(destTexture);
    return destTexture;
}

/** clone this texture, by replacing the argument texture with the clone
 *  @param destTexture the texture to replace with a copy of this texture
 */
void TextureCube::Clone(ITextureCubePtr destTexture) {
    CopyTexture(destTexture);
}

/** bind this texture
 */
void TextureCube::Bind() {
    glBindTexture(GL_TEXTURE_CUBE_MAP, texID);
}

/** unbind any texture (not only this one)
 *  @note no need to call this method between bind calls
 */
void TextureCube::Unbind() {
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

/** get OpenGL handle of this texture
 */
int TextureCube::GetID() {
    return (int)texID;
}

/** set OpenGL handle of this texture
 *  @param texID handle
 */
void TextureCube::SetID(int texID) {
    if (texID <= 0) throw new PPEResourceException("texID was <= 0");
    this->texID = texID;
}

/** set wrapS of this texture
 *  @param wrap the wrap setting
 */
void TextureCube::SetWrapS(TextureWrap wrap) {
    glPushAttrib(GL_TEXTURE_BIT); // remember currently bound 2D-texture (so that this method doesn't give any side effects)
    Bind();
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GetGLWrap(wrap));
    glPopAttrib();
}

/** set wrapT of this texture
 *  @param wrap the wrap setting
 */
void TextureCube::SetWrapT(TextureWrap wrap) {
    glPushAttrib(GL_TEXTURE_BIT); // remember currently bound 2D-texture (so that this method doesn't give any side effects)
    Bind();
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GetGLWrap(wrap));
    glPopAttrib();
}

/** set wrapR of this texture
 *  @param wrap the wrap setting
 */
void TextureCube::SetWrapR(TextureWrap wrap) {
    glPushAttrib(GL_TEXTURE_BIT); // remember currently bound 2D-texture (so that this method doesn't give any side effects)
    Bind();
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GetGLWrap(wrap));
    glPopAttrib();
}

/** set mag filter of this texture
 *  @param filter the filter setting
 */
void TextureCube::SetMagFilter(TextureFilter filter) {
    glPushAttrib(GL_TEXTURE_BIT); // remember currently bound 2D-texture (so that this method doesn't give any side effects)
    Bind();
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GetGLFilter(filter));
    glPopAttrib();
}

/** set min filter of this texture
 *  @param filter the filter setting
 */
void TextureCube::SetMinFilter(TextureFilter filter) {
    glPushAttrib(GL_TEXTURE_BIT); // remember currently bound 2D-texture (so that this method doesn't give any side effects)
    Bind();
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GetGLFilter(filter));
    glPopAttrib();
}


/** get wrap s of this texture
 *  @returns wrap s
 */
TextureWrap TextureCube::GetWrapS() {
    glPushAttrib(GL_TEXTURE_BIT); // remember currently bound 2D-texture (so that this method doesn't give any side effects)
    GLint wrap_s;
    this->Bind();
    glGetTexParameteriv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, &wrap_s);
    glPopAttrib();
    return GetOEWrap(wrap_s);
}

/** get wrap t of this texture
 *  @returns wrap t
 */
TextureWrap TextureCube::GetWrapT() {
    glPushAttrib(GL_TEXTURE_BIT); // remember currently bound 2D-texture (so that this method doesn't give any side effects)
    GLint wrap_t;
    this->Bind();
    glGetTexParameteriv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, &wrap_t);
    glPopAttrib();
    return GetOEWrap(wrap_t);
}

/** get wrap r of this texture
 *  @returns wrap r
 */
TextureWrap TextureCube::GetWrapR() {
    glPushAttrib(GL_TEXTURE_BIT); // remember currently bound 2D-texture (so that this method doesn't give any side effects)
    GLint wrap_r;
    this->Bind();
    glGetTexParameteriv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, &wrap_r);
    glPopAttrib();
    return GetOEWrap(wrap_r);
}

/** get mag filter of this texture
 *  @returns mag filter
 */
TextureFilter TextureCube::GetMagFilter() {
    glPushAttrib(GL_TEXTURE_BIT); // remember currently bound 2D-texture (so that this method doesn't give any side effects)
    GLint filter_mag;
    this->Bind();
    glGetTexParameteriv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, &filter_mag);
    glPopAttrib();
    return GetOEFilter(filter_mag);
}

/** get min filter of this texture
 *  @returns min filter
 */
TextureFilter TextureCube::GetMinFilter() {
    glPushAttrib(GL_TEXTURE_BIT); // remember currently bound 2D-texture (so that this method doesn't give any side effects)
    GLint filter_min;
    this->Bind();
    glGetTexParameteriv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, &filter_min);
    glPopAttrib();
    return GetOEFilter(filter_min);
}

/** get the internal format of this texture
 *  @returns the internal format
 */
TexelFormat TextureCube::GetFormat() {
    glPushAttrib(GL_TEXTURE_BIT); // remember currently bound 2D-texture (so that this method doesn't give any side effects)
    GLint internalFormat;
    this->Bind();
    // antager alle 6 faces har samme internal format
    glGetTexLevelParameteriv(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalFormat);
    glPopAttrib();
    return GetOEInternalFormat(internalFormat);
}

/** get the width of this texture
 *  @returns the width
 */
unsigned int TextureCube::GetWidth() {
    glPushAttrib(GL_TEXTURE_BIT); // remember currently bound 2D-texture (so that this method doesn't give any side effects)
    GLsizei width;
    this->Bind();
    glGetTexLevelParameteriv(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_TEXTURE_WIDTH, &width);
    glPopAttrib();
    return (int)width;
}

/** get the height of this texture
 *  @returns the height
 */
unsigned int TextureCube::GetHeight() {
    glPushAttrib(GL_TEXTURE_BIT); // remember currently bound 2D-texture (so that this method doesn't give any side effects)
    GLsizei height;
    this->Bind();
    glGetTexLevelParameteriv(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_TEXTURE_HEIGHT, &height);
    glPopAttrib();
    return (int)height;
}

/** get the depth of this texture (always 0)
 *  @returns the depth
 */
int TextureCube::GetZDepth() {
    return 0;
}

ColorFormat TextureCube::GetColorFormat() {
    unsigned int depth = GetDepth();
    if (depth==32)
        return RGBA;
    else if (depth==24)
        return RGB;
    else if (depth==8)
        return LUMINANCE;
    else
        throw Exception("unknown color depth");
}

/** get the bit-depth of this texture (not z-depth!!!)
 *  @returns the bit-depth
 */
unsigned int TextureCube::GetDepth() {
    throw new PPEResourceException("GetDepth: not implemented yet");
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
void TextureCube::Resize(int width, int height) { // gldelete og lav igen, men genbrug texID. (is a slow operation)
    CreateOrModifyTexture(width, height, GetFormat(), GetWrapS(), GetWrapT(), GetWrapR(), GetMagFilter(), GetMinFilter());
}

/** Resize this texture and change internal format (destructive resize - content will not be preserved)
 * @param width the new width
 * @param height the new height
 * @param format the new internal format
 *
 * @note: content of textures will be trashed
 * @todo: keep the contents somehow. This will probably trash performance (when it's not needed), so make it optional.
 */
void TextureCube::Resize(int width, int height, TexelFormat format) { // gldelete og lav igen, men genbrug texID. (is a slow operation)
    CreateOrModifyTexture(width, height, format, GetWrapS(), GetWrapT(), GetWrapR(), GetMagFilter(), GetMinFilter());
}

/* these returns opengl stuff depending on the instance vars */
GLint TextureCube::GetGLInternalFormat(TexelFormat format) {
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

GLenum TextureCube::GetGLFormat(TexelFormat format) {
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

GLint TextureCube::GetGLWrap(TextureWrap wrap) {
    switch (wrap) {
	case TEX_CLAMP:           return GL_CLAMP;
	case TEX_CLAMP_TO_EDGE:   return GL_CLAMP_TO_EDGE;
	case TEX_CLAMP_TO_BORDER: return GL_CLAMP_TO_BORDER;
	case TEX_REPEAT:	  return GL_REPEAT;
	case TEX_MIRRORED_REPEAT: return GL_MIRRORED_REPEAT;
	default:                  throw new PPEResourceException("illegal wrap");
    }
}

GLint TextureCube::GetGLFilter(TextureFilter filter) {
    switch (filter) {
	case TEX_NEAREST: return GL_NEAREST;
	case TEX_LINEAR:  return GL_LINEAR;
	default:          throw new PPEResourceException("illegal filter");
    }
}

GLsizei TextureCube::GetGLWidth(int width) {
    return (GLsizei)width;
}

GLsizei TextureCube::GetGLHeight(int height) {
    return (GLsizei)height;
}


/* hej */
TexelFormat TextureCube::GetOEInternalFormat(GLint glInternalFormat) {
    switch (glInternalFormat) {
	case GL_DEPTH_COMPONENT:      return TEX_DEPTH;
	case GL_DEPTH24_STENCIL8_EXT: return TEX_DEPTH_STENCIL;
	case GL_LUMINANCE8:           return TEX_LUMINANCE;
	case GL_RGB8:                 return TEX_RGB;
	case GL_RGBA8:                return TEX_RGBA;
	case GL_LUMINANCE16F_ARB:     return TEX_LUMINANCE_FLOAT;
	case GL_RGB16F_ARB:           return TEX_RGB_FLOAT;
	case GL_RGBA16F_ARB:          return TEX_RGBA_FLOAT;
	default:                      throw new PPEResourceException("getOEInternalFormat: illegal format");
    }
}

TextureWrap TextureCube::GetOEWrap(GLint glWrap) {
    switch (glWrap) {
	case GL_CLAMP:           return TEX_CLAMP;
	case GL_CLAMP_TO_EDGE:   return TEX_CLAMP_TO_EDGE;
	case GL_CLAMP_TO_BORDER: return TEX_CLAMP_TO_BORDER;
	case GL_REPEAT:	         return TEX_REPEAT;
	case GL_MIRRORED_REPEAT: return TEX_MIRRORED_REPEAT;
	default:                 throw new PPEResourceException("illegal wrap");
    }
}

TextureFilter TextureCube::GetOEFilter(GLint glFilter) {
    switch (glFilter) {
	case GL_NEAREST: return TEX_NEAREST;
	case GL_LINEAR:  return TEX_LINEAR;
	default:         throw new PPEResourceException("illegal filter");
    }
}


// either creates a new texture or modifies an existing 2D-texture (modify = reuse texID).
// Generates a new texture if texID=0, otherwise it just changes the parameters of the existing texture corresponding to texID.
// NOTE: when mofifying trashes everything that was previously in the texture.
void TextureCube::CreateOrModifyTexture(int width, int height, TexelFormat format, TextureWrap wrapS, TextureWrap wrapT, TextureWrap wrapR, TextureFilter filterMag, TextureFilter filterMin) {

    glPushAttrib(GL_TEXTURE_BIT); // to avoid side effects
    if (texID == 0) glGenTextures(1, &texID); // if not already created, then create texture-handle for this texture
    Bind();

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GetGLWrap(wrapS)); // (important to remember to set the texture parameters for it to work!)
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GetGLWrap(wrapT));
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GetGLWrap(wrapR));
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GetGLFilter(filterMag));
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GetGLFilter(filterMin));

    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GetGLInternalFormat(format), GetGLWidth(width), GetGLHeight(height), 0, GetGLFormat(format), GL_FLOAT, NULL);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GetGLInternalFormat(format), GetGLWidth(width), GetGLHeight(height), 0, GetGLFormat(format), GL_FLOAT, NULL);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GetGLInternalFormat(format), GetGLWidth(width), GetGLHeight(height), 0, GetGLFormat(format), GL_FLOAT, NULL);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GetGLInternalFormat(format), GetGLWidth(width), GetGLHeight(height), 0, GetGLFormat(format), GL_FLOAT, NULL);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GetGLInternalFormat(format), GetGLWidth(width), GetGLHeight(height), 0, GetGLFormat(format), GL_FLOAT, NULL);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GetGLInternalFormat(format), GetGLWidth(width), GetGLHeight(height), 0, GetGLFormat(format), GL_FLOAT, NULL);

    //unbind(); // unbind again
    glPopAttrib();
}

/** copy the content and settings of this texture to another texture
 *  @param destTex the destination texture
 *  @note seems OpenGL doesn't have build-in support for this, so we use FBOs to achieve it
 */
void TextureCube::CopyTexture(ITextureCubePtr destTex) {
    throw new PPEResourceException("not implemented yet!");
}


/**
 * Checks for OpenGL errors.
 * Extremely useful debugging function: When developing,
 * make sure to call this after almost every GL call.
 */
void TextureCube::CheckGLErrors (const char *label) {
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


/** Get image type. This is IMG_TEXTURE_CUBE.
 *  @returns IMG_TEXTURE_CUBE
 */
ImageType TextureCube::GetImageType() {         // TextureCube, renderbuffer, ...
    return IMG_TEXTURE_CUBE;
}

/** get number of components of each texel
 */
int TextureCube::GetNumComponents() {
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
unsigned char* TextureCube::GetData(int face, GLenum type) {
    throw new PPEResourceException("not implemented yet!");
    return NULL;
}


/** returns an array of unsigned chars of size width*height*numcomponents
 *  you should delete the array with "delete" when you're done with it.
 *  (returns arrays instead of vectors, because this is how it is in ITextureResource)
 *  (face must be in [0;5])
 */
unsigned char* TextureCube::GetData(int face) {
    return GetData(GL_UNSIGNED_BYTE);
}

/** returns an array of floats of size width*height*numcomponents
 *  you should delete the array with "delete" when you're done with it.
 *  (returns arrays instead of vectors, because this is how it is in ITextureResource)
 *  (face must be in [0;5])
 */
float* TextureCube::GetFloatData(int face) {
    return (float*)GetData(GL_FLOAT);
}

/** expected array of the same size as the texture multiplied by num components: GetWidth()*GetHeight()*numcomp
 *  (face must be in [0;5])
 */
void TextureCube::SetData(int face, unsigned char* data) {
    glPushAttrib(GL_TEXTURE_BIT); // to avoid side effects
    Bind();
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GetGLInternalFormat(GetFormat()), GetGLWidth(GetWidth()), GetGLHeight(GetHeight()), 0,
                 GetGLFormat(GetFormat()), GL_UNSIGNED_BYTE, data);
    glPopAttrib();
}

/** expected array of the same size as the texture multiplied by num components: GetWidth()*GetHeight()*numcomp
 *  (face must be in [0;5])
 */
void TextureCube::SetFloatData(int face, float* data) {
    glPushAttrib(GL_TEXTURE_BIT); // to avoid side effects
    Bind();
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GetGLInternalFormat(GetFormat()), GetGLWidth(GetWidth()), GetGLHeight(GetHeight()), 0,
                 GetGLFormat(GetFormat()), GL_FLOAT, data);
    glPopAttrib();
}

/** this method does not make sense for this type of resource.
 *  It is here to stay compatible with the ITextureResource interface.
 */
void TextureCube::Load() {
}

/** this method does not make sense for this type of resource.
 *  It is here to stay compatible with the ITextureResource interface.
 */
void TextureCube::Unload() {
}

} // NS Resources
} // NS OpenEngine
