float3 rgb_to_hsv(float3 color){
	float3 hsv;
	float max_rgb = max(color.r, color.g);
	max_rgb = max(max_rgb, color.b);
	float min_rgb = min(color.r, color.g);
	min_rgb = min(min_rgb, color.b);
	float delta = (max_rgb - min_rgb);
	hsv.y = .0f;
	if(max_rgb != .0f){
		hsv.y = 1 - min_rgb / max_rgb;
	}
	hsv.z = max_rgb;
	if(max_rgb == min_rgb){
		hsv.x = .0f;
	} else {
		if(max_rgb == color.r){
			if(color.g >= color.b){
				hsv.x = 60.0f * (color.g - color.b) / delta;
			} else {
				hsv.x = 360.0f + 60.0f * (color.g - color.b) / delta;
			}
		} else {
			if(max_rgb = color.g){
				hsv.x = 120.0f + 60.0f * (color.b - color.r) / delta;
			} else {
				hsv.x = 240.0f + 60.0f * (color.r - color.g) / delta;
			}
		}
	}
	return hsv;
}

float3 hsv_to_rgb(float3 hsv){
	float3 color;
	float temp = hsv.x / 60.0f;
	float f_temp = floor(temp);
	float h = fmod(f_temp, 6.0f);
	float f = temp - f_temp;
	float p = hsv.z * (1.0f - hsv.y);
	float q = hsv.z * (1.0f - f * hsv.y);
	float t = hsv.z * (1.0f - (1.0f - f) * hsv.y);
	float v = hsv.z;
	switch(h){
		case .0f:
			color = float3(v, t, p);
			break;
		case 1.0f:
			color = float3(q, v, p);
			break;
		case 2.0f:
			color = float3(p, v, t);
			break;
		case 3.0f:
			color = float3(p, q, v);
			break;
		case 4.0f:
			color = float3(t, p, v);
			break;
		case 5.0f:
			color = float3(v, p, q);
			break;
	}
	return color;
}

float3 change_saturate(float3 color, float factor){
	float3 xyz = mul(rgb_to_xyz, color);
	xyz.y *= factor;
	return mul(xyz_to_rgb, xyz);
}

float3 convert_to_monochrome(float3 color){
	float3 hsv = rgb_to_hsv(color);
	hsv.y = .0f;
	return hsv_to_rgb(hsv);
}

SamplerState BlurSampler{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Clamp;
	AddressV = Clamp;
};

float3 x_blur(float2 texcoord, float radius){
	float delta;
	float2 current;
	float3 result = image.Sample(BlurSampler, texcoord).rgb;
	int i;
	delta = radius / scene_size.x;
	current.y = texcoord.y;
	current.x = texcoord.x - delta / 2.0f;
	for(i = 0; i < 8; i++){
		current.x += delta / 8.0f;
		result += image.Sample(BlurSampler, current).rgb;
	}
	return result / 9.0f;
}

float3 y_blur(float2 texcoord, float radius){
	float delta;
	float2 current;
	float3 result = image.Sample(BlurSampler, texcoord).rgb;
	int i;
	delta = radius / scene_size.y;
	current.y = texcoord.y - delta / 2.0f;
	current.x = texcoord.x;
	for(i = 0; i < 8; i++){
		current.y += delta / 8.0f;
		result += image.Sample(BlurSampler, current).rgb;
	}
	return result / 9.0f;
}

float3 dof(float2 texcoord, bool mode){
	float depth = depth_image.Sample(LinearSampler, texcoord);
	float radius = 1.0f;
	float2 delta;
	float2 current;
	float3 result = float3(.0f, .0f, .0f);
	if(depth < focus_data.x){
		radius = .0f;
	} else {
		if(depth < focus_data.y){
			radius = (depth - focus_data.y) / (focus_data.x - focus_data.y);
			radius = 1.0f - abs(radius);
		} else {
			if(depth < focus_data.z){
				radius = (depth - focus_data.z) / (focus_data.z - focus_data.y);
				radius = abs(radius);
			} else {
				radius = .0f;
			}
		}
	}
	if(mode){
		return x_blur(texcoord, 24.0f * radius);
	}
	return y_blur(texcoord, 24.0f * radius);
}

float4 post_effect_manager(float2 texcoord){
	float4 result = image.Sample(LinearSampler, texcoord);
	int i = 0;
	for(i = 0; i < post_effect_count; i++){
		switch(post_effects[i].type){
			case 0:
				result.rgb = convert_to_monochrome(result.rgb);
				break;
			case 1:
				result.rgb = change_saturate(result.rgb, post_effects[i].alpha);
				break;
		}
	}
	return result;
}