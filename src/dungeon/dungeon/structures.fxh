/*
	TODO: remake documentation for this module.
	P.S. May be it's good to generate shader on CPU side instead of using such big monster on GPU side?
*/

struct light_source{
	int lightning_model;
	int type;
	int attenuation;
	float3 color;
	float3 position;
	float3 direction;
	//this field is used in color mixing
	float intensity;
	float radius;
};

/*
	Only one type of fog is implemented at the current moment
*/
struct fog{
	int type;
	float3 color;
	float4 scissor_plane;
	float argument;
};

/*
	All lightning will be multiplied by:
		- if lightning has ambient type by ambient material component
		- if lightning has diffuse type by diffuse material component
		- if lightning has specular type by specular material component
*/
struct material_description{
	float3 ambient_color;
	float3 diffuse_color;
	float3 specular_color;
	float ambient;
	float diffuse;
	float specular;
	//specular-specific parameter:
	float specular_exp;
	//Cook-Torrance specific parameters:
	float fresnel_reflectance;
	float roughness;
	//Strauss specific parameters
	float smoothness;
	float metalness;
	float transparency;
};

/*
	I don't know yet what parameters will be used while rendering procedure textures so I will use only type.
*/
struct procedure_material_description{
	int type;
	bool normal_mapping;
};

/*
	Following post effects is suported:
	Monochrome		0
	Saturate		1
	Alpha is the first argument for post effect processor.
*/
struct post_effect{
	int type;
	float alpha;
};

struct VS_OUTPUT {
	float4 Pos : SV_POSITION;
	float4 Color : COLOR0;
	float3 Norm : TEXCOORD0;
	float3 RealPos : TEXCOORD1;
	float3 RelativePos : TEXCOORD2;
	float4 TexCoord   : TEXCOORD3;
};

struct TEX_VS_OUTPUT {
	float4 Pos : SV_POSITION;
	float4 Color : COLOR0;
	float3 Norm : TEXCOORD0;
	float3 RealPos : TEXCOORD1;
	float3 RelativePos : TEXCOORD2;
	float2 TexCoord : TEXCOORD3;
};

struct billboard_vertex{
	float3 position : POSITION0;
	float3 center : POSITION1;
	float4 color : COLOR;
	float radius : PSIZE0;
	int type : PSIZE1;
};

struct billboard_fragment{
	float4 target_position : SV_POSITION;
	float3 position : TEXCOORD0;
	float3 normal : TEXCOORD1;
	float3 center : PSIZE0;
	float4 color : COLOR0;
	float radius : PSIZE1;
	int type : PSIZE2;
};

struct animation{
	int type;
	float3 axis;
	float radius;
	float velocity;
};

float3x3 rgb_to_xyz = {
	 0.4124564, 0.3575761, 0.1804375,
	 0.2126729, 0.7151522, 0.0721750,
	 0.0193339, 0.1191920, 0.9503041
	};

float3x3 xyz_to_rgb = {
	3.2404542, -1.5371385, -0.4985314,
	-0.9692660, 1.8760108, 0.0415560,
	 0.0556434, -0.2040259, 1.0572252
	};