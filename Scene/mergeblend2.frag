uniform sampler2D parentColorBuf;
uniform sampler2D parentDepthBuf;
uniform sampler2D childColorBuf;
uniform sampler2D childDepthBuf;
uniform int blendMethod;

void main() {

    vec2 texcoord = gl_TexCoord[0].xy;
    
    vec4  parentColor = texture2D(parentColorBuf, texcoord);
    float parentDepth = texture2D(parentDepthBuf, texcoord).x;
    vec4  childColor  = texture2D(childColorBuf , texcoord);
    float childDepth  = texture2D(childDepthBuf , texcoord).x;

    gl_FragColor = childColor * childColor.a + parentColor * (1.0 - childColor.a);
    gl_FragDepth = min(childDepth, parentDepth);
}