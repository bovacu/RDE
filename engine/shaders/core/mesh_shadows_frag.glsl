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
uniform mat4 light_space_matrix;
uniform mat4 camera_view_matrix;
uniform rde_directional_light directional_light;
uniform float dt;
uniform vec2 mouse_position;

#if RDE_MAX_POINT_LIGHTS > 0
uniform rde_point_light point_lights[RDE_MAX_POINT_LIGHTS];
#endif

#if RDE_MAX_SPOT_LIGHTS > 0
uniform rde_spot_light spot_lights[RDE_MAX_SPOT_LIGHTS];
#endif

uniform rde_material material;
uniform sampler2D tex_ka;
uniform sampler2D tex_kd;
uniform sampler2D tex_ks;
uniform sampler2D tex_bump;
uniform sampler2D render_texture;
uniform sampler2D shadow_map;

vec3 directional_light_calc(float _shadow) {
	vec3 _final_light = vec3(0, 0, 0);
	vec3 _norm = normalize(normal);
	vec3 _diffuse = vec3(0, 0, 0);
	vec3 _specular = vec3(0, 0, 0);
	vec3 _ambient = vec3(0, 0, 0);

	if(material.Ka != vec3(0, 0, 0)) {
		_ambient = directional_light.ambient_color * texture(tex_kd, text_coord).rgb;
	}

	if(material.Kd != vec3(0, 0, 0)) {
		float _diff = dot(normalize(normal), normalize(-directional_light.direction));
		_diffuse = material.Kd * directional_light.diffuse_color * _diff * texture(tex_kd, text_coord).rgb;
	}

	if(material.Ks != vec3(0, 0, 0)) {
		vec3 _view_dir = normalize(camera_pos + frag_pos);
		vec3 _reflect_dir = reflect(normalize(directional_light.direction), _norm);
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

float ShadowCalculation(vec4 frag_pos_light_space) {
    vec3 _proj_coords = frag_pos_light_space.xyz / frag_pos_light_space.w;
    _proj_coords = _proj_coords * 0.5 + 0.5;
    float _closest_depth = texture(shadow_map, _proj_coords.xy).r;
    float _current_depth = _proj_coords.z;

	vec3 _light_dir = normalize(directional_light.direction - frag_pos);
	vec3 _norm = normalize(normal);
	float _bias = mix(0.0005, 0.0, dot(_norm, -_light_dir));

    float _shadow = 0.0;
    vec2 _texel_size = 1.0 / textureSize(shadow_map, 0);

    for(int _x = -1; _x <= 1; ++_x) {
        for(int _y = -1; _y <= 1; ++_y) {
            float _pcf_depth = texture(shadow_map, _proj_coords.xy + vec2(_x, _y) * _texel_size).r;
            _shadow += _current_depth - _bias > _pcf_depth  ? 1.0 : 0.0;
        }
    }

    _shadow /= 20.0;

// 	if(_proj_coords.z > 1.0)
//         _shadow = 0.0;

    return _shadow;
}

void shadow_rendering() {
	if(texture(tex_kd, text_coord).a < 0.05) discard;

	float _shadow = ShadowCalculation(fs_in.frag_pos_light_space);
	vec3 _final_light = vec3(0.0);

	_final_light += directional_light_calc(_shadow);

	color_out = vec4(_final_light, 1.0);
}

void render_texture_rendering() {
	color_out = texture(render_texture, text_coord);
}

void main(){
	if(material.using_render_texture == 0) {
		shadow_rendering();
	} else {
		render_texture_rendering();
	}
}
