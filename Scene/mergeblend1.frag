uniform sampler2D parentDepthBuf;
uniform sampler2D childColorBuf;
uniform sampler2D childDepthBuf;
uniform vec4      clearcolor;

void main() {

    vec2 texcoord = gl_TexCoord[0].xy;
    
    float parentDepth = texture2D(parentDepthBuf, texcoord).x;
    vec4  childColor  = texture2D(childColorBuf , texcoord);
    float childDepth  = texture2D(childDepthBuf , texcoord).x;
    
    if (parentDepth >= childDepth) {
        gl_FragColor = childColor;
        gl_FragDepth = childDepth;
    } else {
        gl_FragColor = clearcolor;
        gl_FragDepth = 1.0;    
    }
}