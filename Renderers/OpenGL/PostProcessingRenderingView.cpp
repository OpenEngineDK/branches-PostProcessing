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
#include <Renderers/IRenderNode.h>
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
    : IRenderingView(viewport),
      renderer(NULL) {
    this->ppe = ppe;
    RenderStateNode* renderStateNode = new RenderStateNode();
    renderStateNode->AddOptions(RenderStateNode::RENDER_TEXTURES);
    renderStateNode->AddOptions(RenderStateNode::RENDER_SHADERS);
    renderStateNode->AddOptions(RenderStateNode::RENDER_BACKFACES);
    stateStack.push_back(renderStateNode);
}

/**
 * Rendering view destructor.
 */
PostProcessingRenderingView::~PostProcessingRenderingView() {

}

/**
 * Get the renderer that the view is processing for.
 *
 * @return Current renderer, NULL if no renderer processing is active.
 */
IRenderer* PostProcessingRenderingView::GetRenderer() {
    return renderer;
}

void PostProcessingRenderingView::Handle(RenderingEventArg arg) {
	Render(&arg.renderer, arg.renderer.GetSceneRoot());
}

/**
 * Renderer the scene.
 *
 * @param renderer a Renderer
 * @param root The scene to be rendered
 */
void PostProcessingRenderingView::Render(IRenderer* renderer, ISceneNode* root) {
    this->renderer = renderer;
    //ppe->PreRender();
    root->Accept(*this);
    //ppe->PostRender();
    this->renderer = NULL;
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

/**
 * Process a rendering node.
 *
 * @param node Rendering node to apply.
 */
void PostProcessingRenderingView::VisitRenderNode(IRenderNode* node) {
    node->Apply(this);
}

/**
 * Process a render state node.
 *
 * @param node Render state node to apply.
 */
void PostProcessingRenderingView::VisitRenderStateNode(RenderStateNode* node) {
    stateStack.push_back(node);
    node->VisitSubNodes(*this);
    stateStack.pop_back();
}

/**
 * Process a transformation node.
 *
 * @param node Transformation node to apply.
 */
void PostProcessingRenderingView::VisitTransformationNode(TransformationNode* node) {
    // push transformation matrix
    Matrix<4,4,float> m = node->GetTransformationMatrix();
    float f[16];
    m.ToArray(f);
    glPushMatrix();
    glMultMatrixf(f);
    // traverse sub nodes
    node->VisitSubNodes(*this);
    // pop transformation matrix
    glPopMatrix();
}

/**
 * Process a geometry node.
 *
 * @param node Geometry node to render
 */
void PostProcessingRenderingView::VisitGeometryNode(GeometryNode* node) {

    if( IsOptionSet(RenderStateNode::RENDER_WIREFRAMED) ) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Enable back-face culling, only faces facing towards the view is rendered.
    if( IsOptionSet(RenderStateNode::RENDER_BACKFACES) )
        glDisable(GL_CULL_FACE);
    else
        glEnable(GL_CULL_FACE);

    // Remember last bound texture and shader
    int currentTexture = 0;
    IShaderResourcePtr currentShader;
    int binormalid = -1;
    int tangentid = -1;
    FaceList::iterator itr;
    FaceSet* faces = node->GetFaceSet();
    if (faces == NULL) return;

    // for each face ...
    for (itr = faces->begin(); itr != faces->end(); itr++) {
        FacePtr f = (*itr);

        // check if shaders should be applied
        if (Renderer::IsGLSLSupported()) {

            // if the shader changes release the old shader
            if (currentShader != NULL && currentShader != f->mat->shad) {
                currentShader->ReleaseShader();
                currentShader.reset();
            }

            // check if a shader shall be applied
            if (IsOptionSet(RenderStateNode::RENDER_SHADERS) &&
                f->mat->shad != NULL &&              // and the shader is not null
                currentShader != f->mat->shad) {     // and the shader is different from the current
                // get the bi-normal and tangent ids
                binormalid = f->mat->shad->GetAttributeID("binormal");
                tangentid = f->mat->shad->GetAttributeID("tangent");
                f->mat->shad->ApplyShader();
                // set the current shader
                currentShader = f->mat->shad;
            }
        }

        // if a shader is in use reset the current texture,
        // but dont disable in GL because the shader may use textures.
        if (currentShader != NULL) currentTexture = 0;

        // if the face has no texture reset the current texture
        else if (f->mat->texr == NULL) {
            glBindTexture(GL_TEXTURE_2D, 0); // @todo, remove this if not needed, release texture
            glDisable(GL_TEXTURE_2D);
            currentTexture = 0;
        }

        // check if texture shall be applied
        else if (IsOptionSet(RenderStateNode::RENDER_TEXTURES) &&
            currentTexture != f->mat->texr->GetID()) {  // and face texture is different then the current one
            currentTexture = f->mat->texr->GetID();
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, currentTexture);
        }

        glBegin(GL_TRIANGLES);
        // for each vertex ...
        for (int i=0; i<3; i++) {
            Vector<3,float> v = f->vert[i];
            Vector<2,float> t = f->texc[i];
            Vector<3,float> n = f->norm[i];
            Vector<4,float> c = f->colr[i];
            glTexCoord2f(t[0],t[1]);
            glColor4f (c[0],c[1],c[2],c[3]);
            glNormal3f(n[0],n[1],n[2]);
            // apply tangent and binormal per vertex for the shader to use
            if (currentShader != NULL) {
                if (binormalid != -1)
                    currentShader->VertexAttribute(binormalid, f->bino[i]);
                if (tangentid != -1)
                    currentShader->VertexAttribute(tangentid, f->tang[i]);
            }
			glVertex3f(v[0],v[1],v[2]);
        }
        glEnd();

        // Render normal if enabled
        GLboolean c = glIsEnabled(GL_COLOR_MATERIAL);
        GLboolean l = glIsEnabled(GL_LIGHTING);
 	glEnable(GL_COLOR_MATERIAL);
        glDisable(GL_LIGHTING);

        if (IsOptionSet(RenderStateNode::RENDER_BINORMALS))
            RenderBinormals(f);
        if (IsOptionSet(RenderStateNode::RENDER_TANGENTS))
            RenderTangents(f);
        if (IsOptionSet(RenderStateNode::RENDER_NORMALS))
            RenderNormals(f);
        if (IsOptionSet(RenderStateNode::RENDER_HARD_NORMAL))
            RenderHardNormal(f);
        if (c) glEnable(GL_COLOR_MATERIAL);
        if (l) glEnable(GL_LIGHTING);
    }

    // last we release the final shader
    if (currentShader != NULL)
        currentShader->ReleaseShader();

    // disable textures if it has been enabled
    glDisable(GL_TEXTURE_2D);
}

bool PostProcessingRenderingView::IsOptionSet(RenderStateNode::RenderStateOption o) {
    return stateStack.back()->IsOptionSet(o);
}

void PostProcessingRenderingView::RenderNormals(FacePtr face) {
    for (int i=0; i<3; i++) {
        Vector<3,float> v = face->vert[i];
        Vector<3,float> n = face->norm[i];
		Vector<3,float> c(0,1,0);
        RenderLine(v,n,c);
    }
}

void PostProcessingRenderingView::RenderHardNormal(FacePtr face) {
    Vector<3,float> v = (face->vert[0]+face->vert[1]+face->vert[2])/3;
    Vector<3,float> n = face->hardNorm;
    Vector<3,float> c(1,0,1);
    RenderLine(v,n,c);
}

void PostProcessingRenderingView::RenderBinormals(FacePtr face) {
    for (int i=0; i<3; i++) {
        Vector<3,float> v = face->vert[i];
        Vector<3,float> n = face->bino[i];
		Vector<3,float> c(0,1,1);
        RenderLine(v,n,c);
    }
}

void PostProcessingRenderingView::RenderTangents(FacePtr face) {
    for (int i=0; i<3; i++) {
        Vector<3,float> v = face->vert[i];
		Vector<3,float> n = face->tang[i];
		Vector<3,float> c(1,0,0);
        RenderLine(v,n,c);
    }
}

void PostProcessingRenderingView::RenderLine(Vector<3,float> vert, Vector<3,float> norm, Vector<3,float> color) {
    renderer->DrawLine(Line(vert,vert+norm),color,1);
}

} // NS OpenGL
} // NS Renderers
} // NS OpenEngine