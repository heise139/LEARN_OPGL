#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 tex_coord;
out vec3 vertex_normal;
out vec3 world_pos;

void main(){
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	tex_coord = aTexCoord;
	vertex_normal = mat3(transpose(inverse(model))) * norm;
	world_pos = (model * vec4(aPos, 1.0)).xyz;
}
