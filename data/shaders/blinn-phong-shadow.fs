#version 460

#define MAX_POINT_LIGHTS 1

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

struct DirectionalLight {
	vec3 color;
	vec3 dir;

	float ambient_intensity;
	float diffuse_intensity;
	float specular_intensity;
};

struct PointLight {
	vec3 pos;
	vec3 color;

	float ambient_intensity;
	float diffuse_intensity;
	float specular_intensity;

	float constant;
	float linear;
	float quadratic;
};

struct SpotLight {
	vec3 pos;
	vec3 color;
	vec3 dir;

	float cut_off;
	float outer_cut_off;

	float ambient_intensity;
    float diffuse_intensity;
    float specular_intensity;

	float constant;
	float linear;
	float quadratic;
};

in vec3 world_pos0;
in vec3 norm0;
in vec2 tex0;
in vec4 light_pos0;

out vec4 out_color;

uniform DirectionalLight directional_light;
uniform PointLight point_lights[MAX_POINT_LIGHTS];
uniform SpotLight spot_light;

uniform Material material;

uniform samplerCube skybox;
uniform sampler2D shadow_texture;

uniform vec3 world_camera_pos;

vec3 calculate_directional_light(DirectionalLight light, vec3 normal, vec3 view_dir);
vec3 calculate_point_light(PointLight light, vec3 normal, vec3 view_dir);
vec3 calculate_spot_light(SpotLight light, vec3 normal, vec3 view_dir);
float calc_shadow(vec4 light_pos);

void main() {
	vec3 normal = normalize(norm0);
	vec3 view_dir = normalize(world_pos0 - world_camera_pos);

	vec3 light = vec3(0.0);
	light += calculate_directional_light(directional_light, normal, view_dir);

	for (int i = 0; i < MAX_POINT_LIGHTS; ++i) {
// 		light += calculate_point_light(point_lights[0], normal, view_dir);
	}

// 	light += calculate_spot_light(spot_light, normal, view_dir);

// 	vec3 reflected_view_dir = reflect(view_dir, normal);
// 	vec3 refracted_view_dir = refract(view_dir, normal, 1.0 / 1.52);

// 	out_color = vec4(texture(skybox, refracted_view_dir).xyz , 1.0);
	out_color = vec4(light , 1.0);
}

vec3 calculate_directional_light(DirectionalLight light, vec3 normal, vec3 view_dir) {
	vec3 halfway_dir = normalize(-view_dir + -(light.dir));

	vec3 ambient = light.ambient_intensity *
	vec3(texture(material.diffuse, tex0));

	float shadow = calc_shadow(light_pos0);

	vec3 diffuse = light.diffuse_intensity *
	vec3(max(dot(normal, -light.dir), 0.0)) *
	vec3(texture(material.diffuse, tex0));

	vec3 specular = light.specular_intensity *
	vec3(pow(max(dot(normal, halfway_dir), 0.0), material.shininess) *
	vec3(texture(material.specular, tex0)));

	return ambient + (1.0 - shadow) * (diffuse + specular);
}

vec3 calculate_point_light(PointLight light, vec3 normal, vec3 view_dir) {
	vec3 frag_to_light = light.pos - world_pos0;
	float distance = length(frag_to_light);
	vec3 frag_to_lightn = normalize(frag_to_light);

	vec3 halfway_dir = normalize(-view_dir + frag_to_lightn);

	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

	vec3 ambient = light.ambient_intensity *
	vec3(texture(material.diffuse, tex0))
	* attenuation;

	vec3 diffuse = light.diffuse_intensity *
	vec3(max(dot(normal, frag_to_lightn), 0.0)) *
	vec3(texture(material.diffuse, tex0))
	* attenuation;

	vec3 specular = light.specular_intensity *
	vec3(pow(max(dot(normal, halfway_dir), 0.0), material.shininess) *
	vec3(texture(material.specular, tex0)))
	* attenuation;

	return ambient + diffuse + specular;
}

vec3 calculate_spot_light(SpotLight light, vec3 normal, vec3 view_dir) {
	vec3 frag_to_light = light.pos - world_pos0;
	float distance = length(frag_to_light);
	vec3 frag_to_lightn = normalize(frag_to_light);

	float theta = dot(frag_to_lightn, normalize(-light.dir));
	float epsilon = light.cut_off - light.outer_cut_off;
	float intensity = clamp((theta - light.outer_cut_off) / epsilon, 0.0, 1.0);

	vec3 halfway_dir = normalize(-view_dir + frag_to_lightn);

	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

	vec3 ambient = light.ambient_intensity *
	vec3(texture(material.diffuse, tex0))
	* attenuation;

	vec3 diffuse = light.diffuse_intensity *
	vec3(max(dot(normal, frag_to_lightn), 0.0)) *
	vec3(texture(material.diffuse, tex0))
	* attenuation
	* intensity;

	vec3 specular = light.specular_intensity *
	vec3(pow(max(dot(normal, halfway_dir), 0.0), material.shininess) *
	vec3(texture(material.specular, tex0)))
	* attenuation
	* intensity;

	return ambient + diffuse + specular;
}

float calc_shadow(vec4 light_pos) {
	vec3 pos = light_pos.xyz / light_pos.w;

	pos = pos * 0.5 + 0.5;

	float shadow = pos.z > texture(shadow_texture, pos.xy).x ? 1.0 : 0.0;

	return shadow;
}
