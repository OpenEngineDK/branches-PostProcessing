#ifndef __FRAGMENTPROGRAM_H__
#define __FRAGMENTPROGRAM_H__

#include <Resources/ITextureResource.h>
#include <Resources/PPEResourceException.h>
#include <Logging/Logger.h>

#include <Meta/OpenGL.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <vector>
#include <string>

namespace OpenEngine {
namespace Resources {

using namespace std;

/** An object of this class encapsulates a GLSL fragmentprogram
 *  @note: OpenGL 2.0 or above only
 *  @author Bjarke N. Laustsen
 */
class FragmentProgram {

  private:

    vector<GLuint> shaderIDs;
    GLuint programID;

    // max texture units on this gfx-card (max number of samplers that can be used)
    GLint maxTextureUnits;

    // since binding textures in GLSL is a bit cumbersome, we don't do it in the bindTexture method, but rather remember what to bind
    struct TextureBinding {
	string              parameterName;
	ITextureResourcePtr texture;
	TextureBinding(string nam, ITextureResourcePtr tex) {parameterName=nam; texture=tex;}
    };
    vector<TextureBinding*> textureBindings;

    void SetupFragmentProgram(vector<string> filenames);

    string LoadString(string filename);
    void SetupTextureUnits();
    void ConstructorSetup(vector<string> filenames);

    // for avoiding side effects of most of the methods
    GLint savedProgID;
    void GuardedBind();
    void GuardedUnbind();

  public:

    FragmentProgram(string filename);
    FragmentProgram(vector<string> filenames);
    ~FragmentProgram();

    void Bind();
    void Unbind(); // unbinds all

    void BindInt(string parameterName, vector<int> intvector);
    void BindInt(string parameterName, vector<vector<int> > intvectors) ;
    void BindFloat(string parameterName, vector<float> floatvector);
    void BindFloat(string parameterName, vector<vector<float> > floatvectors);
    void BindMatrix(string parameterName, int n, int m, vector<float> floatmatrix, const bool transpose = false);
    void BindMatrix(string parameterName, int n, int m, vector<vector<float> > floatmatrices, const bool transpose = false);
    void BindTexture(string parameterName, ITextureResourcePtr texture); // will not be bound immediately - not until next bind! (GLSL binding is a bit weird)

    int GetMaxTextureBindings();
};

} // NS Resources
} // NS OpenEngine

#endif

