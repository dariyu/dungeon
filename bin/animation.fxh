float4 apply_animation(animation input, float4 position, float time){
	float4 result;
	switch(input.type){
		case 0:
			result = position;
			break;
		case 1:
			result = position;
			break;
	}
	return result;
}