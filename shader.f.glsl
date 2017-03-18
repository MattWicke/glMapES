#version 300 es
uniform sampler2D tex;
precision mediump float;
in vec2 vs_tex_coord;
out vec4 fColor;

void main(void)
{
   fColor = texture( tex, vs_tex_coord);
}
