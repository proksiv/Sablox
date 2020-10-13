uniform sampler2D al_tex;
varying vec4 varying_color;
varying vec2 varying_texcoord;

void main()
{
    vec4 color = varying_color;
    float brightness = color.r*0.2126 + color.g*0.7152 + color.b*0.0722;
    gl_FragColor = color*brightness;
}