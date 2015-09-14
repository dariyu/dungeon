SamplerState VolumeSampler{
	Filter = MIN_MAG_LINEAR_MIP_POINT;
	AddressU = Wrap;
	AddressV = Wrap;
	AddressW = Wrap;
};

VS_OUTPUT marble(VS_OUTPUT input, float scale_factor, float3 alpha, float3 beta, bool normal_mapping) : SV_Target { 
	float4 current = noise_texture.Sample(VolumeSampler, input.RelativePos.xyz * float3(scale_factor, scale_factor, scale_factor));
	float relative = current.x;
	input.Color.rgb = lerp(alpha, beta, relative);
	if(normal_mapping){
		input.Norm = lerp(input.Norm, float3(current.y, current.z, 1.0f), current.x / 2.0f);
	}
	input.Norm = normalize(input.Norm);
	return input;
}

VS_OUTPUT wood(VS_OUTPUT input, float scale_factor, float3 alpha, float3 beta, bool normal_mapping) : SV_Target {
	float3 lookup_vector = input.RelativePos.xyz;
	lookup_vector.z = 1.0f;
	lookup_vector.y = 1.0f;
	float4 current = noise_texture.Sample(VolumeSampler, lookup_vector * float3(scale_factor, scale_factor, scale_factor));
	float relative = current.x;
	input.Color.rgb = lerp(alpha, beta, relative);
	if(normal_mapping){
		input.Norm = lerp(input.Norm, float3(current.y, current.z, 1.0f), current.x / 2.0f);
	}
	input.Norm = normalize(input.Norm);
	return input;
}

VS_OUTPUT strange(VS_OUTPUT input, float scale_factor, float3 alpha, float3 beta, bool normal_mapping) : SV_Target {
	float3 lookup_vector = input.RelativePos.xyz;
	float4 current = noise_texture.Sample(VolumeSampler, lookup_vector * float3(scale_factor, scale_factor, scale_factor));
	float t = current.x;
	current.x = current.x * 20 - floor(current.x);
	current.x = max(abs(current.x), .1f);
	current.x = min(current.x, 1.0f);
	float relative = current.x;
	input.Color.rgb = lerp(alpha, beta, relative);
	if(normal_mapping){
		input.Norm = lerp(input.Norm, float3(current.y, current.z, 1.0f), t / 2.0f);
	}
	input.Norm = normalize(input.Norm);
	return input;
}

VS_OUTPUT procedure_material_process(VS_OUTPUT input){
	if(!procedure_material_enable){
		return input;
	} else {
		switch(ObjectProcedureMaterial.type){
			case 0:
				input = marble(input, 1.0f, float3(.0f, .0f, .0f), float3(1.0f, 1.0f, 1.0f), ObjectProcedureMaterial.normal_mapping);
				break;
			case 1:
				input = wood(input, 1.0f, float3(.0f, .0f, .0f), float3(1.0f, 1.0f, 1.0f), ObjectProcedureMaterial.normal_mapping);
				break;
			case 2:
				input = strange(input, .25f, float3(1.0f, .0f, 1.0f), float3(0.2f, 0.2f, .2f), ObjectProcedureMaterial.normal_mapping);
				break;
		}
	}
	return input;
}