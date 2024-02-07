#version 330 core

#define RDE_MAX_POINT_LIGHTS __point_lights__
#define RDE_MAX_SPOT_LIGHTS __spot_lights__

in vec3 normal;
in vec2 text_coord;
in vec3 frag_pos;
out vec4 color_out;

struct rde_material {
	float shininess;
	vec3 Ka;
	vec3 Kd;
	vec3 Ks;
	int using_render_texture;
};
struct rde_directional_light {
	vec3 direction;
	vec3 position;
	vec3 ambient_color;
	vec3 diffuse_color;
	vec3 specular_color;
};
struct rde_point_light {
	vec3 position;
	vec3 ambient_color;
	vec3 diffuse_color;
	vec3 specular_color;
	float constant;
	float linear;
	float quadratic;
	int used;
};
struct rde_spot_light {
	vec3 position;
	vec3 direction;
	vec3 ambient_color;
	vec3 diffuse_color;
	vec3 specular_color;
	float constant;
	float linear;
	float quadratic;
	float cut_off;
	float outer_cut_off;
	int used;
};
in VS_OUT {
    vec3 frag_pos;
    vec3 normal;
    vec4 frag_pos_light_space;
} fs_in;


uniform vec3 camera_pos;
uniform rde_directional_light directional_light;
uniform float dt;
uniform vec2 mouse_position;
uniform mat4 light_space_matrix;

#if RDE_MAX_POINT_LIGHTS > 0
uniform rde_point_light point_lights[RDE_MAX_POINT_LIGHTS];
#endif

#if RDE_MAX_SPOT_LIGHTS > 0
uniform rde_spot_light spot_lights[RDE_MAX_SPOT_LIGHTS];
#endif

uniform int use_shadows;
uniform rde_material material;
uniform sampler2D tex_ka;
uniform sampler2D tex_kd;
uniform sampler2D tex_ks;
uniform sampler2D tex_bump;
uniform sampler2D render_texture;
uniform sampler2D shadow_map;

vec3 directional_light_calc(float _shadow) {
	vec3 _final_light = vec3(0, 0, 0);
	vec3 _light_dir = normalize(directional_light.position - fs_in.frag_pos);
	vec3 _norm = normalize(fs_in.normal);
	vec3 _diffuse = vec3(0, 0, 0);
	vec3 _specular = vec3(0, 0, 0);
	vec3 _ambient = vec3(0, 0, 0);

	if(material.Ka != vec3(0, 0, 0)) {
		vec3 _ambient = material.Ka * directional_light.ambient_color * texture(tex_kd, text_coord).rgb;
	}


	if(material.Kd != vec3(0, 0, 0)) {
		float _diff = max(dot(_light_dir, _norm), 0.0);
		_diffuse = material.Kd * directional_light.diffuse_color * _diff * texture(tex_kd, text_coord).rgb;
	}

	if(material.Ks != vec3(0, 0, 0)) {
		vec3 _view_dir = normalize(camera_pos + frag_pos);
		vec3 _reflect_dir = reflect(-_light_dir, _norm);
		float _spec = pow(max(dot(_view_dir, _reflect_dir), 0.0), material.shininess);
		_specular = material.Ks * directional_light.specular_color * _spec * texture(tex_ks, text_coord).rgb;
	}

	_final_light = _ambient + (1.0 - _shadow) * (_diffuse + _specular);

	return _final_light;
}

#if RDE_MAX_POINT_LIGHTS
#endif

#if RDE_MAX_SPOT_LIGHTS
#endif

float ShadowCalculation(vec4 fragPosLightSpace) {
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    // Sample the closest depth value from the shadow map
    float closestDepth = texture(shadow_map, projCoords.xy).r; 
    
    float currentDepth = projCoords.z;
    
    // Calculate bias (adjust as needed)
    float bias = mix(0.005, 0.0, dot(normalize(normal), -directional_light.direction));
    
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadow_map, 0);
    
    // Adjust the kernel size according to your requirements
    const int kernelSize = 3; // 3x3 kernel for PCF
    
    for(int x = -kernelSize; x <= kernelSize; ++x) {
        for(int y = -kernelSize; y <= kernelSize; ++y) {
            // Sample depth value from the shadow map
            float pcfDepth = texture(shadow_map, projCoords.xy + vec2(x, y) * texelSize).r; 
            
            // Apply depth bias
            pcfDepth -= bias;
            
            // Increment shadow if current fragment is in shadow
            shadow += currentDepth > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    
    // Divide by the total number of samples in the kernel
    shadow /= float((2 * kernelSize + 1) * (2 * kernelSize + 1));
    
    // Clamp shadow value to avoid artifacts
    shadow = clamp(shadow, 0.0, 1.0);
    
    // Check if the fragment is outside the view frustum (optional)
    if(projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}

void main(){
	if(texture(tex_kd, text_coord).a < 0.05) discard;

	float _shadow = ShadowCalculation(fs_in.frag_pos_light_space);
	vec3 _final_light = vec3(0.0);

	_final_light += directional_light_calc(_shadow);

	color_out = vec4(_final_light, 1.0);
}
