#ifndef __IPOSTPROCESSINGPASS_H__
#define __IPOSTPROCESSINGPASS_H__

#include <string>
#include <vector>

#include <Resources/ITextureResource.h>
#include <PostProcessing/PostProcessingException.h>
#include <Resources/ITexture2D.h>
#include <Resources/IRenderBuffer.h>
#include <Display/Viewport.h>

namespace OpenEngine {
namespace PostProcessing {

using namespace OpenEngine::Resources;
using namespace OpenEngine::Display; // for viewport
using namespace OpenEngine::Resources; // for ITextureResourcePtr

/** Interface for PostProcessingPass
 *  @author Bjarke N. Laustsen
 */
class IPostProcessingPass {

  public:

    virtual ~IPostProcessingPass() {}

    /* assign fragment program input parameters */
    virtual void BindInt         (string fpParameterName, vector<int> intvector) = 0;
    virtual void BindInt         (string fpParameterName, vector<vector<int> > intvectors) = 0;
    virtual void BindFloat       (string fpParameterName, vector<float> floatvector) = 0;
    virtual void BindFloat       (string fpParameterName, vector<vector<float> > floatvectors) = 0;
    virtual void BindMatrix      (string fpParameterName, int n, int m, vector<float> floatmatrix, const bool transpose = false) = 0;
    virtual void BindMatrix      (string fpParameterName, int n, int m, vector<vector<float> > floatmatrices, const bool transpose = false) = 0;
    virtual void BindTexture     (string fpParameterName, ITextureResourcePtr tex) = 0;
    virtual void BindColorBuffer (string fpParameterName) = 0;
    virtual void BindDepthBuffer (string fpParameterName) = 0;
    virtual void BindUserBuffer  (string fpParameterName, IPostProcessingPass* outputPass, int outputAttachmentPoint) = 0;

    /* assign which buffers the fragmentprogram of a pass outputs to. (must enable for all buffers it writes to) */
    virtual void EnableColorBufferOutput() = 0;
    virtual void EnableDepthBufferOutput() = 0;

    /* attach userbuffer at the attachmentPoint */
    virtual void AttachUserBuffer(int attachmentPoint, const bool createFloatTexture = false) = 0;

    /* return a COPY of the texture from the userbuffer at the attachmentPoint */
    virtual ITexture2DPtr GetUserBuffer(int attachmentPoint) = 0;
    virtual void          GetUserBuffer(int attachmentPoint, ITexture2DPtr texCopy) = 0;

    /* return a non-copy of the texture from the userbuffer at the attachmentPoint (be careful!) */
    virtual ITexture2DPtr GetUserBufferRef(int attachmentPoint) = 0;

    /* wrap and filter settings for color/depth-buffer textures */
    virtual void SetUserBufferWrap(int attachmentPoint, TextureWrap wrapS, TextureWrap wrapT) = 0;
    virtual void SetUserBufferFilter(int attachmentPoint, TextureFilter filter) = 0;

    /* returns if this pass outputs to color or depth buffer */
    virtual bool IsColorBufferOutput() = 0;
    virtual bool IsDepthBufferOutput() = 0;
    virtual bool IsUserBufferOutput(int attachmentPoint) = 0;
};

} // NS PostProcessing
} // NS OpenEngine

#endif
