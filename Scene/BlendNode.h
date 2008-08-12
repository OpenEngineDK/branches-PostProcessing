// BSP tree node.
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS)
//
// This program is free software; It is covered by the GNU General
// Public License version 2 or any later version.
// See the GNU General Public License for more details (see LICENSE).
//--------------------------------------------------------------------

#ifndef _BLEND_NODE_H_
#define _BLEND_NODE_H_

#include <Scene/SceneNode.h>
#include <PostProcessing/OpenGL/PostProcessingEffect.h>
#include <Resources/ITexture2D.h>

namespace OpenEngine {
namespace Scene {

using namespace OpenEngine::Scene;
using namespace OpenEngine::PostProcessing;

/**
 * Blend Node.
 * @author Bjarke N. Laustsen
 */
class BlendNode : public SceneNode {
  private:
    PostProcessingEffect* ppe;
    float alpha;

    void PerformBlend();
  public:
    BlendNode(PostProcessingEffect* ppe, const float alpha = 1.0f);
    ~BlendNode();

    void Accept(ISceneNodeVisitor& visitor);
    void ApplyToSubNodes(ISceneNodeVisitor& visitor);

    void SetAlpha(float alpha);
};

} // NS Scene
} // NS OpenEngine

#endif // _BSP_NODE_H_
