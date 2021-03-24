#version 460

#define MAX_POINT_LIGHTS 1

struct Material {
	sampler2D diffuse;
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

uniform sampler2D shadow_texture;

uniform vec3 world_camera_pos;

vec3 calculate_directional_light(DirectionalLight light, vec3 normal, vec3 view_dir);
vec3 calculate_point_light(PointLight light, vec3 normal, vec3 view_dir);
vec3 calculate_spot_light(SpotLight light, vec3 normal, vec3 view_dir);
float calc_shadow(vec4 light_pos, vec3 norm, vec3 light_dir);

void main() {
	vec3 normal = normalize(norm0);
	vec3 view_dir = normalize(world_pos0 - world_camera_pos);

	vec3 light = vec3(0.0);
	light += calculate_directional_light(directional_light, normal, view_dir);

	for (int i = 0; i < MAX_POINT_LIGHTS; ++i) {
// 		light += calculate_point_light(point_lights[0], normal, view_dir);
	}

// 	light += calculate_spot_light(spot_light, normal, view_dir);

	out_color = vec4(light , 1.0);
}

vec3 calculate_directional_light(DirectionalLight light, vec3 normal, vec3 view_dir) {
	vec3 halfway_dir = normalize(-view_dir + -(light.dir));

	vec3 ambient = light.ambient_intensity *
	vec3(texture(material.diffuse, tex0));

	float shadow = calc_shadow(light_pos0, normal, -light.dir);

	vec3 diffuse = light.diffuse_intensity *
	vec3(max(dot(normal, -light.dir), 0.0)) *
	vec3(texture(material.diffuse, tex0));

	vec3 specular = light.specular_intensity *
	vec3(pow(max(dot(normal, halfway_dir), 0.0), material.shininess) *
	vec3(1.));

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
	vec3(1.0))
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
	vec3(1.0))
	* attenuation
	* intensity;

	return ambient + diffuse + specular;
}

float calc_shadow(vec4 light_pos, vec3 norm, vec3 light_dir) {

	vec3 pos = light_pos.xyz / light_pos.w;
	if (pos.z > 1.0) return 0.0;

	pos = pos * 0.5 + 0.5;

	float current_depth = pos.z;

	float closest_depth = texture(shadow_texture, pos.xy).r;

	float bias = max(0.05 * (1.0 - dot(norm, light_dir)), 0.005);
	float shadow = 0.0;
	vec2 texel_size = 1.0 / textureSize(shadow_texture, 0);

	for (int x = -5; x <= 5; ++x) {
		for (int y = -5; y <= 5; ++y) {
			float pcf_depth = texture(shadow_texture, pos.xy + vec2(x, y) * texel_size).r;
			shadow += current_depth - bias > pcf_depth ? 1.0 : 0.0;
		}
	}

	return shadow / 121.0;
}
