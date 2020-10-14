uniform sampler2D al_tex;
varying vec4 varying_color;
varying vec2 varying_texcoord;

void main()
{
    vec4 color = varying_color*texture2D(al_tex, varying_texcoord);
    if(color.r >= 0.8 || color.g >= 0.8)
        gl_FragColor = color;
    else
        gl_FragColor = vec4(0.0);
}