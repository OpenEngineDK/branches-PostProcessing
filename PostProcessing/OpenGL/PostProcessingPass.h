#ifndef __POSTPROCESSINGPASS_H__
#define __POSTPROCESSINGPASS_H__

#include <iostream>
#include <string>
#include <vector>

#include <PostProcessing/IPostProcessingEffect.h>
#include <PostProcessing/IPostProcessingPass.h>
#include <Resources/ITextureResource.h>
#include <PostProcessing/PostProcessingException.h>
#include <Resources/OpenGL/FragmentProgram.h>
#include <Resources/OpenGL/FramebufferObject.h>
#include <Resources/OpenGL/Texture2D.h>
#include <Resources/OpenGL/RenderBuffer.h>
#include <Display/Viewport.h>
#include <Logging/Logger.h>

namespace OpenEngine {
namespace PostProcessing {

using namespace OpenEngine::Resources;
using namespace OpenEngine::Display; // for viewport
using namespace OpenEngine::Resources; // for ITextureResourcePtr

class IPostProcessingEffect;
class PostProcessingEffect;

/** Objects of this class represents a pass.
 *  @author Bjarke N. Laustsen
 */
class PostProcessingPass : public IPostProcessingPass {

  private:

    int  passID;                  // used for error-checking in bindUserBuffer()
    IPostProcessingEffect* ppe;   // used for error-checking in bindUserBuffer()

    int maxColorAttachments;
    int currScreenWidth;
    int currScreenHeight;

    FragmentProgram* fp; // the fragment program assigned to this pass

    FramebufferObject* fbo; // the fbo assigned to this pass

    string inputColorBufferParameterName; // the input-parametername assigned to the colorbuffer-texture ("" if no parameters are assigned)
    string inputDepthBufferParameterName; // the input-parametername assigned to the depthbuffer-texture ("" if no parameters are assigned)

    bool outputsToColorBuffer; // if the fragment programmet writes to the  colorbuffer (gl_FragColor, eller gl_FragData[0]).
    bool outputsToDepthBuffer; // if the fragment programmet writes to the  depthbufferen (gl_FragDepth)

    vector<ITexture2DPtr> userBufferTextures; // textures for each attachment point

    /** private methods which are only accessible from PostProcessingEffect (*not* accessible to the user) */

    friend class PostProcessingEffect;
    PostProcessingPass(vector<string> fpFileNames, int currScreenWidth, int currScreenHeight, int passID, IPostProcessingEffect* ppe);
    virtual ~PostProcessingPass();
    PostProcessingPass() {}

    /* resize all userbuffers for this pass (must not be called by user) */
    void Resize(int currScreenWidth, int currScreenHeight);

    /* execute this pass (must not be called by user) */
    void Execute(ITexture2DPtr texColorInput, ITexture2DPtr texColorOutput, ITexture2DPtr texDepthInput, ITexture2DPtr texDepthOutputID, Viewport* viewport);//, int texSizeX, int texSizeY); // execute a pass

    void CheckGLErrors (const char *label);

    static void SetProperViewport(Viewport* viewport, bool fbo);
    static void PerformGpuComputation(Viewport* viewport);

  public:

    /* assign fragment program input parameters */
    void BindInt         (string fpParameterName, vector<int> intvector);
    void BindInt         (string fpParameterName, vector<vector<int> > intvectors);
    void BindFloat       (string fpParameterName, vector<float> floatvector);
    void BindFloat       (string fpParameterName, vector<vector<float> > floatvectors);
    void BindMatrix      (string fpParameterName, int n, int m, vector<float> floatmatrix, const bool transpose = false);
    void BindMatrix      (string fpParameterName, int n, int m, vector<vector<float> > floatmatrices, const bool transpose = false);
    void BindTexture     (string fpParameterName, ITextureResourcePtr tex);
    void BindColorBuffer (string fpParameterName);
    void BindDepthBuffer (string fpParameterName);
    void BindUserBuffer  (string fpParameterName, IPostProcessingPass* outputPass, int outputAttachmentPoint);

    /* assign which buffers the fragmentprogram of a pass outputs to. (must enable for all buffers it writes to) */
    void EnableColorBufferOutput();
    void EnableDepthBufferOutput();

    /* attach userbuffer at the attachmentPoint */
    void AttachUserBuffer(int attachmentPoint, const bool createFloatTexture = false);

    /* return a COPY of the texture from the userbuffer at the attachmentPoint */
    ITexture2DPtr GetUserBuffer(int attachmentPoint);
    void          GetUserBuffer(int attachmentPoint, ITexture2DPtr texCopy);

    /* return a non-copy of the texture from the userbuffer at the attachmentPoint (be careful!) */
    ITexture2DPtr GetUserBufferRef(int attachmentPoint);

    /* wrap and filter settings for color/depth-buffer textures */
    void SetUserBufferWrap(int attachmentPoint, TextureWrap wrapS, TextureWrap wrapT);
    void SetUserBufferFilter(int attachmentPoint, TextureFilter filter);

    /* returns if this pass outputs to color or depth buffer */
    bool IsColorBufferOutput();
    bool IsDepthBufferOutput();
    bool IsUserBufferOutput(int attachmentPoint);
};

} // NS PostProcessing
} // NS OpenEngine

#endif
