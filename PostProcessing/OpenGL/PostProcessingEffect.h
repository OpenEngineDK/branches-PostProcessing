#ifndef __POSTPROCESSINGEFFECT_H__
#define __POSTPROCESSINGEFFECT_H__

#include <vector>
#include <string>

#include <PostProcessing/IPostProcessingEffect.h>
#include <PostProcessing/PostProcessingException.h>
#include <PostProcessing/OpenGL/PostProcessingPass.h>
#include <Resources/OpenGL/FragmentProgram.h>
#include <Resources/OpenGL/FramebufferObject.h>
#include <Resources/OpenGL/Texture2D.h>
#include <Resources/OpenGL/RenderBuffer.h>
#include <Display/Viewport.h>
#include <Logging/Logger.h>
#include <Core/IListener.h>     // to be able to add itself as a module
#include <Core/IEngine.h>
//#include <Core/IGameEngine.h> // to be able to add itself as a module
#include <Meta/OpenGL.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

namespace OpenEngine {
namespace PostProcessing {

using namespace OpenEngine::Resources;
using namespace OpenEngine::Display;
using namespace OpenEngine::Core;


/** objects of this class represents a post-processing effect
 *  @author Bjarke N. Laustsen
 */
class PostProcessingEffect : public IListener<ProcessEventArg>
    , public IPostProcessingEffect {

  private:
    IEngine& engine;

    // max antal color-attachments/texture-units dette grafikkort tillader
    GLint maxColorAttachments;
    GLint maxTextureUnits;

    // the fragment program for each pass (index 0 is the first pass, etc) (vektoren gemmer pointers for at undgå kopiering hele tiden)
    vector<PostProcessingPass*> passes;

    // the added PPEs of this PPE
    vector<PostProcessingEffect*> chainedEffects;
    int infLoopDetectionBit; // (pas på med uendelige loops - en effect må ikke være dens eget barn)

    // width, height of the screen and thus of our textures/renderbuffers
    int currScreenWidth;
    int currScreenHeight;
    Viewport* viewport;

    // whether to output to the screen or not (if you just want to render to texture)
    bool screenOutput;

    // whether the colorbuffer texture-attachments should be float-textures or not (userbuffer textures are specified individually)
    bool useFloatTextures;

    // whether this effect is completely disabled or not
    bool enabled;

    // whether Setup has been called yet
    bool satup;

    // ensures PerFrame is only called _after_ the effect has been executed, and only on frames it has been executed
    bool callPerFrame;

    // wether to keep stencil buffer attached when passes are executed
    //bool keepStencil;

    // Handles for FBO, FBO-Textures, Renderbuffers
    FramebufferObject* fbo; // <- fbo for "render user-screen" (the FBOs for the passes are in the PPEPass objects)
    ITexture2DPtr      colorTex1;
    ITexture2DPtr      colorTex2;
    ITexture2DPtr      depthTex1;
    ITexture2DPtr      depthTex2;
    //ITexture2DPtr      stencilTex;

    // used for restoring the fbo after postRender which was bound before preRender
    GLint savedFboID;

    void SetupFBO();  // create FBO, FBO-textures, renderbuffers

    void Swap(ITexture2DPtr* a, ITexture2DPtr* b); // swap values of a and b (just to make some code a bit prettier)

    ITexture2DPtr finalColorTex; // used by getFinalColorBufferTexture (if any effects are chained, it will be the result after those)
    ITexture2DPtr finalDepthTex; // used by getFinalDepthBufferTexture (if any effects are chained, it will be the result after those)


    ITexture2DPtr CreateColorTex();
    ITexture2DPtr CreateDepthTex();
    //ITexture2DPtr CreateDepthStencilTex();

    // private method used when chaining effects (remember private in C++ is only private to objects of other classes)
    void PreRender(bool bindFbo);
    void PostRender(ITexture2DPtr colorTex1, ITexture2DPtr depthTex1, bool output2screen);

    // misc
    void CheckGLErrors (const char *label);
    void CallSetup();
    void SetSameFilterWrap(ITexture2DPtr src, ITexture2DPtr dst);

    /* change size of FBO virtual screens (when resizing the viewport!) */
    void Resize(int currScreenWidth, int currScreenHeight);

  protected:

    /* assign fragment programs for the various passes */
    IPostProcessingPass* AddPass(string fpFileName); // returns an object used when assigning input/output-parameters
    IPostProcessingPass* AddPass(vector<string> fpFileNames);

  public:

    PostProcessingEffect(Viewport* viewport, IEngine& engine, const bool useFloatTextures = false);
    virtual ~PostProcessingEffect();

    void PreRender();  // call before rendering the screen (to setup FBO)
    void PostRender(); // call after  rendering the screen (to apply postprocessing effects to the rendered screen)

    /* return a COPY of the final color/depth-buffer texture */
    void GetFinalColorBuffer(ITexture2DPtr texCopy);
    void GetFinalDepthBuffer(ITexture2DPtr texCopy);

    ITexture2DPtr GetFinalColorBuffer();
    ITexture2DPtr GetFinalDepthBuffer();

    /* return a non-copy of the final color/depth-buffer texture */
    ITexture2DPtr GetFinalColorBufferRef();
    ITexture2DPtr GetFinalDepthBufferRef();

    /* wrap and filter settings for color/depth-buffer textures */
    void SetColorBufferWrap(TextureWrap wrapS, TextureWrap wrapT);
    void SetDepthBufferWrap(TextureWrap wrapS, TextureWrap wrapT);
    void SetColorBufferFilter(TextureFilter filter);
    void SetDepthBufferFilter(TextureFilter filter);

    /* get max number of fbo color-attachments or texture-units for this gfx-card */
    int GetMaxColorAttachments();
    int GetMaxTextureBindings();

    /* enable/disable final output to screen (in case you want to render to a texture only (for example a mirror)) - kan få texture med get-funktionerne */
    void EnableScreenOutput(bool enable);

    /* enable/disable this PPE completely (if disabled it will just output its input unchanged) */
    void Enable(bool enable);
    bool IsEnabled();

    /* attach stencil */
    //void EnableStencilBuffer();

    /* methods for chaining PPEs */
    void Add(IPostProcessingEffect* ppe);
    void Remove(IPostProcessingEffect* ppe);
    void RemoveAll();

    /* overwritable user-methods */
    virtual void Setup() = 0;
    virtual void PerFrame(const float deltaTime) = 0;

    Viewport* GetViewport();

    /* methods inherited from IModule */
    void Handle(ProcessEventArg arg);
};

} // NS PostProcessing
} // NS OpenEngine

#endif


