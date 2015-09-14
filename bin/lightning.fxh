/*
	This module required:
	light_source light_sources[];
	bool light_sources_mask[];
	int light_count;
*/

float3 CalculateSpecular(float3 normal, float3 viewer, float3 light, float3 light_color) {
	float3 reflected = reflect(light, normal);
	float specular = saturate(dot(reflected, viewer));
	specular *= pow(specular, 10.0f);
	return light_color * specular;
}

float3 CalculateDiffuse(float3 lightColor, float3 normal, float3 lightVector) {
	float3 diffuse = lightColor * max(0.0f, dot(normal, lightVector));
	return diffuse;
}

/*
	Caution: material.roughness and material.fresnel_reflectance must be set when this function is used.
*/
float3 calculateCookTorrance(light_source input, material_description material, float3 vertex_normal, float3 light_vector, float3 camera_sight){
	float n_dot_l = saturate(dot(vertex_normal, light_vector));
	float check_value = max(.0f, n_dot_l);
	if(check_value == .0f){
		return float3(.0f, .0f, .0f);
	}
	float3 result;
	float3 diffuse;
	float3 specular;
	float3 half_vector = normalize(light_vector + camera_sight);
	float n_dot_h = saturate(dot(vertex_normal, half_vector));
	float n_dot_v = saturate(dot(vertex_normal, camera_sight));
	float v_dot_h = saturate(dot(camera_sight, half_vector));
	float geo_alpha = 2.0f * n_dot_h * n_dot_v / v_dot_h;
	float geo_beta = 2.0f * n_dot_h * n_dot_l / v_dot_h;
	float geometric_factor = min(1.0f, min(geo_alpha, geo_beta));
	/*
		TODO:
		Status: optional.
		Add constant for Gaussian distribution.
		Don't know should it be global or local...
	*/
	/*
		Caution: I hate acos function call in shader code. I will be very happy when this code is removed from the shader.
		But I have no desiree and time to implement texture lookup: so I will suffer.
	*/
	float roughness_square = material.roughness * material.roughness;
	float alpha = acos(n_dot_h);
	float roughness_factor = exp(-alpha / roughness_square);
	float fresnel_factor = material.fresnel_reflectance + pow(1 - v_dot_h, 5) * (1 - material.fresnel_reflectance);
	/*
		Caution: mixing colors: I don't know how to mix colors.
	*/
	diffuse = input.color * material.diffuse_color;
	specular = input.color * material.specular_color;
	specular *= geometric_factor * roughness_factor * fresnel_factor;
	specular /= n_dot_v * n_dot_l;
	result = diffuse + specular;
	result *= check_value;
	return result;
}


const float kf = 1.12f;
const float ks = 1.01f;
const float k = 0.1f;

float fresnel(in float x){
	float a = (x - kf) * (x - kf);
	float b = kf * kf;
	float c = (1 - kf) * (1 - kf);
	float result = 1 / a - 1 / b;
	result /= 1 / c - 1 / b;
	return result;
}

/*
	Caution: this function use variables, which should be global.
*/
float shadow(in float x){
	float a = (1 - ks) * (1 - ks);
	float b = (x - ks) * (x - ks);
	float c = ks * ks;
	float result = 1 / a - 1 / b;
	result /= 1 / a - 1 / c;
	return result;
}

/*
	Caution: material.smoothness, material.metalness and material.fresnel_reflectance must be set when this function is used.
*/
float3 calculateStrauss(light_source input, material_description material, float3 vertex_normal, float3 light_vector, float3 camera_sight){
	float n_dot_l = max(.0f, dot(vertex_normal, light_vector));
	float3 high_light = reflect(-light_vector, vertex_normal);
	float h_dot_v = max(.0f, dot(high_light, camera_sight));
	float n_dot_v = max(.0f, dot(vertex_normal, camera_sight));
	float3 result;
	float3 diffuse;
	float3 specular;
	float3 res_specular;
	float3 white = float3(1.0f, 1.0f, 1.0f);
	float j;
	float factor_one = (1.0f - material.transparency) * (1.0f - pow(material.smoothness, 3.0f));
	float alpha;
	float beta;
	float gamma;
	float delta;
	float r;
	float reflected;
	
	diffuse = input.color * material.diffuse_color;
	specular = input.color * material.specular_color;
	
	diffuse *= n_dot_l * factor_one * (1.0f - material.metalness * material.smoothness);
	alpha = fresnel(n_dot_l);
	beta = shadow(n_dot_l);
	gamma = shadow(n_dot_v);
	specular = white + material.metalness * (1.0f - alpha) * (specular - white);
	delta = 3.0f / (1.0f - material.smoothness);
	specular *= pow(abs(-h_dot_v), delta);
	
	r = (1.0f - material.transparency) - factor_one;
	j = alpha * beta * gamma;
	reflected = r + j * (r + k);
	reflected = min(1.0f, reflected);
	
	specular *= reflected;
	result = diffuse + specular;
	return result;
}

float3 calculatePhong(light_source input, material_description material, float3 vertex_normal, float3 light_vector, float3 camera_sight){
	float3 reflected = float3(.0f, .0f, .0f);
	float3 result = float3(.0f, .0f, .0f);
	float3 diffuse = float3(.0f, .0f, .0f);
	float specular_value = .0f;
	reflected = reflect(-light_vector, vertex_normal);
	specular_value = max(dot(reflected, camera_sight), .0f);
	result = input.color * material.specular_color;
	result *= material.specular * pow(specular_value, material.specular_exp);
	
	diffuse = input.color * material.diffuse_color;
	diffuse *= material.diffuse * max(0.0f, dot(vertex_normal, -light_vector));
	result = result + diffuse;
	return result;
}

/*
	At this moment our calculateLight function works only with rgb color component.
*/
float3 calculateLight(light_source input, material_description material, float3 vertex_position, float3 vertex_normal, float3 camera_position) {
	float3 result = float3(.0f, .0f, .0f);
	float3 black = float3(.0f, .0f, .0f);
	float3 light_vector;
	float3 camera_sight;
	float factor = 1.0f;
	float t = 1.0f;
	light_vector = normalize(vertex_position - input.position);
	switch(input.type){
		case 0:
			t = dot(input.direction, light_vector);
			if(t < input.radius){
				return result;
			}
			switch(input.attenuation){
				case 1:
					factor = (t - input.radius) / (1.0f - input.radius);
					break;
			}
			break;
		case 1:
			t = distance(vertex_position, input.position);
			if(t > input.radius){
				return result;
			}
			switch(input.attenuation){
				case 1:
					factor = (input.radius - t) / input.radius;
					break;
			}
			break;
	}
	switch(input.lightning_model){
		case 0:
			result = input.color * material.ambient_color;
			break;
		case 1:
			result = input.color * material.diffuse_color;
			result *= max(0.0f, dot(-vertex_normal, -light_vector));
			break;
		case 2:
			camera_sight = normalize(vertex_position - camera_position);
			result = calculatePhong(input, material, vertex_normal, light_vector, camera_sight);
			break;
		case 3:
			camera_sight = normalize(vertex_position - camera_position);
			result = calculateCookTorrance(input, material, vertex_normal, -light_vector, -camera_sight);
			break;
		case 4:
			camera_sight = normalize(vertex_position - camera_position);
			result = calculateStrauss(input, material, vertex_normal, -light_vector, -camera_sight);
			break;
	}
	result = lerp(black, result, factor);
	return result;
}

float calculateLightIntensity(light_source input, float3 vertex_position){
	float result = .0f;
	float3 light_vector;
	float t = 1.0f;
	light_vector = normalize(vertex_position - input.position);
	switch(input.type){
		case 0:
			t = dot(input.direction, light_vector);
			if(t < input.radius){
				return result;
			}
			switch(input.attenuation){
				case 1:
					result = (t - input.radius) / (1.0f - input.radius);
					break;
			}
			break;
		case 1:
			t = distance(vertex_position, input.position);
			if(t > input.radius){
				return result;
			}
			switch(input.attenuation){
				case 1:
					result = (input.radius - t) / input.radius;
					break;
			}
			break;
	}
	return result;
}

float3 calculateVertexLight(float3 vertex_position, float3 vertex_normal, float3 camera_position, material_description current_material) {
	float3 result = float3(.0f, .0f, .0f);
	float common_intensity = .0f;
	float3 current = float3(.0f, .0f, .0f);
	int i = 0;
	bool fired = false;
	for(;i < light_count; i++) {
		if(light_sources_mask[i]){
			fired = true;
			common_intensity += light_sources[i].intensity;
		}
	}
	if(!fired){
		return float3(1.0f, 1.0f, 1.0f);
	}
	result = current;
	for(int i = 0; i < light_count; i++) {
		if(light_sources_mask[i]){
			current = calculateLight(light_sources[i], current_material, vertex_position, vertex_normal, camera_position);
			result += current * light_sources[i].intensity;
		}
	}
	return result / common_intensity;
}

float calculateIntensity(float3 vertex_position, float3 vertex_normal){
	float result = .0f;
	float common_intensity = .0f;
	float current = .0f;
	bool fired = false;
	int i = 0;
	for(;i < light_count; i++) {
		if(light_sources_mask[i]){
			fired = true;
			common_intensity += light_sources[i].intensity;
		}
	}
	if(!fired){
		return .0f;
	}
	for(i = 0; i < light_count; i++) {
		if(light_sources_mask[i]){
			current = calculateLightIntensity(light_sources[i], vertex_position);
			result += current * light_sources[i].intensity / common_intensity;
		}
	}
	return result;
}