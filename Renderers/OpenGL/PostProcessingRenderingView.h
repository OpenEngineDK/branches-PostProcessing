
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
#include <Renderers/OpenGL/RenderingView.h>
#include <Scene/RenderStateNode.h>
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
 class PostProcessingRenderingView : virtual public RenderingView {
    IPostProcessingEffect* ppe;

public:
    PostProcessingRenderingView(Viewport& viewport, IPostProcessingEffect* ppe);
    virtual ~PostProcessingRenderingView();

    void VisitBlendNode(BlendNode* node);
    void VisitMergeNode(MergeNode* node);
    void VisitMergeBlendNode(MergeBlendNode* node);

};

} // NS OpenGL
} // NS Renderers
} // NS OpenEngine

#endif // _OPENGL_RENDERING_VIEW_H_
