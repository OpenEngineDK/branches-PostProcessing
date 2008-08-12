uniform sampler2D parentColorBuf;
uniform sampler2D parentDepthBuf;
uniform sampler2D childColorBuf;
uniform sampler2D childDepthBuf;

void main() {

    vec2 texcoord = gl_TexCoord[0].xy;
    
    vec4  parentColor = texture2D(parentColorBuf, texcoord);
    float parentDepth = texture2D(parentDepthBuf, texcoord);
    vec4  childColor  = texture2D(childColorBuf , texcoord);
    float childDepth  = texture2D(childDepthBuf , texcoord);
    
    if (parentDepth > childDepth) {
        gl_FragColor = childColor;
        gl_FragDepth = childDepth;
    } else {
        gl_FragColor = parentColor;
        gl_FragDepth = parentDepth;    
    }
}