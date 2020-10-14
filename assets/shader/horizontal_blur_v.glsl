attribute vec4 al_pos;
uniform mat4 al_projview_matrix;
varying vec4 blurTextureCoords[11];

void main()
{
    gl_Position = al_projview_matrix*al_pos;
    vec4 centerTexCoords = gl_Position*0.5 + 0.5;
    float pixelSize = 1.0/192;

    for(int i=-5; i<=5; i++)
        blurTextureCoords[i+5] = centerTexCoords + vec4(pixelSize*i, 0.0, 0.0, 0.0);
}