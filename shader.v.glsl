#version 300 es

in vec4 in_pos;
in vec2 in_tex_coord;

out vec2 vs_tex_coord;

void main(void)
{
   // dont transform model coords yet
   gl_Position = in_pos;
   
   // feed the texture coordinates through
   vs_tex_coord = in_tex_coord;
}
