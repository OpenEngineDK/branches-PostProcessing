#include "PostProcessingPass.h"

/*  @author Bjarke N. Laustsen
 */
namespace OpenEngine {
namespace PostProcessing {

PostProcessingPass::PostProcessingPass(vector<string> fpFileNames, int currScreenWidth, int currScreenHeight, int passID, IPostProcessingEffect* ppe) {
    this->currScreenWidth = currScreenWidth;
    this->currScreenHeight = currScreenHeight;

    this->passID = passID;
    this->ppe    = ppe;
    glGetIntegerv(GL_MAX_DRAW_BUFFERS, &(this->maxColorAttachments));

    // create the fragmentprogram for this pass
    fp = new FragmentProgram(fpFileNames);

    // create the fbo for this pass
    fbo = new FramebufferObject();

    inputColorBufferParameterName = "";
    inputDepthBufferParameterName = "";
    outputsToColorBuffer = false;
    outputsToDepthBuffer = false;
    for (int i=0; i<maxColorAttachments; i++) userBufferTextures.push_back(ITexture2DPtr());
}

PostProcessingPass::~PostProcessingPass() {
    // delete fragment program for this pass
    delete fp;

    // delete framebuffer-object for this pass
    delete fbo;
}



/** Bind a value to a uniform int, ivec2, ivec4 or ivec4 input-parameter of the fragmentprogram of this pass.
 *  The value of the input-parameter must be given as a int-vector of length n, where n is the vector dimension.
 *  (For int, ivec2, ivec3, ivec4 the dimension is respectively 1,2,3,4)
 *
 *  @param[in] fpParameterName the name of the input-parameter in the fragment program
 *  @param[in] intvector  the int-vector (must have length n)
 *  @exception PPEResourceException thrown if n is not 1,2,3 or 4
 */
void PostProcessingPass::BindInt(string fpParameterName, vector<int> intvector) {
    fp->BindInt(fpParameterName, intvector);
}

/** Bind values to a uniform array of ints, ivec2s, ivec4s or ivec4s to the fragmentprogram of this pass.
 *  The values must be given as a vector of int-vectors.
 *  The first vector contains the different elements of the array.
 *  These elements are int-vectors of length n, where n is the vector dimension.
 *  (For int, ivec2, ivec3, ivec4 the dimension n is respectively 1,2,3,4)
 *
 *  @param[in] fpParameterName the name of the input-parameter in the fragment program
 *  @param[in] intvectors  the vector of int-vector (must each have length n)
 *  @exception PPEResourceException thrown if n is not 1,2,3 or 4
 *  @exception PPEResourceException thrown if not all int-vectors have the same size
 */
void PostProcessingPass::BindInt(string fpParameterName, vector<vector<int> > intvectors) {
    fp->BindInt(fpParameterName, intvectors);
}

/** Bind a value to a uniform float, vec2, vec4 or vec4 input-parameter of the fragmentprogram of this pass.
 *  The value of the input-parameter must be given as a float-vector of length n, where n is the vector dimension.
 *  (For float, vec2, vec3, vec4 the dimension is respectively 1,2,3,4)
 *
 *  @param[in] fpParameterName the name of the input-parameter in the fragment program
 *  @param[in] floatvector  the float-vector (must have length n)
 *  @exception PPEResourceException thrown if n is not 1,2,3 or 4
 */
void PostProcessingPass::BindFloat(string fpParameterName, vector<float> floatvector) {
    fp->BindFloat(fpParameterName, floatvector);
}

/** Bind values to a uniform array of floats, vec2s, vec4s or vec4s to the fragmentprogram of this pass.
 *  The values must be given as a vector of int-vectors.
 *  The first vector contains the different elements of the array.
 *  These elements are int-vectors of length n, where n is the vector dimension.
 *  (For float, vec2, vec3, vec4 the dimension is respectively 1,2,3,4)
 *
 *  @param[in] fpParameterName the name of the input-parameter in the fragment program
 *  @param[in] floatvectors  the vector of int-vector (must each have length n)
 *  @exception PPEResourceException thrown if n is not 1,2,3 or 4
 *  @exception PPEResourceException thrown if not all int-vectors have the same size
 */
void PostProcessingPass::BindFloat(string fpParameterName, vector<vector<float> > floatvectors) {
    fp->BindFloat(fpParameterName, floatvectors);
}

/** Bind a value to a uniform matNxM input-parameter of the fragmentprogram of this pass
 *  The value of the input-parameter must be given as a float-array of length N*M.
 *  (where 1<=N<=4 and 1<=M<=4)
 *  The entries in the array must be given in column-major order per default - can be
 *  given in row-major order by setting transpose=true.
 *  The size N and M of the matrix must ofcause match the dimensions of the input-parameters n and m!
 *
 *  @param[in] fpParameterName the name of the input-parameter in the fragment program
 *  @param[in] n the number of rows
 *  @param[in] m the number of columns
 *  @param[in] floatmatrix the vector with the matrix values
 *  @param[in] transpose whether the matrix is given in row-major order (default=false)
 *  @exception PPEResourceException if vector size N*M doesn't match n*m
 *  @exception PPEResourceException if the dimensions n or m are less than 1 or greater than 4.
 */
void PostProcessingPass::BindMatrix(string fpParameterName, int n, int m, vector<float> floatmatrix, const bool transpose) {
    fp->BindMatrix(fpParameterName, n, m, floatmatrix, transpose);
}

/** Bind values to a uniform array of matNxM to the fragmentprogram of this pass
 *
 *  @param[in] fpParameterName the name of the input-parameter in the fragment program
 *  @param[in] n the number of rows
 *  @param[in] m the number of columns
 *  @param[in] floatmatrices the vector with vectors of the matrix values
 *  @param[in] transpose whether the matrices is given in row-major order (default=false)
 *  @exception PPEResourceException if vector sizes N*M doesn't match n*m
 *  @exception PPEResourceException if the dimensions n or m are less than 1 or greater than 4.
 *  @exception PPEResourceException if not all matrices have same size.
 */
void PostProcessingPass::BindMatrix(string fpParameterName, int n, int m, vector<vector<float> > floatmatrices, const bool transpose) {
    fp->BindMatrix(fpParameterName, n, m, floatmatrices, transpose);
}

/** Bind a texture to a uniform sampler2D input-parameter of the fragmentprogram of this pass.
 *
 *  @param[in] fpParameterName the name of the input-parameter in the fragment program
 *  @param[in] inputTexture the texture to bind
 *  @exception if the smart-pointer inputTexture contains NULL
 */
void PostProcessingPass::BindTexture(string fpParameterName, ITextureResourcePtr inputTexture) {
    fp->BindTexture(fpParameterName, inputTexture);
}

/** Bind the color buffer to a uniform sampler2D input-parameter of the fragmentprogram of this pass.
 *  (it's the color buffer from the last pass that wrote to it, or the color buffer of the
 *  rendered screen, if no previous passes has written to it)
 *
 *  @param[in] fpParameterName the name of the input-parameter in the fragment program
 */
void PostProcessingPass::BindColorBuffer(string fpParameterName) {
    if (inputColorBufferParameterName == "") inputColorBufferParameterName = fpParameterName;
    else                                     throw new PostProcessingException("colorbuffer texture already assigned to an input parameter");
}

/** Bind the depth buffer to a uniform sampler2D input-parameter of the fragmentprogram of this pass.
 *  (it's the depth buffer from the last pass that wrote to it, or the depth buffer of the
 *  rendered screen, if no previous passes has written to it)
 *
 *  @param[in] fpParameterName the name of the input-parameter in the fragment program
 */
void PostProcessingPass::BindDepthBuffer(string fpParameterName) {
    if (inputDepthBufferParameterName == "") inputDepthBufferParameterName = fpParameterName;
    else                                     throw new PostProcessingException("depthbuffer texture already assigned to an input parameter");
}


/** Bind a userbuffer-texture from an earlier pass to a uniform sampler2D input-parameter of the fragmentprogram of this pass.
 *  The userbuffer-texture being bound, is the one from the given pass at the given attachment point of that pass.
 *  The given pass must be an earlier pass that this one, otherwise the output (ofcause) haven't been written to the userbuffer yet
 *  when this pass executes. Also, both passes must be from the same PostProcessingEffect.
 *
 *  @param[in] fpParameterName the name of the input-parameter in the fragment program
 *  @param[in] outputPass the pass with the wanted userbuffer
 *  @param[in] outputAttachmentPoint the attachment point at outputPass with the wanted userbuffer
 *  @exception PostProcessingException thrown if the outputPass doen't belong to the same PostProcessingEffect as this pass
 *  @exception PostProcessingException thrown if outputPass wasn't an earlier pass than this pass
 *  @exception PostProcessingException thrown if attachmentPoint >= PostProcessingEffect::getMaxColorAttachments().
 *  @exception PostProcessingException thrown if there were no userbuffer in outputPass at outputAttachmentPoint
 */
void PostProcessingPass::BindUserBuffer(string fpParameterName, IPostProcessingPass* outputPass, int outputAttachmentPoint) {
    if (this->ppe != ((PostProcessingPass*)outputPass)->ppe) throw new PostProcessingException("can only bind userbuffers from a pass belonging to the same PostProcessingEffect as this pass");
    if (this->passID <= ((PostProcessingPass*)outputPass)->passID) throw new PostProcessingException("can only bind userbuffers from passes executed earlier than this pass!");
    if (outputAttachmentPoint >= maxColorAttachments) throw new PostProcessingException("attachmentpoint too large (for this gfx card)");
    if (!(outputPass->IsUserBufferOutput(outputAttachmentPoint))) throw new PostProcessingException("there were no userbuffer for the outputpass at the attachmentpoint");
    //if (outputPass->userBufferTextures[outputAttachmentPoint] == NULL) throw new PostProcessingException("there were no userbuffer for the outputpass at the attachmentpoint");

    // bind the output texture to the input-parameter
    ITexture2DPtr outputTex = outputPass->GetUserBufferRef(outputAttachmentPoint);//outputPass->userBufferTextures[outputAttachmentPoint];
    this->BindTexture(fpParameterName, outputTex);
}


/** Specifies that the fragmentprogram of this pass writes output to the colorbuffer.
 *  If you want this pass to write its output to the colorbuffer, you must enable it using this method.
 *
 *  If you enable output to the color buffer for a pass, it will be attached as attachment 0 of the pass' FBO.
 *  This means that if you want to output to the color buffer, you can't have a userbuffer attached at attachmentpoint 0 as well.
 *
 *  @exception PostProcessingException if a userbuffer was already attached at attachment-point 0
 */
void PostProcessingPass::EnableColorBufferOutput() {
    // NOTE: the buffer is not attached here to the fbo for the pass, it's done in executePass(), since we don't know here which of
    //       the two ping-pong textures for the buffer that will be the one that must be attached.
    if (userBufferTextures[0].get() != NULL) throw new PostProcessingException("can't attach both colorbuffer and userbuffer at attachment-point 0");
    outputsToColorBuffer = true;
}

/** Specifies that the fragmentprogram of this pass writes output to the depthbuffer.
 *  If you want this pass to write its output to the depthbuffer, you must enable it using this method.
 */
void PostProcessingPass::EnableDepthBufferOutput() {
    // NOTE: the buffer is not attached here to the fbo for the pass, it's done in executePass(), since we don't know here which of
    //       the two ping-pong textures for the buffer that will be the one that must be attached.
    outputsToDepthBuffer = true;
}


/** Attach a userbuffer for the FBO at this pass at the given attachment point, which can be used by the fragment program to write output to.
 *  This output can be used as input for a later pass or returned to the application as a texture with the getOutputUserBufferTexture methods.
 *
 *  You can attach multiple userbuffers, at different attachment points, which allows the fragmentprogram to write to several buffers
 *  at the same time (MRT). The maximum number of simultaneous attachments, depends of the graphics card, and can be found by
 *  calling PostProcessingEffect::getMaxColorAttachments().
 *
 *  A userbuffer is a FBO colorbuffer-texture attachment (it is different from the regular colorbuffer, only in the way it's handled by
 *  the PostProcessingEffect class), which means that each texel is a float4.
 *
 *  If output to the (regular) colorbuffer has been enabled for this pass, it will be attached at attachment 0 of this pass' FBO.
 *  This means that you can't attach a userbuffer at attachment 0 as well.
 *
 *  @param[in] attachmentPoint the attachmentpoint to attach a userbuffer
 *  @param[in] createFloatTexture whether the texture should be floating-point
 *  @exception PostProcessingException thrown if attachmentPoint >= PostProcessingEffect::getMaxColorAttachments().
 *  @exception PostProcessingException thrown if attachmentPoint=0 and the pass also outputs to the colorbuffer
 *  @exception PostProcessingException thrown if there were already attached a userbuffer at the given attachmentPoint.
 */
void PostProcessingPass::AttachUserBuffer(int attachmentPoint, const bool createFloatTexture) {
    if (attachmentPoint >= maxColorAttachments) throw new PostProcessingException("attachmentpoint too large (for this gfx card)");
    if (attachmentPoint==0 && outputsToColorBuffer) throw new PostProcessingException("can't attach both colorbuffer and userbuffer at attachment-point 0");
    if (userBufferTextures[attachmentPoint].get() != NULL) throw new PostProcessingException("there were already a output-userbuffer for this pass at this attachmentpoint");

    // create a new color-texture (since we're using rextures, not renderbuffers)
    ITexture2DPtr tex;
    if (createFloatTexture) tex = ITexture2DPtr(new Texture2D(currScreenWidth, currScreenHeight, TEX_RGBA_FLOAT, TEX_CLAMP_TO_EDGE, TEX_CLAMP_TO_EDGE, TEX_LINEAR, TEX_LINEAR));
    else                    tex = ITexture2DPtr(new Texture2D(currScreenWidth, currScreenHeight, TEX_RGBA      , TEX_CLAMP_TO_EDGE, TEX_CLAMP_TO_EDGE, TEX_LINEAR, TEX_LINEAR));

    // store in at the pass under the correct attachment point
    userBufferTextures[attachmentPoint] = tex;

    // attach it to the fbo for the pass
    fbo->AttachColorTexture(tex, attachmentPoint);
}


/** returns a COPY of the texture for the userbuffer at the given attachment point.
 *
 *  @param[in] attachmentPoint the attachmentPoint
 *  @return the copy of the userbuffer texture
 *  @exception PostProcessingException thrown if attachmentPoint >= PostProcessingEffect::getMaxColorAttachments().
 *  @exception PostProcessingException thrown if there were no userbuffers at the attachment point.
 */
ITexture2DPtr PostProcessingPass::GetUserBuffer(int attachmentPoint) {
    if (attachmentPoint >= maxColorAttachments) throw new PostProcessingException("attachmentpoint too large (for this gfx card)");
    if (userBufferTextures[attachmentPoint].get() == NULL) throw new PostProcessingException("there were no userbuffer for this pass at this attachmentpoint");

    // lav en kopi af texturen
    ITexture2DPtr tex = userBufferTextures[attachmentPoint];
    return tex->Clone();
}

/** as above, but the user supplies the textureID and we don't create a new textureID.
 *  this is useful if f.e. the user gets a texture each frame, because with the above method he would also have to delete it
 *  each frame again, in order to not flood vram, and that is slow to do all the time.
 *
 *  @param[in] attachmentPoint the attachmentPoint
 *  @param[in] texCopy the texture to replace with a copy of the userbuffer
 *  @exception PostProcessingException thrown if attachmentPoint >= PostProcessingEffect::getMaxColorAttachments().
 *  @exception PostProcessingException thrown if there were no userbuffers at the attachment point.
 *  @exception PostProcessingException thrown if the shared-pointer texCopy contains NULL (done in copyColorTexture)
 */
void PostProcessingPass::GetUserBuffer(int attachmentPoint, ITexture2DPtr texCopy) {
    if (attachmentPoint >= maxColorAttachments) throw new PostProcessingException("attachmentpoint too large (for this gfx card)");
    if (userBufferTextures[attachmentPoint].get() == NULL) throw new PostProcessingException("there were no userbuffer for this pass at this attachmentpoint");

    // lav en kopi af texturen
    ITexture2DPtr tex = userBufferTextures[attachmentPoint];
    tex->Clone(texCopy);
}

/** as above but doesn't return a copy of the userbuffer-texture but the texture itself. (implemented for performance reasons only)
 *  Consider using getOutputUserBufferTexture instead (a bit slower due to the texture copying).
 *
 *  Warnings:
 *   - The content of the texture will change each frame (because the pass writes to it each frame)!! (so you can't save prev frames)
 *   - Don't alter the texture
 *
 *  @param[in] attachmentPoint the attachmentPoint
 *  @return the userbuffer texture
 *  @exception PostProcessingException thrown if attachmentPoint >= PostProcessingEffect::getMaxColorAttachments().
 *  @exception PostProcessingException thrown if there were no userbuffers at the attachment point.
 */
ITexture2DPtr PostProcessingPass::GetUserBufferRef(int attachmentPoint) {
    if (attachmentPoint >= maxColorAttachments) throw new PostProcessingException("attachmentpoint too large (for this gfx card)");
    if (userBufferTextures[attachmentPoint].get() == NULL) throw new PostProcessingException("there were no userbuffer for this pass at this attachmentpoint");
    return userBufferTextures[attachmentPoint];
}

/* execute this pass */
void PostProcessingPass::Execute(ITexture2DPtr texColorInput, ITexture2DPtr texColorOutput, ITexture2DPtr texDepthInput, ITexture2DPtr texDepthOutput, Viewport* viewport) { //int texSizeX, int texSizeY) {

    // attach the color- and depth-output textures to the fbo (color is attached at attachmentpoint 0)
    // (ONLY attach if the fp outputs to them, otherwise it'll be filled with crap! E.g. if fragprog doesn't write to depth, it will be the interpolated vertex-depths => constant values due to quad!!)
    // (another reason: if we attach the color-buffer while the user have his own userbuffer at attachment 0, hell will break loose)
    if (outputsToColorBuffer) fbo->AttachColorTexture(texColorOutput, 0);
    if (outputsToDepthBuffer) fbo->AttachDepthTexture(texDepthOutput);

    // enable MRT (always in the order 0,1,2,...,15 - otherwise it would be damn confusing)
    fbo->SelectDrawBuffers();

    // bind buffer-textures to input parameters
    if (inputColorBufferParameterName != "")
	fp->BindTexture(inputColorBufferParameterName, texColorInput);

    if (inputDepthBufferParameterName != "")
        fp->BindTexture(inputDepthBufferParameterName, texDepthInput);

    // bind fragment program for this pass
    fp->Bind();

    // bind fbo for this pass
    fbo->Bind();

    // set proper viewport and draw quad (which fills the entire fbo-screen)
    PostProcessingPass::SetProperViewport(viewport, true);
    PostProcessingPass::PerformGpuComputation(viewport);

    // unbind FBO again (no, no need to do it, and it is faster not to)
    //fbo->Unbind();
    glBindTexture(GL_TEXTURE_2D, 0);

    // check if something went completely wrong
    CheckGLErrors ("myCheck1!");

    // unbind fragment program again
    fp->Unbind();
}


/** @return wether this pass has been enabled to output to the color buffer
 */
bool PostProcessingPass::IsColorBufferOutput() {
    return outputsToColorBuffer;
}

/** @return wether this pass has been enabled to output to the depth buffer
 */
bool PostProcessingPass::IsDepthBufferOutput() {
    return outputsToDepthBuffer;
}

/** @return wether there is an (output) userbuffer attached at the given attachmentpoint
 */
bool PostProcessingPass::IsUserBufferOutput(int attachmentPoint) {
    return userBufferTextures[attachmentPoint].get() != NULL;
}


/* change size of FBO virtual screens (called by PostProcessingEffect when its resize-method is called) */
void PostProcessingPass::Resize(int currScreenWidth, int currScreenHeight) {
    this->currScreenWidth  = currScreenWidth;
    this->currScreenHeight = currScreenHeight;

    // resize alle userbuffers i dette pass
    for (int j=0; j<maxColorAttachments; j++) {
	ITexture2DPtr tex = userBufferTextures[j];
	//if (tex != NULL) tex->Resize(currScreenWidth, currScreenHeight);
	if (tex.get() != NULL) tex->Resize(currScreenWidth, currScreenHeight);
    }
}


/**
 * Checks for OpenGL errors.
 * Extremely useful debugging function: When developing,
 * make sure to call this after almost every GL call.
 */
void PostProcessingPass::CheckGLErrors (const char *label) {
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



/* Setup so that we can have One-to-one mapping from fragments (pixels) to texture coordinates */
/* For FBO'erne skal viewpoeren starte i (0,0)... dvs. (0,0,w,h). (since its buffer sizes is always (w,h)) For framebuffer (x,y,w,h) */
void PostProcessingPass::SetProperViewport(Viewport* viewport, bool fbo) {

    // viewport for 1:1 pixel=texel=geometry mapping
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluOrtho2D(0,
               viewport->GetDimension()[2],
               0,
               viewport->GetDimension()[3]);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glViewport(fbo ? 0 : viewport->GetDimension()[0],
               fbo ? 0 : viewport->GetDimension()[1],
               viewport->GetDimension()[2],
               viewport->GetDimension()[3]);
}

/* Perform the computation */
void PostProcessingPass::PerformGpuComputation(Viewport* viewport) {

    // make quad filled, not wireframe, to hit every pixel/texel (should be default but we never know)
    glPolygonMode(GL_FRONT,GL_FILL);
    // and render quad
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex2f(0.0                        , 0.0);
    glTexCoord2f(1.0, 0.0); glVertex2f(viewport->GetDimension()[2], 0.0);
    glTexCoord2f(1.0, 1.0); glVertex2f(viewport->GetDimension()[2], viewport->GetDimension()[3]);
    glTexCoord2f(0.0, 1.0); glVertex2f(0.0                        , viewport->GetDimension()[3]);
    glEnd();
}


/** Set wrap setting for a user-buffer
 *
 *  @param[in] attachmentPoint the attachmentPoint of the userbuffer
 *  @param[in] wrapS the wrap_s setting
 *  @param[in] wrapT the wrap_t setting
 *  @exception PostProcessingException thrown if attachmentPoint >= PostProcessingEffect::getMaxColorAttachments().
 *  @exception PostProcessingException thrown if there were no userbuffers at the attachment point.
 */
void PostProcessingPass::SetUserBufferWrap(int attachmentPoint, TextureWrap wrapS, TextureWrap wrapT) {
    if (attachmentPoint >= maxColorAttachments) throw new PostProcessingException("attachmentpoint too large (for this gfx card)");
    if (userBufferTextures[attachmentPoint].get() == NULL) throw new PostProcessingException("there were no userbuffer for this pass at this attachmentpoint");

    ITexture2DPtr tex = userBufferTextures[attachmentPoint];
    tex->SetWrapS(wrapS);
    tex->SetWrapT(wrapT);
}

/** Set wrap setting for a user-buffer
 *
 *  @param[in] attachmentPoint the attachmentPoint of the userbuffer
 *  @param[in] filter the filter setting
 *  @exception PostProcessingException thrown if attachmentPoint >= PostProcessingEffect::getMaxColorAttachments().
 *  @exception PostProcessingException thrown if there were no userbuffers at the attachment point.
 *  @note at the moment we have no use for setting mag or min filters seperately
 */
void PostProcessingPass::SetUserBufferFilter(int attachmentPoint, TextureFilter filter) {
    if (attachmentPoint >= maxColorAttachments) throw new PostProcessingException("attachmentpoint too large (for this gfx card)");
    if (userBufferTextures[attachmentPoint].get() == NULL) throw new PostProcessingException("there were no userbuffer for this pass at this attachmentpoint");

    ITexture2DPtr tex = userBufferTextures[attachmentPoint];
    tex->SetMagFilter(filter);
    tex->SetMinFilter(filter);
}

} // NS PostProcessing
} // NS OpenEngine
