varying vec4 varying_color;
varying vec4 blurTextureCoords[11];
uniform sampler2D al_tex;

void main(void){
	gl_FragColor = vec4(0.0);
	gl_FragColor += texture2D(al_tex, blurTextureCoords[0]) * 0.0093;
    gl_FragColor += texture2D(al_tex, blurTextureCoords[1]) * 0.028002;
    gl_FragColor += texture2D(al_tex, blurTextureCoords[2]) * 0.065984;
    gl_FragColor += texture2D(al_tex, blurTextureCoords[3]) * 0.121703;
    gl_FragColor += texture2D(al_tex, blurTextureCoords[4]) * 0.175713;
    gl_FragColor += texture2D(al_tex, blurTextureCoords[5]) * 0.198596;
    gl_FragColor += texture2D(al_tex, blurTextureCoords[6]) * 0.175713;
    gl_FragColor += texture2D(al_tex, blurTextureCoords[7]) * 0.121703;
    gl_FragColor += texture2D(al_tex, blurTextureCoords[8]) * 0.065984;
    gl_FragColor += texture2D(al_tex, blurTextureCoords[9]) * 0.028002;
    gl_FragColor += texture2D(al_tex, blurTextureCoords[10]) * 0.0093;
}