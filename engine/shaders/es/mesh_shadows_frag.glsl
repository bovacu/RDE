#version 300 es
precision mediump float;

#define RDE_MAX_POINT_LIGHTS __point_lights__
#define RDE_MAX_SPOT_LIGHTS __spot_lights__

in vec3 normal;
in vec2 text_coord;
in vec3 frag_pos;
in vec4 frag_pos_light_space;
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

uniform vec3 camera_pos;
uniform rde_directional_light directional_light;
uniform float dt;
uniform vec2 mouse_position;

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
	vec3 _norm = normalize(normal);
	vec3 _diffuse = vec3(0, 0, 0);
	vec3 _specular = vec3(0, 0, 0);
	vec3 _ambient = vec3(0, 0, 0);

	_ambient = directional_light.ambient_color * texture(tex_kd, text_coord).rgb;

	float _diff = dot(normalize(normal), normalize(-directional_light.direction));
    _diffuse = material.Kd * directional_light.diffuse_color * _diff * texture(tex_kd, text_coord).rgb;

	vec3 _view_dir = normalize(camera_pos + frag_pos);
    vec3 _reflect_dir = reflect(normalize(directional_light.direction), _norm);
    float _spec = pow(max(dot(_view_dir, _reflect_dir), 0.0), material.shininess);
    _specular = material.Ks * directional_light.specular_color * _spec * texture(tex_ks, text_coord).rgb;

	_final_light = _ambient + (1.0 - _shadow) * (_diffuse + _specular);

	return _final_light;
}

#if RDE_MAX_POINT_LIGHTS
#endif

#if RDE_MAX_SPOT_LIGHTS
#endif

float ShadowCalculation(vec4 frag_pos_light_space) {
    vec3 _proj_coords = vec3(frag_pos_light_space.x / frag_pos_light_space.w, frag_pos_light_space.y / frag_pos_light_space.w, frag_pos_light_space.z / frag_pos_light_space.w);
    _proj_coords = _proj_coords * 0.5 + 0.5;
    float _closest_depth = texture(shadow_map, _proj_coords.xy).r;
    float _current_depth = _proj_coords.z;

	vec3 _light_dir = normalize(directional_light.direction - frag_pos);
	vec3 _norm = normalize(normal);
	float _bias = mix(0.0005, 0.0, dot(_norm, -_light_dir));

    float _shadow = 0.0;
    ivec2 _texture_size = textureSize(shadow_map, 0);
    vec2 _texel_size = vec2(1.0 / float(_texture_size.x), 1.0 / float(_texture_size.y));

    for(int _x = -1; _x <= 1; ++_x) {
        for(int _y = -1; _y <= 1; ++_y) {
            float _pcf_depth = texture(shadow_map, _proj_coords.xy + vec2(_x, _y) * _texel_size).r;
            _shadow += _current_depth - _bias > _pcf_depth  ? 1.0 : 0.0;
        }
    }

    _shadow /= 20.0;

	if(_proj_coords.z > 1.0)
        _shadow = 0.0;

    return _shadow;
}

void main(){
	if(texture(tex_kd, text_coord).a < 0.05) discard;

	float _shadow = ShadowCalculation(frag_pos_light_space);
	vec3 _final_light = vec3(0.0);

	_final_light += directional_light_calc(_shadow);

	color_out = vec4(_final_light, 1.0);
}
