#version 330 core
out vec4 FragColor;

struct DirLight{
	vec3 direction;
	vec3 color;
	float intensity;
};

struct PointLight{
	vec3 position;
	vec3 color;
	float intensity;

	float constant;
	float linear;
	float quadratic;
};
#define NR_POINT_LIGHTS 4

struct SpotLight{
	vec3 position;
	vec3 direction;
	vec3 color;
	float intensity;

	float inner_angle_cos;
	float outter_angle_cos;
	float constant;
	float linear;
	float quadratic;
};

uniform PointLight point_light[NR_POINT_LIGHTS] = PointLight[NR_POINT_LIGHTS](
												PointLight(vec3(0, 0, 0), vec3(1, 1, 1), 0, 1, 1, 1),
												PointLight(vec3(0, 0, 0), vec3(1, 1, 1), 0, 1, 1, 1),
											  	PointLight(vec3(0, 0, 0), vec3(1, 1, 1), 0, 1, 1, 1),
												PointLight(vec3(0, 0, 0), vec3(1, 1, 1), 0, 1, 1, 1));
uniform DirLight dir_light = {vec3(-1, -1, -1), vec3(1, 1, 1), 0};
uniform SpotLight spot_light = {vec3(0, 0, 0), vec3(-1, -1, -1), vec3(1, 1, 1), 0, 0.5, 1, 1, 1, 1};
uniform vec3 view_pos;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
uniform float shininess = 8;

in vec2 tex_coord;
in vec3 vertex_normal;
in vec3 world_pos;

vec3 calc_direction_light(DirLight light, vec3 normal, vec3 world_pos, vec3 view_dir){
	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 reflect_dir = reflect(light.direction, normal);
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), shininess);
	vec3 diffuse = light.color * diff * vec3(texture(texture_diffuse, tex_coord));
	vec3 specular = light.color * spec * vec3(texture(texture_specular, tex_coord));
	return (diffuse + specular) * light.intensity;
}

vec3 calc_point_light(PointLight light, vec3 normal, vec3 world_pos, vec3 view_dir){
	vec3 light_dir = normalize(light.position - world_pos);
	float diff = max(dot(normal, light_dir), 0.0);
	vec3 reflect_dir = reflect(-light_dir, normal);
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), shininess);
	float _distance = length(light.position - world_pos);
	float attenuation = 1.0 / (light.constant + light.linear * _distance + light.quadratic * (_distance * _distance));

	vec3 diffuse = light.color * diff * vec3(texture(texture_diffuse, tex_coord));
	vec3 specular = light.color * spec * vec3(texture(texture_specular, tex_coord));
	return (diffuse + specular) * attenuation * light.intensity;
}

vec3 calc_spot_light(SpotLight light, vec3 normal, vec3 world_pos, vec3 view_dir){
	vec3 light_dir = normalize(light.position - world_pos);
	float diff = max(dot(normal, light_dir), 0.0);
	vec3 reflect_dir = reflect(-light_dir, normal);
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), shininess);
	float _distance = length(light.position - world_pos);
	float attenuation = 1.0 / (light.constant + light.linear * _distance + light.quadratic * (_distance * _distance));
	float theta = dot(light_dir, normalize(-light.direction));
	float epsilon = light.inner_angle_cos - light.outter_angle_cos;
	float _intensity = clamp((theta - light.outter_angle_cos) / epsilon, 0.0, 1.0);

	vec3 diffuse = light.color * diff * vec3(texture(texture_diffuse, tex_coord));
	vec3 specular = light.color * spec * vec3(texture(texture_specular, tex_coord));
	return (diffuse + specular) * attenuation * _intensity * light.intensity;
}


void main()
{    
 //   FragColor = texture(texture_diffuse, tex_coord);
	vec3 out_color = vec3(texture(texture_diffuse, tex_coord));
 	vec3 view_dir = normalize(view_pos - world_pos);
	out_color += calc_direction_light(dir_light, vertex_normal, world_pos, view_dir);
	for(int i = 0; i < NR_POINT_LIGHTS; i++){
		out_color += calc_point_light(point_light[i], vertex_normal, world_pos, view_dir);
	}
	out_color += calc_spot_light(spot_light, vertex_normal, world_pos, view_dir);
	FragColor = vec4(out_color, 1.0f);
}