/*
	This module requires:
	TODO: full documentation for this module
*/

float calculateFog(fog input, float3 vertex_position, float3 camera_position) {
	float distance = dot(input.scissor_plane.xyz, vertex_position) + input.scissor_plane.w;
	float alpha = .0f;
	float result = .0f;
	if(distance >= 0)
		return .0f;
	switch(input.type){
		case 0:
			alpha = distance * input.argument;
			result = 1.0f - exp(-alpha * alpha);
			break;
	}
	return result;
}

float3 fogVertex(float3 vertex_color, float3 vertex_position, float3 camera_position) {
	float3 result = float3(.0f, .0f, .0f);
	float counter = .0f;
	float current = .0f;
	int i = 0;
	bool fired = false;
	for(i = 0; i < fog_count; i++) {
		if(fog_mask[i]){
			counter += 1.0f;
		}
	}
	for(i = 0; i < fog_count; i++) {
		if(fog_mask[i]){
			fired = true;
			current = calculateFog(fogs[i], vertex_position, camera_position);
			result += lerp(vertex_color, fogs[i].color, current) / counter;
		}
	}
	if(!fired){
		return vertex_color;
	}
	return result;
}