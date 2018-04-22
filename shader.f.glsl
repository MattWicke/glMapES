uniform sampler2D tex;
varying vec2 vs_tex_coord;

void main(void)
{
   gl_FragColor = texture2D( tex, vs_tex_coord);
   //gl_FragColor = vec4(1.0,1.0,1.0,1.0) * texture2D( tex, vs_tex_coord , 0.0);
   //gl_FragColor = vec4(1.0,1.0,1.0,1.0);
}
