#include "BlendNode.h"

/* @author Bjarke N. Laustsen
 */
namespace OpenEngine {
namespace Scene {

BlendNode::BlendNode(PostProcessingEffect* ppe, const float alpha) {
    this->ppe = ppe;
    this->alpha = alpha;
}

BlendNode::~BlendNode() {
}

void BlendNode::Accept(ISceneNodeVisitor& visitor) {
    visitor.VisitBlendNode(this);
}

void BlendNode::ApplyToSubNodes(ISceneNodeVisitor& visitor) {

    ppe->PreRender();

    VisitSubNodes(visitor);

    ppe->PostRender();

    PerformBlend();
}


void BlendNode::PerformBlend() {

    // remember current attributes and matrices
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    // get position, width and height of current viewport
    GLint viewport[4]; // (x,y,w,h)
    glGetIntegerv(GL_VIEWPORT, viewport);
    int x = viewport[0];
    int y = viewport[1];
    int w = viewport[2];
    int h = viewport[3];

    // set projection, modelview matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glViewport(x, y, w, h);

    // disable depth-buffer, lighting, etc
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_ALPHA_TEST); // OR?
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // enable texturing
    glEnable(GL_TEXTURE_2D);

    // draw quad
    ITexture2DPtr finalcolbuf = ppe->GetFinalColorBufferRef();
    finalcolbuf->Bind();
    glPolygonMode(GL_FRONT,GL_FILL);
    glColor4f(1,1,1,alpha);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex2f(x  , y);
    glTexCoord2f(1.0, 0.0); glVertex2f(x+w, y);
    glTexCoord2f(1.0, 1.0); glVertex2f(x+w, y+h);
    glTexCoord2f(0.0, 1.0); glVertex2f(x  , y+h);
    glEnd();

    // restore attributes and matrices
    glPopAttrib();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void BlendNode::SetAlpha(float alpha) {
    this->alpha = alpha;
}

} // NS Scene
} // NS OpenEngine