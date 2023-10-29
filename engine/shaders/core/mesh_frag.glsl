#version 330 core

const int RDE_MAX_POINT_LIGHTS = 10;
const int RDE_MAX_SPOT_LIGHTS = 10;

in vec3 normal;
in vec2 text_coord;
in vec3 frag_pos;
out vec4 color_out;

struct rde_material {
	float shininess;
	vec3 Ka;
	vec3 Kd;
	vec3 Ks;
	sampler2D tex_ka;
	sampler2D tex_kd;
	sampler2D tex_ks;
	sampler2D tex_bump;
};
struct rde_directional_light {
	vec3 direction;
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
uniform rde_point_light point_lights[RDE_MAX_POINT_LIGHTS];
uniform rde_spot_light spot_lights[RDE_MAX_SPOT_LIGHTS];
uniform rde_material material;

vec3 directional_light_calc() {
	vec3 _ambient = material.Ka * directional_light.ambient_color * texture(material.tex_kd, text_coord).rgb;
	
	vec3 _norm = normalize(normal);
	vec3 _light_dir = normalize(-directional_light.direction);
	float _diff = max(dot(_norm, _light_dir), 0.0);
	vec3 _diffuse = material.Kd * directional_light.diffuse_color * _diff * texture(material.tex_kd, text_coord).rgb;
	
	vec3 _view_dir = normalize(camera_pos + frag_pos);
	vec3 _reflect_dir = reflect(-_light_dir, _norm);
	float _spec = pow(max(dot(_view_dir, _reflect_dir), 0.0), material.shininess);
	vec3 _specular = material.Ks * directional_light.specular_color * _spec * texture(material.tex_ks, text_coord).rgb;
	vec3 _final_light = _ambient + _diffuse + _specular;
	return _final_light;
}

vec3 point_light_calc(int _i) {
	rde_point_light _light = point_lights[_i];
	if(_light.used < 0) return vec3(0.0, 0.0, 0.0);
	
	vec3 _ambient = material.Ka * _light.ambient_color * texture(material.tex_kd, text_coord).rgb;
	
	vec3 _norm = normalize(normal);
	vec3 _light_dir = normalize(_light.position - frag_pos);
	float _diff = max(dot(_norm, _light_dir), 0.0);
	vec3 _diffuse = material.Kd * _light.diffuse_color * _diff * texture(material.tex_kd, text_coord).rgb;
	
	vec3 _view_dir = normalize(camera_pos + frag_pos);
	vec3 _reflect_dir = reflect(-_light_dir, _norm);
	float _spec = pow(max(dot(_view_dir, _reflect_dir), 0.0), material.shininess);
	vec3 _specular = material.Ks * _light.specular_color * _spec * texture(material.tex_ks, text_coord).rgb;
	float _distance = length(_light.position - frag_pos);
	float _attenuation = 1.0 / (_light.constant + _light.linear * _distance + _light.quadratic * (_distance * _distance));
	_ambient  *= _attenuation;
	_diffuse  *= _attenuation;
	_specular *= _attenuation;
	vec3 _final_light = _ambient + _diffuse + _specular;
	return _final_light;
}
vec3 spot_light_calc(int _i) {
	rde_spot_light _light = spot_lights[_i];
	if(_light.used < 0) return vec3(0.0, 0.0, 0.0);
	
	vec3 _light_dir = normalize(_light.position - frag_pos);
	float _theta = dot(_light_dir, normalize(-_light.direction));
	float _epsilon = (_light.cut_off - _light.outer_cut_off);
	float _intensity = clamp((_theta - _light.outer_cut_off) / _epsilon, 0.0, 1.0);
	
	vec3 _ambient = material.Ka * _light.ambient_color * texture(material.tex_kd, text_coord).rgb;
	
	vec3 _norm = normalize(normal);
	float _diff = max(dot(_norm, _light_dir), 0.0);
	vec3 _diffuse = material.Kd * _light.diffuse_color * _diff * texture(material.tex_kd, text_coord).rgb;
	
	vec3 _view_dir = normalize(camera_pos + frag_pos);
	vec3 _reflect_dir = reflect(-_light_dir, _norm);
	float _spec = pow(max(dot(_view_dir, _reflect_dir), 0.0), material.shininess);
	vec3 _specular = material.Ks * _light.specular_color * _spec * texture(material.tex_ks, text_coord).rgb;
	float _distance = length(_light.position - frag_pos);
	float _attenuation = 1.0 / (_light.constant + _light.linear * _distance + _light.quadratic * (_distance * _distance));
	_ambient  *= _attenuation;
	_diffuse  *= _attenuation * _intensity;
	_specular *= _attenuation * _intensity;
	vec3 _final_light = _ambient + _diffuse + _specular;
	return _final_light;
}

void main(){
	if(texture(material.tex_kd, text_coord).a < 0.05) discard;
	vec3 _final_light = vec3(0.0);
	_final_light += directional_light_calc();
	for(int _i = 0; _i < RDE_MAX_POINT_LIGHTS; _i++) {
		_final_light += point_light_calc(_i);
	}
	for(int _i = 0; _i < RDE_MAX_SPOT_LIGHTS; _i++) {
		_final_light += spot_light_calc(_i);
	}
	color_out = vec4(_final_light, 1.0);
}