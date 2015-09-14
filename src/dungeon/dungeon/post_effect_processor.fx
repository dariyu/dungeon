#include "structures.fxh"

uniform Texture2D image;
/*
	Not yet used: depth is gathered from image alpha chanell
*/
uniform Texture2D < float > depth_image;
uniform float3 focus_data;
uniform float2 scene_size;

cbuffer ImageEffects{
	uniform int post_effect_count;
	uniform post_effect post_effects[16];
}

SamplerState LinearSampler{
	Filter = MIN_MAG_MIP_LINEAR;
	addressU = Wrap;
	addressV = Wrap;
};

#include "post_effect.fxh"

DepthStencilState DisableDepth{
	DepthEnable	 = FALSE;
	DepthWriteMask = ZERO;
};

struct VS_INPUT{
	float4 Position : POSITION;
	float2 TexCoord : TEXCOORD;
};

struct PS_INPUT{
	float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD0;
};

PS_INPUT VS_main(VS_INPUT IN){
	PS_INPUT OUT = (PS_INPUT)0;
	OUT.Position = IN.Position;
	OUT.TexCoord = IN.TexCoord;
	return OUT;
}

float4 PS_main(PS_INPUT IN) : SV_Target{
	float4 color = post_effect_manager(IN.TexCoord);
	return color;
}

float4 HorizontalBlur(PS_INPUT IN) : SV_Target{
	float4 color;
	color.rgb = dof(IN.TexCoord, false);
	color.a = 1.0f;
	return color;
}

float4 VerticalBlur(PS_INPUT IN) : SV_Target{
	float4 color;
	color.rgb = dof(IN.TexCoord, true);
	color.a = 1.0f;
	return color;
}

technique10 Process{
	pass P0{
		SetDepthStencilState(DisableDepth, 0);
		SetVertexShader(CompileShader(vs_4_0, VS_main()));
		SetPixelShader(CompileShader(ps_4_0, PS_main()));
		SetGeometryShader(NULL);
	}
}

/*
	The first place where I use several passes for one technique.
*/
technique10 DOF{
	pass P0{
		SetDepthStencilState(DisableDepth, 0);
		SetVertexShader(CompileShader(vs_4_0, VS_main()));
		SetPixelShader(CompileShader(ps_4_0, HorizontalBlur()));
		SetGeometryShader(NULL);
	}
	pass P1{
		SetDepthStencilState(DisableDepth, 0);
		SetVertexShader(CompileShader(vs_4_0, VS_main()));
		SetPixelShader(CompileShader(ps_4_0, VerticalBlur()));
		SetGeometryShader(NULL);
	}
}