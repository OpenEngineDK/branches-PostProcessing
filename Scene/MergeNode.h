// BSP tree node.
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS)
//
// This program is free software; It is covered by the GNU General
// Public License version 2 or any later version.
// See the GNU General Public License for more details (see LICENSE).
//--------------------------------------------------------------------

#ifndef _MERGE_NODE_H_
#define _MERGE_NODE_H_

#include <Scene/SceneNode.h>
#include <PostProcessing/OpenGL/PostProcessingEffect.h>
#include <Resources/ITexture2D.h>
#include <Resources/ResourceManager.h>
#include <Core/IEngine.h>

namespace OpenEngine {
namespace Scene {

using namespace OpenEngine::Scene;
using namespace OpenEngine::PostProcessing;
using namespace OpenEngine::Resources;

/**
 * Merge Node.
 * @note a post-processing must be active before this node is applied (with ApplyToSubNodes)
 * @author Bjarke N. Laustsen
 */
class MergeNode : public SceneNode {
    OE_SCENE_NODE(QuadNode, ISceneNode)
  private:
    PostProcessingEffect* ppe;
    ITexture2DPtr newparentColorTex;
    ITexture2DPtr newparentDepthTex;

    void GetParentTextures();

    // the pass which cuts off the parts of the child image occluded by the parent image
    class Merge : public PostProcessingEffect {
      private:
        IPostProcessingPass* pass1;
      public:
        Merge(Viewport* viewport, IEngine& engine, bool useFloatBuffers);
	void Setup();
	void PerFrame(const float deltaTime);
	void SetParameters(ITexture2DPtr parentColorTex, ITexture2DPtr parentDepthTex);
    };

    Merge* merge;

  public:
    MergeNode(PostProcessingEffect* ppe, IEngine& engine, const float alpha = 1.0f, const bool useFloatBuffers = false);
    ~MergeNode();

    void ApplyToSubNodes(ISceneNodeVisitor& visitor);
};

} // NS Scene
} // NS OpenEngine

#endif // _BSP_NODE_H_
