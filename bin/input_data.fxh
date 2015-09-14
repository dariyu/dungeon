cbuffer frame_data{
	uniform float4x4 MainTransformation;
	uniform float4x4 FullShadowTransformation;
	uniform float4x4 WorldTransformation;
	uniform float4x4 TextureTransformation;
	uniform float3 CameraPosition;
	uniform float3 CameraUp;
	uniform float3 CameraRight;
	uniform float time;
}

cbuffer object_data{
	uniform float4x4 ObjectPosition;
	uniform material_description ObjectMaterial;
	uniform procedure_material_description ObjectProcedureMaterial;
	uniform float opaque;
	uniform animation ObjectAnimation;
}

/*
	For creating shadow maps
*/
cbuffer light_source_data{
	uniform float3 position;
	uniform float3 direction;
}

/*
	TODO: organize this stuff in cbuffers
*/
uniform light_source light_sources[128];
uniform bool light_sources_mask[128];
uniform int light_count;

uniform fog fogs[128];
uniform bool fog_mask[128];
uniform int fog_count;

uniform Texture2D current_texture;
/*
	Currently this input parameter not used (lerp interpolation is used)
	0 for per component mixing
	1 for lerp component mixing
*/
uniform int mixing_type;
uniform bool procedure_material_enable;
uniform material_description current_material;
uniform Texture3D noise_texture;

/*
	Shadow maps data
*/

float DEPTH_BIAS = .18f;
float LIGHT_SIZE = 0.01f;
float SCENE_SCALE = 1.0f;

int SHADOW_RADIUS_SAMPLES = 8;
int PCF_SAMPLES = 8;

SamplerState Depth_Sampler{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Clamp;
	AddressV = Clamp;
};

SamplerState Point_Sampler{
	Filter   = MIN_MAG_MIP_POINT;
	AddressU = Border;
	AddressV = Border;
	BorderColor = float4(.00000001f, 0, 0, 0);
};

SamplerComparisonState PCF_Sampler{
    ComparisonFunc = LESS;
    Filter = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
    AddressU = Border;
    AddressV = Border;
    BorderColor = float4(.00000001f, 0, 0, 0);
};
