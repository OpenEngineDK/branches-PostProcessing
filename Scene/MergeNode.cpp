// todo: support floating point buffers!

#include "MergeNode.h"

/* @author Bjarke N. Laustsen
 */
namespace OpenEngine {
namespace Scene {

    MergeNode::MergeNode(PostProcessingEffect* ppe, IEngine& engine, const float alpha, const bool useFloatBuffers) {
    this->ppe = ppe;
    this->newparentColorTex = ITexture2DPtr(new Texture2D(1, 1, TEX_RGBA , TEX_REPEAT, TEX_REPEAT, TEX_LINEAR, TEX_LINEAR));
    this->newparentDepthTex = ITexture2DPtr(new Texture2D(1, 1, TEX_DEPTH, TEX_REPEAT, TEX_REPEAT, TEX_LINEAR, TEX_LINEAR));
    merge = new Merge(ppe->GetViewport(),engine, useFloatBuffers);
    ppe->Add(merge);
}

MergeNode::~MergeNode() {
    delete merge;
}

void MergeNode::ApplyToSubNodes(ISceneNodeVisitor& visitor) {

    GetParentTextures();

    ppe->PreRender();
    merge->SetParameters(newparentColorTex, newparentDepthTex);
    VisitSubNodes(visitor);
    ppe->PostRender();

    merge->GetFinalColorBuffer(newparentColorTex);
    merge->GetFinalDepthBuffer(newparentDepthTex);
}


// get color and depth texture handles of the effect which was bound before this node was called (quite hackish - improve if there is time)
void MergeNode::GetParentTextures() {
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


/* Merge class */
MergeNode::Merge::Merge(Viewport* viewport, IEngine& engine, bool useFloatBuffers) 
    : PostProcessingEffect(viewport, engine, useFloatBuffers) {}

void MergeNode::Merge::Setup() {
    //string path = ResourceManager::GetPath();
    DirectoryManager::AppendPath("extensions/PostProcessing/Scene/"); // how to not hardcode the path?!
    pass1 = AddPass("merge.frag");
    //ResourceManager::SetPath(path);

    pass1->BindColorBuffer("childColorBuf");
    pass1->BindDepthBuffer("childDepthBuf");
    pass1->EnableColorBufferOutput();
    pass1->EnableDepthBufferOutput();
}

void MergeNode::Merge::PerFrame(const float deltaTime) {
}

void MergeNode::Merge::SetParameters(ITexture2DPtr parentColorTex, ITexture2DPtr parentDepthTex) {
    pass1->BindTexture("parentColorBuf", parentColorTex->Clone());
    pass1->BindTexture("parentDepthBuf", parentDepthTex->Clone());
}


} // NS Scene
} // NS OpenEngine


