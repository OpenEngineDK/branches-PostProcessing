#include "FragmentProgram.h"
#include <Resources/ResourceManager.h>
#include <Resources/DirectoryManager.h>

// see http://www.opengl.org/sdk/docs/man/xhtml/glUniform.xml for how to set uniforms

/* @author Bjarke N. Laustsen
 */
namespace OpenEngine {
namespace Resources {

/**
 * create a fragment program from a file (must contain a main() method)
 * @param[in] filename the filename of the file containing the GLSL fragmentprogram sourcecode
 */
FragmentProgram::FragmentProgram(string filename){
    vector<string> filenames;
    filenames.push_back(filename);
    ConstructorSetup(filenames);
}

/**
 * create a fragment program from several files. Precisely one of them must contain the main() method.
 * This can be useful if you have functions common to several fragmentprograms which you have factored out
 * into seperate files.
 * @param[in] filenames the filenames of the files containing the GLSL fragmentprogram sourcecode
 */
FragmentProgram::FragmentProgram(vector<string> filenames) {
    if (filenames.size() == 0) throw new PPEResourceException("list of filenames was empty");
    ConstructorSetup(filenames);
}

void FragmentProgram::ConstructorSetup(vector<string> filenames) {
    this->programID = 0;
    this->savedProgID = 0;
    glGetIntegerv(GL_MAX_TEXTURE_UNITS, &(this->maxTextureUnits));
    SetupFragmentProgram(filenames);
}

FragmentProgram::~FragmentProgram() {
    for (int i=0; i<shaderIDs.size(); i++)
        glDeleteShader(shaderIDs.at(i));
    glDeleteProgram(programID);
    for (int i=0; i<textureBindings.size(); i++)
        delete textureBindings.at(i);
}

/** get max number of textures that can be bound to uniform sampler parameters in the fragment program on this graphics card
 */
int FragmentProgram::GetMaxTextureBindings() {
    return maxTextureUnits;
}

// se : http://www.lighthouse3d.com/opengl/glsl/index.php?oglshader
void FragmentProgram::SetupFragmentProgram(vector<string> filenames) {

    // create the shaders
    for (int i=0; i<filenames.size(); i++) {
	string filename = filenames.at(i);
	GLuint shaderID = glCreateShader(GL_FRAGMENT_SHADER);
	shaderIDs.push_back(shaderID);
	//const char* shaderString = LoadString(filename).c_str();
	string shaderString = LoadString(filename);
	const char* shaderChars = shaderString.c_str();
	glShaderSource(shaderID, 1, (const GLchar**)&shaderChars, NULL); // <- null means that the strings are NULL terminated
	glCompileShader(shaderID);

	// print errors and warnings to logger
	GLsizei bufSize;
	glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &bufSize);
	GLsizei length;
	char*   infoLog = new char[bufSize];
	glGetShaderInfoLog(shaderID, bufSize, &length, infoLog);
	if (length>0) logger.error << "\"" << filename << "\" compiler output:\n" << infoLog << logger.end;
	delete infoLog;

	/*
	// abort if compile error
	GLint shaderCompileOk;
	glGetShaderiv (shaderID , GL_COMPILE_STATUS, &shaderCompileOk);
	if (!shaderCompileOk)
	     throw new PPEResourceException("error compiling shader");
	     //printf("error compiling shader-source %s\n",filename);
	*/
    }

    // create a program (link all the shaders together to create the executable shader program)
    programID = glCreateProgram();
    for (int i=0; i<shaderIDs.size(); i++)
        glAttachShader(programID, shaderIDs.at(i));
    glLinkProgram(programID);

    // print errors and warnings to logger (only if real errors, otherwise it will just repeat the shader errors)
    GLint programLinkOk;
    glGetProgramiv(programID, GL_LINK_STATUS, &programLinkOk);
    if (!programLinkOk) {
	GLsizei bufSize;
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &bufSize);
	GLsizei length;
	char*   infoLog = new char[bufSize];
	glGetProgramInfoLog(programID, bufSize, &length, infoLog);
	if (length>0) {
	    for (int i=0; i<filenames.size(); i++) logger.error << "\"" << filenames.at(i) << "\" ";
	    logger.error << "linker output:\n" << infoLog << logger.end;
	}
	delete infoLog;
    }

    /*
    // abort if link error
    GLint programLinkOk;
    glGetProgramiv(programID, GL_LINK_STATUS   , &programLinkOk);
    if (!programLinkOk)
        throw new PPEResourceException("shader linking error");
    */
}

string FragmentProgram::LoadString(string filename) {

    filename = OpenEngine::Resources::DirectoryManager::FindFileInPath(filename);

    FILE *fp = fopen(filename.c_str(),"rb"); // for binary files ftell works correctly (not text files)

    if (fp == NULL) {
        logger.error << filename << logger.end;
	throw new PPEResourceException("error opening shader");
    }

    fseek(fp, 0, SEEK_END);
    int length = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char* str = new char[length+1];
    int reallen = fread(str, 1, length, fp);
    str[reallen] = '\0';

    if (ferror(fp))
	throw new PPEResourceException("error loading shader");

    fclose(fp);

    string s(str);
    delete str;

    return s;
}


/** Bind this fragment program
 * @note sideeffect: if any textures bound, texture units will be changed! (could be backed up by user)
 */
void FragmentProgram::Bind() {
    glUseProgram(programID);
    //TODO: remember texture-bindings/settings for all texture units
    SetupTextureUnits();
}

/** Unbind any fragment programs (not just this one)
 */
void FragmentProgram::Unbind() {
    glUseProgram(0); // return to fixed-function operation
    //TODO: restore texture-bindings/settings for all texture units
}

/** bind uniform int
 */
void FragmentProgram::BindInt(string parameterName, vector<int> intvector) {

    vector<vector<int> > intvectors;
    intvectors.push_back(intvector);
    BindInt(parameterName, intvectors);
}



/** bind uniform array of int-scalars/vectors
 */
void FragmentProgram::BindInt(string parameterName, vector<vector<int> > intvectors) {
    if (intvectors.size() == 0) return;

    int vectorsize = intvectors.at(0).size();
    if (vectorsize < 1 || vectorsize > 4) throw new PPEResourceException("GLSL doesn't have a ivecX type, with the supplied X!");

    // create a C array of all the intvector-values (to be supplied to OpenGL)
    int* intarray = new int[vectorsize * intvectors.size()];

    for (int i=0; i<intvectors.size(); i++) {
        vector<int> intvector = intvectors.at(i);
	if (intvector.size() != vectorsize) throw new PPEResourceException("all vectors in an array must have the same size!");
	for (int j=0; j<vectorsize; j++)
	    intarray[i*vectorsize + j] = intvector.at(j);
    }


    GuardedBind();

    // get input parameter handle by name
    GLint paramID = glGetUniformLocation(programID, parameterName.c_str());
    if (paramID == -1) logger.error << "uniform \"" << parameterName << "\" does not exist" << logger.end;

    // set value of variable (type: float, float2, float3, float4)
    if (vectorsize==1) glUniform1iv(paramID, intvectors.size(), intarray); // see: http://www.thescripts.com/forum/thread394740.html
    if (vectorsize==2) glUniform2iv(paramID, intvectors.size(), intarray); // see: http://developer.3dlabs.com/documents/glmanpages/glUniform.htm
    if (vectorsize==3) glUniform3iv(paramID, intvectors.size(), intarray);
    if (vectorsize==4) glUniform4iv(paramID, intvectors.size(), intarray);

    GuardedUnbind();

    delete intarray;
}



/** Bind a value to a uniform floatN input-parameter of the fragmentprogram of this pass.
 *  The value of the input-parameter must be given as a float-array of length N.
 *  GLSL only supports float, float2, float3, float4 so N must be 1,2,3 or 4.
 *
 *  @param[in] parameterName the name of the input-parameter in the fragment program
 *  @param[in] floatvector the float-vector (N is derived from the length of this vector)
 *  @exception PPEResourceException thrown if N is not 1,2,3 or 4
 */
void FragmentProgram::BindFloat(string parameterName, vector<float> floatvector) {

    vector<vector<float> > floatvectors;
    floatvectors.push_back(floatvector);
    BindFloat(parameterName, floatvectors);
}



/** bind uniform array of float-scalars/vectors
 */
void FragmentProgram::BindFloat(string parameterName, vector<vector<float> > floatvectors) {
    if (floatvectors.size() == 0) return;

    int vectorsize = floatvectors.at(0).size();
    if (vectorsize < 1 || vectorsize > 4) throw new PPEResourceException("GLSL doesn't have a vecX type, with the supplied X!");

    // create a C array of all the floatvector-values (to be supplied to OpenGL)
    float* floatarray = new float[vectorsize * floatvectors.size()];

    for (int i=0; i<floatvectors.size(); i++) {
        vector<float> floatvector = floatvectors.at(i);
	if (floatvector.size() != vectorsize) throw new PPEResourceException("all vectors in an array must have the same size!");
	for (int j=0; j<vectorsize; j++)
	    floatarray[i*vectorsize + j] = floatvector.at(j);
    }

    GuardedBind();

    // get input parameter handle by name
    GLint paramID = glGetUniformLocation(programID, parameterName.c_str());
    if (paramID == -1) logger.error << "uniform \"" << parameterName << "\" does not exist" << logger.end;

    // set value of variable (type: float, float2, float3, float4)
    if (vectorsize==1) glUniform1fv(paramID, floatvectors.size(), floatarray); // see: http://www.thescripts.com/forum/thread394740.html
    if (vectorsize==2) glUniform2fv(paramID, floatvectors.size(), floatarray); // see: http://developer.3dlabs.com/documents/glmanpages/glUniform.htm
    if (vectorsize==3) glUniform3fv(paramID, floatvectors.size(), floatarray);
    if (vectorsize==4) glUniform4fv(paramID, floatvectors.size(), floatarray);

    GuardedUnbind();

    delete floatarray;
}



/** Bind a value to a uniform floatNxM input-parameter of the fragmentprogram of this pass
 *  The value of the input-parameter must be given as a float-array of length N*N.
 *  The entries in the array must be given in row-major order (unless you set transpose=true)
 *
 *  @param[in] parameterName the name of the input-parameter in the fragment program
 *  @param[in] n columns
 *  @param[in] m rows
 *  @param[in] floatmatrix the array with the matrix values
 *  @param[in] transpose (optional) wether the matrix is given in column-major order (the C++ way)
 *  @exception PPEResourceException thrown if the matrix is not 2*2, 3*3 or 4*4
 */
void FragmentProgram::BindMatrix(string parameterName, int n, int m, vector<float> floatmatrix, const bool transpose) {
    vector<vector<float> > floatmatrices;
    floatmatrices.push_back(floatmatrix);
    BindMatrix(parameterName, n, m, floatmatrices, transpose);
}


/** bind array of uniform matrices
 */
void FragmentProgram::BindMatrix(string parameterName, int n, int m, vector<vector<float> > floatmatrices, const bool transpose) {
    if (floatmatrices.size() == 0) return;

    int matrixsize = floatmatrices.at(0).size(); // num entries in each matrix
    if (matrixsize != n*m) throw new PPEResourceException("supplied vector-size doesn't match supplied dimensions!");
    if (n<2 || n>4 || m<2 || m>4) throw new PPEResourceException("unsupported dimensions!");

    // create a C array of all the floatmatrix-entries (to be supplied to OpenGL)
    float* floatarray = new float[matrixsize * floatmatrices.size()];

    for (int i=0; i<floatmatrices.size(); i++) {
        vector<float> floatmatrix = floatmatrices.at(i);
	if (floatmatrix.size() != matrixsize) throw new PPEResourceException("all matrices in an array must have the same size!");
	for (int j=0; j<matrixsize; j++)
	    floatarray[i*matrixsize + j] = floatmatrix.at(j);
    }


    GuardedBind();

    // get input parameter handle by name
    GLint paramID = glGetUniformLocation(programID, parameterName.c_str());
    if (paramID == -1) logger.error << "uniform \"" << parameterName << "\" does not exist" << logger.end;

    // set value of variable
    if (n==2 && m==2) glUniformMatrix2fv  (paramID, floatmatrices.size(), (GLboolean)transpose, floatarray);
    if (n==2 && m==3) glUniformMatrix2x3fv(paramID, floatmatrices.size(), (GLboolean)transpose, floatarray);
    if (n==2 && m==4) glUniformMatrix2x4fv(paramID, floatmatrices.size(), (GLboolean)transpose, floatarray);
    if (n==3 && m==2) glUniformMatrix3x2fv(paramID, floatmatrices.size(), (GLboolean)transpose, floatarray);
    if (n==3 && m==3) glUniformMatrix3fv  (paramID, floatmatrices.size(), (GLboolean)transpose, floatarray);
    if (n==3 && m==4) glUniformMatrix3x4fv(paramID, floatmatrices.size(), (GLboolean)transpose, floatarray);
    if (n==4 && m==2) glUniformMatrix4x2fv(paramID, floatmatrices.size(), (GLboolean)transpose, floatarray);
    if (n==4 && m==3) glUniformMatrix4x3fv(paramID, floatmatrices.size(), (GLboolean)transpose, floatarray);
    if (n==4 && m==4) glUniformMatrix4fv  (paramID, floatmatrices.size(), (GLboolean)transpose, floatarray);

    GuardedUnbind();

    delete floatarray;
}

/** Bind a texture to a uniform sampler2D input-parameter of the fragmentprogram of this pass.
 *
 *  @param[in] parameterName the name of the input-parameter in the fragment program
 *  @param[in] texture the texture
 *  @note must be called _before_ binding the fragment program to have any effect (not while it is bound)
 */
void FragmentProgram::BindTexture(string parameterName, ITextureResourcePtr texture) {
    if (texture.get() == NULL) throw new PPEResourceException("texture was NULL"); // or should it unbind?

    // TODO: check if parameter-name exists in the fragment program

    // check if parameterName was already bound to some texture (if it was: replace it with the supplied texture)
    bool found = false;
    for (int i=0; i<textureBindings.size(); i++) {
	TextureBinding* texbind = textureBindings.at(i);

	if (texbind->parameterName == parameterName) {
	    texbind->texture = texture;
	    found = true;
	}
    }

    // if parameterName wasn't already bound, then add the binding (unless we have reached max number of bindings)
    if (!found) {
	if (textureBindings.size() > maxTextureUnits) logger.error << "can't bind any more textures - ignored" << logger.end;
	else {
	    textureBindings.push_back(new TextureBinding(parameterName, texture));
	    GLint paramID = glGetUniformLocation(programID, parameterName.c_str());
	    if (paramID == -1) logger.error << "uniform \"" << parameterName << "\" does not exist" << logger.end;
	}
    }
}


/** setup texture units according to the recorded texture-bindings
 *  @pre: textureBindings.size() <= maxTextureUnits
 *  @pre: the shader must be bound when this method is called
 */
void FragmentProgram::SetupTextureUnits() {

    for (int i=0; i<textureBindings.size(); i++) {
	TextureBinding* texbind = textureBindings.at(i);

	glActiveTexture(GL_TEXTURE0 + i);
	glBindTexture(GL_TEXTURE_2D, texbind->texture->GetID());
	GLint paramID = glGetUniformLocation(programID, texbind->parameterName.c_str()); // this must be done AFTER fp bind!
	glUniform1i(paramID, i); // texunit i
    }

    glActiveTexture(GL_TEXTURE0);//reset active texture
}


void FragmentProgram::GuardedBind()  {
    if (savedProgID != 0) throw new PPEResourceException("guardedBind: internal error"); // to prevent recursive-ish routines messing up the guardedBind
    glGetIntegerv(GL_CURRENT_PROGRAM, &savedProgID);
    if (programID != (GLuint)savedProgID) // no need to bind if this program is already bound
         glUseProgram(programID);
}

void FragmentProgram::GuardedUnbind() {
    if (programID != (GLuint)savedProgID)
        glUseProgram(savedProgID);
    savedProgID = 0;
}

} // NS Resources
} // NS OpenEngine

