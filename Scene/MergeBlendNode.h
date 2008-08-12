// BSP tree node.
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS)
//
// This program is free software; It is covered by the GNU General
// Public License version 2 or any later version.
// See the GNU General Public License for more details (see LICENSE).
//--------------------------------------------------------------------

#ifndef _MERGE_BLEND_NODE_H_
#define _MERGE_BLEND_NODE_H_

#include <Scene/SceneNode.h>
#include <PostProcessing/OpenGL/PostProcessingEffect.h>
#include <Resources/ITexture2D.h>
#include <Resources/ResourceManager.h>

namespace OpenEngine {
namespace Scene {

using namespace OpenEngine::Scene;
using namespace OpenEngine::PostProcessing;
using namespace OpenEngine::Resources;

/**
 * Merge-Blend Node.
 * @note a post-processing must be active before this node is applied (with ApplyToSubNodes)
 * @author Bjarke N. Laustsen
 */
class MergeBlendNode : public SceneNode {
  private:
    PostProcessingEffect* ppe;
    ITexture2DPtr newparentColorTex;
    ITexture2DPtr newparentDepthTex;

    void GetParentTextures();

    // the pass which cuts off the parts of the child image occluded by the parent image
    class MergeBlend1 : public PostProcessingEffect {
      private:
        IPostProcessingPass* pass1;
        vector<float> clearcolor;
      public:
	MergeBlend1(Viewport* viewport, bool useFloatBuffers);
	void Setup();
	void PerFrame(const float deltaTime);
	void SetParameters(ITexture2DPtr parentDepthTex);
    };

    // the pass which blends the new child image togethe with the parent image according to its depth values
    class MergeBlend2 : public PostProcessingEffect {
      private:
        IPostProcessingPass* pass1;
        int blendMethod;
      public:
	MergeBlend2(Viewport* viewport, int blendMethod, bool useFloatBuffers);
	void Setup();
	void PerFrame(const float deltaTime);
	void SetParameters(ITexture2DPtr parentColorTex, ITexture2DPtr parentDepthTex);
    };

    MergeBlend1* mergeblend1;
    MergeBlend2* mergeblend2;

  public:
    MergeBlendNode(PostProcessingEffect* ppe, const int blendMethod = 0, const bool useFloatBuffers = false);
    ~MergeBlendNode();

    void Accept(ISceneNodeVisitor& visitor);
    void ApplyToSubNodes(ISceneNodeVisitor& visitor);
};

} // NS Scene
} // NS OpenEngine

#endif // _BSP_NODE_H_
