// todo: support floating point buffers!

#include "MergeBlendNode.h"

/* @author Bjarke N. Laustsen
 */
namespace OpenEngine {
namespace Scene {

    MergeBlendNode::MergeBlendNode(PostProcessingEffect* ppe, IEngine& engine, const int blendMethod, const bool useFloatBuffers) {
    this->ppe = ppe;
    this->newparentColorTex = ITexture2DPtr(new Texture2D(1, 1, TEX_RGBA , TEX_REPEAT, TEX_REPEAT, TEX_LINEAR, TEX_LINEAR));
    this->newparentDepthTex = ITexture2DPtr(new Texture2D(1, 1, TEX_DEPTH, TEX_REPEAT, TEX_REPEAT, TEX_LINEAR, TEX_LINEAR));

    this->mergeblend1 = new MergeBlend1(ppe->GetViewport(), engine, useFloatBuffers);
    this->mergeblend1->Add(ppe);
    this->mergeblend1->EnableScreenOutput(false);
    this->mergeblend2 = new MergeBlend2(ppe->GetViewport(), engine, blendMethod, useFloatBuffers);
    this->mergeblend1->Add(mergeblend2);
}

MergeBlendNode::~MergeBlendNode() {
    delete mergeblend1;
    delete mergeblend2;
}

void MergeBlendNode::ApplyToSubNodes(ISceneNodeVisitor& visitor) {

    GetParentTextures();

    mergeblend1->PreRender();
    mergeblend1->SetParameters(newparentDepthTex);
    mergeblend2->SetParameters(newparentColorTex, newparentDepthTex);
    VisitSubNodes(visitor);
    mergeblend1->PostRender();

    mergeblend2->GetFinalColorBuffer(newparentColorTex);
    mergeblend2->GetFinalDepthBuffer(newparentDepthTex);
}

// get color and depth texture handles of the effect which was bound before this node was called (quite hackish - improve if there is time)
void MergeBlendNode::GetParentTextures() {
    GLint colorType;
    GLint depthType;
    glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_EXT, &colorType);
    glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT , GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_EXT, &depthType);
    if (colorType != GL_TEXTURE || depthType != GL_TEXTURE) throw PostProcessingException("internal error");

    GLint colorID;
    GLint depthID;
    glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_EXT, &colorID);
    glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT , GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_EXT, &depthID);

    newparentColorTex->SetID(colorID);
    newparentDepthTex->SetID(depthID);
}


/* MergeBlend1 class */

    MergeBlendNode::MergeBlend1::MergeBlend1(Viewport* viewport, IEngine& engine, bool useFloatBuffers) : PostProcessingEffect(viewport,engine, useFloatBuffers) {
}

void MergeBlendNode::MergeBlend1::Setup() {
    //string path = ResourceManager::GetPath();
    DirectoryManager::AppendPath("extensions/PostProcessing/Scene/"); // how to not hardcode the path?!
    pass1 = AddPass("mergeblend1.frag");
    //ResourceManager::SetPath(path);

    clearcolor.push_back(0);
    clearcolor.push_back(0);
    clearcolor.push_back(0);
    clearcolor.push_back(0);

    pass1->BindColorBuffer("childColorBuf");
    pass1->BindDepthBuffer("childDepthBuf");
    pass1->BindFloat("clearcolor", clearcolor);
    pass1->EnableColorBufferOutput();
    pass1->EnableDepthBufferOutput();
}

void MergeBlendNode::MergeBlend1::PerFrame(const float deltaTime) {
}

void MergeBlendNode::MergeBlend1::SetParameters(ITexture2DPtr parentDepthTex) {
    pass1->BindTexture("parentDepthBuf", parentDepthTex);
}


/* MergeBlend2 class */

    MergeBlendNode::MergeBlend2::MergeBlend2(Viewport* viewport, IEngine& engine, int blendMethod, bool useFloatBuffers) : PostProcessingEffect(viewport,engine,useFloatBuffers) {
    this->blendMethod = blendMethod;
}

void MergeBlendNode::MergeBlend2::Setup() {
    //string path = ResourceManager::GetPath();
    DirectoryManager::AppendPath("extensions/PostProcessing/Scene/"); // how to not hardcode the path?!
    if (blendMethod == 0) pass1 = AddPass("mergeblend2.frag");
    else                  pass1 = AddPass("mergeblend2b.frag");
    //ResourceManager::SetPath(path);

    pass1->BindColorBuffer("childColorBuf");
    pass1->BindDepthBuffer("childDepthBuf");
    pass1->EnableColorBufferOutput();
    pass1->EnableDepthBufferOutput();
}

void MergeBlendNode::MergeBlend2::PerFrame(const float deltaTime) {
}

void MergeBlendNode::MergeBlend2::SetParameters(ITexture2DPtr parentColorTex, ITexture2DPtr parentDepthTex) {
    pass1->BindTexture("parentColorBuf", parentColorTex->Clone());
    pass1->BindTexture("parentDepthBuf", parentDepthTex->Clone());
}

} // NS Scene
} // NS OpenEngine


