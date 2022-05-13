#version 410 core
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec2 tex_coord;

uniform mat4 mvp;
out vec2 TexCoord;

void main() {
	gl_Position = mvp*vec4(vertex_position, 1);
	TexCoord = tex_coord;
}
