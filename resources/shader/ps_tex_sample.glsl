#version 330 core
out vec4 FragColor;

in vec2 tex_coord;

uniform sampler2D texture_specular1;
uniform sampler2D texture_diffuse1;



void main()
{    
    FragColor = texture(texture_diffuse1, tex_coord);
}