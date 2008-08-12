#ifndef __IPOSTPROCESSINGEFFECT_H__
#define __IPOSTPROCESSINGEFFECT_H__

#include <vector>
#include <string>

#include <Resources/ITexture2D.h>
#include <Resources/IRenderBuffer.h>
#include <PostProcessing/IPostProcessingPass.h>
#include <Display/Viewport.h>

namespace OpenEngine {
namespace PostProcessing {

using namespace OpenEngine::Resources;
using namespace OpenEngine::Display;
using namespace OpenEngine::Core;

/** interface for PostProcessingEffect
 *  @author Bjarke N. Laustsen
 */
class IPostProcessingEffect {

  protected:

    /* assign fragment programs for the various passes */
    virtual IPostProcessingPass* AddPass(string fpFileName) = 0;
    virtual IPostProcessingPass* AddPass(vector<string> fpFileNames) = 0;

  public:

    virtual ~IPostProcessingEffect() {}

    /* call before rendering the screen (to setup FBO) */
    virtual void PreRender() = 0;

    /* call after  rendering the screen (to apply postprocessing effects to the rendered screen) */
    virtual void PostRender() = 0;

    /* return a COPY of the final color/depth-buffer texture */
    virtual void GetFinalColorBuffer(ITexture2DPtr texCopy) = 0;
    virtual void GetFinalDepthBuffer(ITexture2DPtr texCopy) = 0;

    virtual ITexture2DPtr GetFinalColorBuffer() = 0;
    virtual ITexture2DPtr GetFinalDepthBuffer() = 0;

    /* return a non-copy of the final color/depth-buffer texture */
    virtual ITexture2DPtr GetFinalColorBufferRef() = 0;
    virtual ITexture2DPtr GetFinalDepthBufferRef() = 0;

    /* wrap and filter settings for color/depth-buffer textures */
    virtual void SetColorBufferWrap(TextureWrap wrapS, TextureWrap wrapT) = 0;
    virtual void SetDepthBufferWrap(TextureWrap wrapS, TextureWrap wrapT) = 0;
    virtual void SetColorBufferFilter(TextureFilter filter) = 0;
    virtual void SetDepthBufferFilter(TextureFilter filter) = 0;

    /* get max number of fbo color-attachments or texture-units for this gfx-card */
    virtual int GetMaxColorAttachments() = 0;
    virtual int GetMaxTextureBindings() = 0;

    /* enable/disable final output to screen (in case you want to render to a texture only (for example a mirror)) - kan få texture med get-funktionerne */
    virtual void EnableScreenOutput(bool enable) = 0;

    /* enable/disable this PPE completely (if disabled it will just output its input unchanged) */
    virtual void Enable(bool enable) = 0;
    virtual bool IsEnabled() = 0;

    /* methods for chaining PPEs */
    virtual void Add(IPostProcessingEffect* ppe) = 0;
    virtual void Remove(IPostProcessingEffect* ppe) = 0;
    virtual void RemoveAll() = 0;

    /* overwritable user-methods */ // PerFrame will be called AFTER each frame where this ppe was active! (also if disabled?)
    virtual void Setup() = 0;
    virtual void PerFrame(const float deltaTime) = 0;

    /* get viewport */
    virtual Viewport* GetViewport() = 0;
};

} // NS PostProcessing
} // NS OpenEngine

#endif


