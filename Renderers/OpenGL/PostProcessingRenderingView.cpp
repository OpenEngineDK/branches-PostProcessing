// OpenGL rendering view.
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS)
//
// This program isP free software; It is covered by the GNU General
// Public License version 2 or any later version.
// See the GNU General Public License for more details (see LICENSE).
//--------------------------------------------------------------------

#include <Renderers/OpenGL/PostProcessingRenderingView.h>

#include <Renderers/OpenGL/Renderer.h>
#include <Scene/RenderNode.h>
#include <Scene/GeometryNode.h>
#include <Scene/TransformationNode.h>
#include <Resources/IShaderResource.h>
#include <Meta/OpenGL.h>
#include <Math/Math.h>

namespace OpenEngine {
namespace Renderers {
namespace OpenGL {

using namespace OpenEngine::Math;
using namespace OpenEngine::Geometry;
using namespace OpenEngine::Resources;

/**
 * Rendering view constructor.
 *
 * @param viewport Viewport in which to render.
 * @param ppe the post-processing effect to use
 */
PostProcessingRenderingView::PostProcessingRenderingView(Viewport& viewport, IPostProcessingEffect* ppe)
    : IRenderingView(viewport), RenderingView(viewport) {
    this->ppe = ppe;
}

/**
 * Rendering view destructor.
 */
PostProcessingRenderingView::~PostProcessingRenderingView() {

}

/**
 * Process a blend node.
 *
 * @param node Render state node to apply.
 */
void PostProcessingRenderingView::VisitBlendNode(BlendNode* node) {
    node->ApplyToSubNodes(*this);
}

/**
 * Process a merge node.
 *
 * @param node Render state node to apply.
 */
void PostProcessingRenderingView::VisitMergeNode(MergeNode* node) {
    node->ApplyToSubNodes(*this);
}

/**
 * Process a mergeblend node.
 *
 * @param node Render state node to apply.
 */
void PostProcessingRenderingView::VisitMergeBlendNode(MergeBlendNode* node) {
    node->ApplyToSubNodes(*this);
}

} // NS OpenGL
} // NS Renderers
} // NS OpenEngine
