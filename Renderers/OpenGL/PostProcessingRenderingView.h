
// OpenGL rendering view.
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS)
//
// This program is free software; It is covered by the GNU General
// Public License version 2 or any later version.
// See the GNU General Public License for more details (see LICENSE).
//--------------------------------------------------------------------

#ifndef _OPENGL_POSTPROCESSING_RENDERING_VIEW_H_
#define _OPENGL_POSTPROCESSING_RENDERING_VIEW_H_


#include <PostProcessing/IPostProcessingEffect.h>
#include <Scene/BlendNode.h>
#include <Scene/MergeNode.h>
#include <Scene/MergeBlendNode.h>

#include <Geometry/FaceSet.h>
#include <Renderers/IRenderingView.h>
#include <Renderers/RenderStateNode.h>
#include <vector>

namespace OpenEngine {
namespace Renderers {
namespace OpenGL {

using namespace OpenEngine::PostProcessing;

using namespace OpenEngine::Renderers;
using namespace OpenEngine::Scene;
using namespace OpenEngine::Geometry;
using namespace std;

/**
 * Concrete PostProcessingRenderingView using OpenGL.
 * Same as the normal RenderingView except it uses post-processing effects.
 * @author Bjarke N. Laustsen and OECore-team
 */
class PostProcessingRenderingView : virtual public IRenderingView {
    IRenderer* renderer;
    vector<RenderStateNode*> stateStack;
    IPostProcessingEffect* ppe;

    void RenderBinormals(FacePtr face);
    void RenderTangents(FacePtr face);
    void RenderNormals(FacePtr face);
    void RenderHardNormal(FacePtr face);
    void RenderLine(Vector<3,float> vert, Vector<3,float> norm, Vector<3,float> color);
    bool IsOptionSet(RenderStateNode::RenderStateOption o);
public:
    PostProcessingRenderingView(Viewport& viewport, IPostProcessingEffect* ppe);
    virtual ~PostProcessingRenderingView();
    void VisitGeometryNode(GeometryNode* node);
    void VisitTransformationNode(TransformationNode* node);
    void VisitRenderStateNode(RenderStateNode* node);
    void VisitRenderNode(IRenderNode* node);
    void VisitBlendNode(BlendNode* node);
    void VisitMergeNode(MergeNode* node);
    void VisitMergeBlendNode(MergeBlendNode* node);
    void Render(IRenderer* renderer, ISceneNode* root);
    void Handle(RenderingEventArg arg);
    IRenderer* GetRenderer();
};

} // NS OpenGL
} // NS Renderers
} // NS OpenEngine

#endif // _OPENGL_RENDERING_VIEW_H_
