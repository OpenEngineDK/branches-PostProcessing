#include "PostProcessingEffect.h"

/* @author Bjarke N. Laustsen
 */
namespace OpenEngine {
namespace PostProcessing {

/** Creates a new PostProcessingEffect object.
 *  Since the size of its FBO-buffers must match the size of the viewport, the viewport must be passed along as arguments.
 *  (if the size of the viewport changes the FBO-buffers are automatically resized)
 *  @param[in] viewport the viewport this effects should be applied to
 *  @param[in] useFloatTextures whether the colorbuffer textures should be floating-point
 */
    PostProcessingEffect::PostProcessingEffect(Viewport* viewport, IEngine& engine, const bool useFloatTextures) : engine(engine) {
    this->viewport         = viewport;
    this->currScreenWidth  = viewport->GetDimension()[2];
    this->currScreenHeight = viewport->GetDimension()[3];

    this->fbo       = NULL;
    this->depthTex1.reset();
    this->depthTex2.reset();
    this->colorTex1.reset();
    this->colorTex2.reset();
    //this->stencilTex.reset();

    this->infLoopDetectionBit = 0;

    this->finalColorTex.reset();
    this->finalDepthTex.reset();

    this->screenOutput = true;
    this->enabled = true;

    this->useFloatTextures = useFloatTextures;

    this->savedFboID = 0;

    this->satup = false;
    this->callPerFrame = false;

    this->maxColorAttachments = -1; // can't be queried yet, as OpenGL might not been initialized at this point
    this->maxTextureUnits = -1; // can't be queried yet, as OpenGL might not been initialized at this point

    //glGetIntegerv(GL_MAX_DRAW_BUFFERS, &(this->maxColorAttachments));
    //SetupFBO();

    this->finalColorTex.reset();
    this->finalDepthTex.reset();

    // register this object as a module
    engine.ProcessEvent().Attach(*this);
}

/** Cleans up
 */
PostProcessingEffect::~PostProcessingEffect() {
    // delete fbo, fbo-textures, fbo-renderbuffer
    delete fbo;

    // delete all passes (fragment programs, userbuffers, etc)
    for (unsigned int i=0; i<passes.size(); i++)
        delete passes.at(i);

    // unregister this object as a module
    engine.ProcessEvent().Detach(*this);
}


// create FBO, FBO-textures, renderbuffers (also called on screen-resize to resize textures and renderbuffers (<- NO!! NOT ANYMORE!))
void PostProcessingEffect::SetupFBO() {

    glGetIntegerv(GL_MAX_DRAW_BUFFERS, &(this->maxColorAttachments));
    glGetIntegerv(GL_MAX_TEXTURE_UNITS, &(this->maxTextureUnits));

    fbo       = new FramebufferObject(); // <- the fbo used for "render user-screen" (not for the passes)
    depthTex1 = CreateDepthTex();
    depthTex2 = CreateDepthTex();
    colorTex1 = CreateColorTex();
    colorTex2 = CreateColorTex();

    fbo->AttachColorTexture(colorTex1, 0);
    fbo->AttachDepthTexture(depthTex1);

    fbo->SelectDrawBuffers();
}


ITexture2DPtr PostProcessingEffect::CreateColorTex() {
    ITexture2DPtr tex;
    if (useFloatTextures)
	tex = ITexture2DPtr(new Texture2D(currScreenWidth, currScreenHeight, TEX_RGBA_FLOAT, TEX_CLAMP_TO_EDGE, TEX_CLAMP_TO_EDGE, TEX_LINEAR, TEX_LINEAR));
    else
	tex = ITexture2DPtr(new Texture2D(currScreenWidth, currScreenHeight, TEX_RGBA      , TEX_CLAMP_TO_EDGE, TEX_CLAMP_TO_EDGE, TEX_LINEAR, TEX_LINEAR));
    return tex;
}

ITexture2DPtr PostProcessingEffect::CreateDepthTex() {
    return ITexture2DPtr(new Texture2D(currScreenWidth, currScreenHeight, TEX_DEPTH, TEX_CLAMP_TO_EDGE, TEX_CLAMP_TO_EDGE, TEX_NEAREST, TEX_NEAREST));
}

/*
ITexture2DPtr PostProcessingEffect::CreateDepthStencilTex() {
    return ITexture2DPtr(new Texture2D(currScreenWidth, currScreenHeight, TEX_DEPTH_STENCIL, TEX_REPEAT, TEX_REPEAT, TEX_NEAREST, TEX_NEAREST));
}
*/

/** Call before rendering the screen (to bind FBO, resize if viewport has been resized and setup everything on the first frame)
 *
 *  Its main purpose is to bind the FBO which the user-screen rendering is 'recorded' to, but is also does other setup stuff.
 *  It calls Resize if the viewport has been resized, and calls PreRender on all chained effects (their FBOs are not bound, however),
 *  to setup them, and a few other settings.
 *  On the first frame it calls the setup method. (this is done here instead of in the constructor, as OpenEngine doesn't setup
 *  OpenGL before very late in the startup process)
 *
 *  @exception PostProcessingException thrown if this PostProcessingEffect has been chained as child of itself (see the Add method)
 */
void PostProcessingEffect::PreRender() {
    PreRender(true);
}

void PostProcessingEffect::PreRender(bool bindFbo) {

    // check for chain inf-loop
    if (infLoopDetectionBit == 1) throw new PostProcessingException("chain inf-loop detected");

    // setup on first frame on this PPE and all chained PPEs (also new PPEs which might have been added since last frame)
    CallSetup();


     // check if viewport has been resized. If so, resize all buffers (incl. chained)
    if (viewport->GetDimension()[2] != currScreenWidth || viewport->GetDimension()[3] != currScreenHeight)
        Resize(viewport->GetDimension()[2], viewport->GetDimension()[3]);

    // don't need to clear normal framebuffer buffers, since they will completely be overwritten (faster!)

    // if any PPEs are chained to this one, call PerFrame on them as well (but don't bind their fbo) (must be done after CallSetup())
    for (unsigned int i=0; i<chainedEffects.size(); i++) {
	PostProcessingEffect* ppe = chainedEffects.at(i);
	ppe->PreRender(false);
    }

    // bind the fbo the userscreen should be rendered to
    if (bindFbo) {

	// remember the currently bound fbo, so we can restore it again after postRender (can't be done with pushAttrib())
      if (savedFboID != 0) throw new PostProcessingException("internal error");
	glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, &savedFboID);

	// bind fbo for "render user-screen"
	fbo->Bind();

	// clear done here since its not done in rendering view, and has to be done _after_ the fbo is bound.
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    }

    // check if all went well
    CheckGLErrors("preRender");
}

/** Call after rendering the screen (to apply postprocessing effects to the rendered screen)
 */
void PostProcessingEffect::PostRender() {
    /* NOTE! The pushing of matrix stacks is done here (instead of in the method below), for speed and since it could otherwise
     * EASILY overflow the projection matrix stack, if several PPEs are chained. (it happened to me with as few as 4 chained PPEs!)
     * It turns out that the size of the projection stack is very limited. See (4) at: http://www.opengl.org/resources/features/KilgardTechniques/oglpitfall/
     * For speed, the rest of the state that we need to backup is also backedup here (reason for backup: to not confuse the user by messing up his OpenGL state)
     * (This is also the right place to do it, since we don't need to backup stuff from a PPE to a chained PPE)
     */

    CheckGLErrors("postRender (early)");  

    /*** backup user OpenGL-state etc ***/

    // disable fbo again - we have now rendered the screen
    fbo->Unbind();

    // since we change the viewport, we need to restore it to what it were later. We also need to restore all other state changes we make, like enabling/disabling
    // projection and modelview matrices are also altered
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    // unbind any textures that may have been bound
    glBindTexture(GL_TEXTURE_2D, 0);

    glEnable(GL_TEXTURE_2D);   // <- vores final quad skal have texture på (de andre klares af fragment programmerne)
    glDisable(GL_LIGHTING);    // <- eventuelle lyskilder skal ikke ændre farverne af vores quad
    glEnable(GL_DEPTH_TEST);   // <- vi skal slå depth test fra, ellers kan vi ikke tegne quaden samme sted uden at cleare først (langsomt i forhold til ikke at gøre det)
    glDepthFunc(GL_ALWAYS);    //    MEN! hvis vi slår den fra, slår vi også depth-writes fra, og så kan vi ikke skrive til depth-værdier fra fragprog! Så derfor sættes depth-testen istedet til altid at lade pixels passere. (http://www.gamedev.net/community/forums/topic.asp?topic_id=342586&whichpage=1&#2236357)
    glDisable(GL_ALPHA_TEST);  // <- behøves ikke (og speeder lidt op iflg. http://www.gamedev.net/community/forums/topic.asp?topic_id=277122)
    glDisable(GL_TEXTURE_CUBE_MAP);
    glDisable(GL_TEXTURE_1D);
    glDisable(GL_TEXTURE_3D);
    glDisable(GL_TEXTURE_RECTANGLE_EXT);

    /*** do the postprocessing! ***/
    PostRender(colorTex1, depthTex1, screenOutput);

    /*** restore user OpenGL-state ***/

    // restore viewport setup, enabling/disabling, etc. Also restore projection and modelview matrices. And FBO.
    glPopAttrib();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, (GLuint)savedFboID);
    savedFboID = 0;
}

// postRender er i 2 funktioner for at kunne sende depth-info med fra ppe til chained-ppe
void PostProcessingEffect::PostRender(ITexture2DPtr colorTex1Param, ITexture2DPtr depthTex1Param, bool output2screen) {
    infLoopDetectionBit = 1;

    // bugfix since project hand-in: make sure colorTex1Param, depthTex1Param has same wrap/filter settings as colorTex1, depthTex1
    // (otherwise wrap/filter-settings won't work for chained effects. This fix won't be needed in V2.)
    SetSameFilterWrap(colorTex1, colorTex1Param);
    SetSameFilterWrap(depthTex1, depthTex1Param);

    // execute each pass (swap textures roles as input/output`- the "ping pong" technique)
    ITexture2DPtr inputColorTex  = colorTex1Param;
    ITexture2DPtr outputColorTex = colorTex2;
    ITexture2DPtr inputDepthTex  = depthTex1Param;
    ITexture2DPtr outputDepthTex = depthTex2;

    if (enabled) for (unsigned int i=0; i<passes.size(); i++) {
	PostProcessingPass* pass = passes.at(i);

	// execute the pass
	pass->Execute(inputColorTex, outputColorTex, inputDepthTex, outputDepthTex, viewport); //currScreenWidth, currScreenHeight);

	// if the fp of this pass is writing to the color-buffer, swap input/output textures AFTER executing it. Same for depth-buffer. (must be done AFTER!! see old bug in main.cpp)
	if (pass->IsColorBufferOutput()) Swap(&inputColorTex, &outputColorTex);
	if (pass->IsDepthBufferOutput()) Swap(&inputDepthTex, &outputDepthTex);
    }

    // swap a final time to get correct final output textures
    Swap(&inputColorTex, &outputColorTex);
    Swap(&inputDepthTex, &outputDepthTex);

    // if any PPEs are chained to this one, execute them, and get the final color and depth texture of the last PPE
    for (unsigned int i=0; i<chainedEffects.size(); i++) {
	PostProcessingEffect* ppe = chainedEffects.at(i);
	ppe->PostRender(outputColorTex, outputDepthTex, false); // false, so that the chained ppes doesn't output to screen, just to texture
	outputColorTex = ppe->GetFinalColorBufferRef();
	outputDepthTex = ppe->GetFinalDepthBufferRef();
    }

    // unbind any fbos
    fbo->Unbind();

    // før vi rendere til screenen skal vi lige vælge den rigtige buffers i den normale framebuffer vi vil skrive til (ellers giver det GL_INVALID_OPERATION-fejl, når vi bruger glDrawBuffers i executePass() til MRT)
    glDrawBuffer(GL_BACK);

    // render the final output color texture to screen, if output to screen is enabled
    if (output2screen) {
	glColor3f(1,1,1);
	PostProcessingPass::SetProperViewport(viewport, false);
	outputColorTex->Bind();
	PostProcessingPass::PerformGpuComputation(viewport);
	outputColorTex->Unbind();
    }

    // used by getColorbuffer and getDepthbuffer
    this->finalColorTex = outputColorTex;
    this->finalDepthTex = outputDepthTex;

    // flag that the PerFrame method of this effect should be called
    callPerFrame = true;

    infLoopDetectionBit = 0;

    CheckGLErrors ("postRender");
}

/** Add a pass to this PostProcessingEffect. (The passes will be executed in the order they are added)
 *  The method will return a PostProcessingPass-object representing the pass.
 *  You must call methods of the returned object to setup the pass (set input parameters, output buffers, etc).
 *
 *  @param[in] fpFileName the filename of file containing the fragmentprogram
 *  @return the PostProcessingPass-object corresponding to this pass
 */
IPostProcessingPass* PostProcessingEffect::AddPass(string fpFileName) {
    vector<string> fpFileNames;
    fpFileNames.push_back(fpFileName);
    return AddPass(fpFileNames);
}

/** As above, but with the fragment program spread across several files
 *
 *  @param[in] fpFileNames the set of the filenames containing the fragmentprogram
 *  @return the PostProcessingPass-object corresponding to this pass
 */
IPostProcessingPass* PostProcessingEffect::AddPass(vector<string> fpFileNames) {
    if (!satup) throw new PostProcessingException("method AddPass called before setup");

    glPushAttrib(GL_ALL_ATTRIB_BITS);

    int index = passes.size();
    PostProcessingPass* pass = new PostProcessingPass(fpFileNames, currScreenWidth, currScreenHeight, index, this);
    passes.push_back(pass);

    glPopAttrib();
    return pass;
}


/** Resizes the FBO virtual screens.
 *  When the viewport is resize, this method is called, as all FBO virtual screens must be resized as well, to match
 *  the current size of the viewport.
 *
 *  @param[in] currScreenWidth the new width of the screen
 *  @param[in] currScreenHeight the new height of the screen
 */
void PostProcessingEffect::Resize(int currScreenWidth, int currScreenHeight) {
    if (!satup) throw new PostProcessingException("method Resize called before setup");

    glPushAttrib(GL_ALL_ATTRIB_BITS);

    this->currScreenWidth  = currScreenWidth;
    this->currScreenHeight = currScreenHeight;

    // resize vores color/depth textures and stencil renderbuffer.
    depthTex1->Resize(currScreenWidth, currScreenHeight);
    depthTex2->Resize(currScreenWidth, currScreenHeight);
    colorTex1->Resize(currScreenWidth, currScreenHeight);
    colorTex2->Resize(currScreenWidth, currScreenHeight);
    //stencilTex->Resize(currScreenWidth, currScreenHeight);

    // resize alle userbuffers
    for (unsigned int i=0; i<passes.size(); i++) {
	PostProcessingPass* pass = passes.at(i);
	pass->Resize(currScreenWidth, currScreenHeight);
    }

    glPopAttrib();
}

// swap values of a and b (just to make some code a bit prettier)
void PostProcessingEffect::Swap(ITexture2DPtr* a, ITexture2DPtr* b) {
    ITexture2DPtr tmp = *a;
    *a = *b;
    *b = tmp;
}

/** Returns a COPY of the final color buffer texture (user supplies output-texture id).
 *  By final i mean final : if any ppes are chained to this one, then it's the result after all of those has been executed
 *
 *  @param[in] texCopy the texture to copy the final colorbuffer to (the textures parameters might be changed)
 *  @exception PostProcessingException if texCopy contains NULL
 *  @exception PostProcessingException if called before the first frame
 */
void PostProcessingEffect::GetFinalColorBuffer(ITexture2DPtr texCopy) {
    if (!satup) throw new PostProcessingException("method GetFinalColorBuffer called before setup");
    if (texCopy.get() == NULL || finalColorTex.get() == NULL) throw new PostProcessingException("can't be called before first frame");
    // note: this will work for chained as well, since finalColorTexID contains the final one after all chained effects
    finalColorTex->Clone(texCopy);
}

/** Returns a COPY of the final depth buffer texture (user supplies output-texture id)
 *  By final i mean final : if any ppes are chained to this one, then it's the result after all of those has been executed
 *
 *  @param[in] texCopy the texture to copy the final depthbuffer to (the textures parameters might be changed)
 *  @exception PostProcessingException if texCopy contains NULL
 *  @exception PostProcessingException if called before the first frame
 */
void PostProcessingEffect::GetFinalDepthBuffer(ITexture2DPtr texCopy) {
    if (!satup) throw new PostProcessingException("method GetFinalDepthBuffer called before setup");
    if (texCopy.get() == NULL || finalDepthTex.get() == NULL) throw new PostProcessingException("can't be called before first frame");
    // note: this will work for chained as well, since finalDepthTexID contains the final one after all chained effects
    finalDepthTex->Clone(texCopy);
}

/** Returns a COPY of the final color buffer texture
 *  By final i mean final : if any ppes are chained to this one, then it's the result after all of those has been executed
 *
 *  @return the copy of the final colorbuffer
 *  @exception PostProcessingException if called before the first frame
 */
ITexture2DPtr PostProcessingEffect::GetFinalColorBuffer() {
    if (!satup) throw new PostProcessingException("method GetFinalColorBuffer called before setup");
    if (finalColorTex.get() == NULL) throw new PostProcessingException("can't be called before first frame");
    // note: this will work for chained as well, since finalColorTexID contains the final one after all chained effects
    return finalColorTex->Clone();
}

/** Returns a COPY of the final depth buffer texture
 *  By final i mean final : if any ppes are chained to this one, then it's the result after all of those has been executed
 *
 *  @return the copy of the final depthbuffer
 *  @exception PostProcessingException if called before the first frame
 */
ITexture2DPtr PostProcessingEffect::GetFinalDepthBuffer() {
    if (!satup) throw new PostProcessingException("method GetFinalDepthBuffer called before setup");
    if (finalDepthTex.get() == NULL) throw new PostProcessingException("can't be called before first frame");
    // note: this will work for chained as well, since finalDepthTexID contains the final one after all chained effects
    return finalDepthTex->Clone();
}

/** As above but doesn't return a copy of the colorbuffer-texture but the texture itself. (implemented for performance reasons only)
 *  Consider using GetFinalColorBuffer instead (a bit slower due to the texture copying).
 *
 *  Warnings:
 *   - The content of the texture will change each frame (because the pass writes to it each frame)!! (so you can't save prev frames)
 *   - Don't modify it
 *   - (You should call it each frame you need it as it's not guaranteed that it's the same texture every time)
 *
 *  @return the colorbuffer texture
 *  @exception PostProcessingException if called before the first frame
 */
ITexture2DPtr PostProcessingEffect::GetFinalColorBufferRef() {
    if (!satup) throw new PostProcessingException("method GetFinalColorBufferRef called before setup");
    if (finalColorTex.get() == NULL) throw new PostProcessingException("can't be called before first frame");
    // note: this will work for chained as well, since finalColorTexID contains the final one after all chained effects
    return finalColorTex;
}

/** As above but doesn't return a copy of the depthbuffer-texture but the texture itself. (implemented for performance reasons only)
 *  Consider using GetFinalDepthBuffer instead (a bit slower due to the texture copying).
 *
 *  Warnings:
 *   - The content of the texture will change each frame (because the pass writes to it each frame)!! (so you can't save prev frames)
 *   - Don't modify it
 *   - (You should call it each frame you need it as it's not guaranteed that it's the same texture every time)
 *
 *  @return the depthbuffer texture
 *  @exception PostProcessingException if called before the first frame
 */
ITexture2DPtr PostProcessingEffect::GetFinalDepthBufferRef() {
    if (!satup) throw new PostProcessingException("method GetFinalDepthBufferRef called before setup");
    if (finalDepthTex.get() == NULL) throw new PostProcessingException("can't be called before first frame");
    // note: this will work for chained as well, since finalDepthTexID contains the final one after all chained effects
    return finalDepthTex;
}

/** Enable/disable final output to screen
 *  (in case you want to render to a texture only (for example a mirror (or shadowmapping)) instead of rendering to the entire screen.
 *   If you want to render to a texture only, you can get the texture with getFinalColorBufferTexture.)
 *  (also works for chained PPEs - it makes no difference enabling/disabling output for other PPEs than the one(s) you call preRender/postRender on)
 *
 *  @param[in] enable wether output to the screen should be enabled or disabled
 */
void PostProcessingEffect::EnableScreenOutput(bool enable) {
    screenOutput = enable;
}

/** Get max number of fbo color-attachments for this gfx-card
 *  @return da numbah
 */
int PostProcessingEffect::GetMaxColorAttachments() {
    if (!satup) throw new PostProcessingException("method GetMaxColorAttachments called before setup");
    return maxColorAttachments;
}

/** Get max number of fbo color-attachments for this gfx-card
 *  @return da numbah
 */
int PostProcessingEffect::GetMaxTextureBindings() {
    if (!satup) throw new PostProcessingException("method GetMaxTextureBindings() called before setup");
    return maxTextureUnits;
}

/** Enable/disable this PostProcessingEffect completely.
 *  (if disabled it will just output its input unmodified)
 *  @param[in] enable wether to enable or disable this PostProcessingEffect
 */
void PostProcessingEffect::Enable(bool enable) {
    this->enabled = enable;
}

/** Return wether this effect is enabled
 *  @return if this effect is enabled
 */
bool PostProcessingEffect::IsEnabled() {
    return enabled;
}

/** Add a PostProcessingEffect to be executed after this PostProcessingEffect.
 *  (The added PostProcessingEffects will be executed after this PostProcessingEffect and in the order they are added)
 *
 *  Using this method you can setup that several effects should execute after eachother.
 *  You can always achieve the same effect, by adding the passes of the second  to this effect (except floatbuffers), so this brings no new power.
 *  This is only useful to seperate effects into logical groups (seperate effects instead of all effects in the same effect), or
 *  if you want to enable/disable some effects at runtime in an easy way.
 *
 *  If any of the effects you add, also have added effects, they will also be executed. This means that you can setup your effects in a
 *  tree structure.
 *  Note: you shouldn't add a effect to itself or to one of its children. This would correspond to an infinite loop in the tree.
 *        If you do so, the preRender or resize method will cast an exception the next time it is called.
 *
 *  (note: at the time of writing, having several effects will take more vram (because of the textures) than having just one big effect)
 *
 *  @param[in] ppe the effect to be added to this one
 */
void PostProcessingEffect::Add(IPostProcessingEffect* ppe) {
    chainedEffects.push_back((PostProcessingEffect*)ppe);
}

/** Remove all occurances of the given PostProcessingEffect from this PostProcessingEffect
 *  @param[in] ppe the effect to remove
 *  (måske lave sådan at der er mulighed for at kun een forekomst bliver fjernet?)
 */
void PostProcessingEffect::Remove(IPostProcessingEffect* ppe) {
    for (vector<PostProcessingEffect*>::iterator it = chainedEffects.begin(); it != chainedEffects.end();) {
	PostProcessingEffect* ppe2 = *it;
	if (ppe == ppe2) it = chainedEffects.erase(it);
	else             it++;
    }
}

/** Remove all added PostProcessingEffects from this PostProcessingEffect
 */
void PostProcessingEffect::RemoveAll() {
    chainedEffects.clear();
}

/**
 * Checks for OpenGL errors.
 * Extremely useful debugging function: When developing,
 * make sure to call this after almost every GL call.
 */
void PostProcessingEffect::CheckGLErrors (const char *label) {
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

// call Setup on this PEE and all chained PPEs
// (also new PPEs which might have been added since last frame)
void PostProcessingEffect::CallSetup() {
    if (!satup) {
	satup = true;
	SetupFBO();
	Setup();
    }
}


/** Set wrap setting for the color-buffer of this effect
 *
 *  @param[in] wrapS the wrap_s setting
 *  @param[in] wrapT the wrap_t setting
 *  @exception PostProcessingException thrown if called before first frame
 */
void PostProcessingEffect::SetColorBufferWrap(TextureWrap wrapS, TextureWrap wrapT) {
    if (!satup) throw new PostProcessingException("method SetColorBufferWrap called before setup");
    colorTex1->SetWrapS(wrapS);
    colorTex1->SetWrapT(wrapT);
    colorTex2->SetWrapS(wrapS);
    colorTex2->SetWrapT(wrapT);
}

/** Set wrap setting for the depth-buffer of this effect
 *
 *  @param[in] wrapS the wrap_s setting
 *  @param[in] wrapT the wrap_t setting
 *  @exception PostProcessingException thrown if called before first frame
 */
void PostProcessingEffect::SetDepthBufferWrap(TextureWrap wrapS, TextureWrap wrapT) {
    if (!satup) throw new PostProcessingException("method SetDepthBufferWrap called before setup");
    depthTex1->SetWrapS(wrapS);
    depthTex1->SetWrapT(wrapT);
    depthTex2->SetWrapS(wrapS);
    depthTex2->SetWrapT(wrapT);
}

/** Set filter setting for the color-buffer of this effect
 *
 *  @param[in] filter the filter setting
 *  @exception PostProcessingException thrown if called before first frame
 *  @note at the moment we have no use for setting mag or min filters seperately
 */
void PostProcessingEffect::SetColorBufferFilter(TextureFilter filter) {
    if (!satup) throw new PostProcessingException("method SetColorBufferFilter called before setup");
    colorTex1->SetMagFilter(filter);
    colorTex1->SetMinFilter(filter);
    colorTex2->SetMagFilter(filter);
    colorTex2->SetMinFilter(filter);
}

/** Set filter setting for the color-buffer of this effect
 *
 *  @param[in] filter the filter setting
 *  @exception PostProcessingException thrown if called before first frame
 *  @note at the moment we have no use for setting mag or min filters seperately
 */
void PostProcessingEffect::SetDepthBufferFilter(TextureFilter filter) {
    if (!satup) throw new PostProcessingException("method SetDepthBufferFilter called before setup");
    depthTex1->SetMagFilter(filter);
    depthTex1->SetMinFilter(filter);
    depthTex2->SetMagFilter(filter);
    depthTex2->SetMinFilter(filter);
}

/* used by "bugfix since hand-in": make sure dst has same filter/wrap settings as src */
void PostProcessingEffect::SetSameFilterWrap(ITexture2DPtr src, ITexture2DPtr dst) {
    if (dst->GetWrapS()     != src->GetWrapS()    ) dst->SetWrapS    (src->GetWrapS());
    if (dst->GetWrapT()     != src->GetWrapT()    ) dst->SetWrapT    (src->GetWrapT());
    if (dst->GetMagFilter() != src->GetMagFilter()) dst->SetMagFilter(src->GetMagFilter());
    if (dst->GetMinFilter() != src->GetMinFilter()) dst->SetMinFilter(src->GetMinFilter());
}

/*
bool PostProcessingEffect::IsSatup() {
    return satup;
}

void PostProcessingEffect::ForceSetup() {
    CallSetup();
}
*/

/** Get viewport
 */
Viewport* PostProcessingEffect::GetViewport() {
    return viewport;
}


// only call PerFrame AFTER drawing a frame and ONLY on the frames this PostProcessingEffect actually were used on the frame
    void PostProcessingEffect::Handle(ProcessEventArg arg) {
    float deltaTime = arg.approx / 1000.0f;
    if (callPerFrame) PerFrame(deltaTime);
    callPerFrame = false;
}



} // NS PostProcessing
} // NS OpenEngine
