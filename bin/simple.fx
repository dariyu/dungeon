#include "structures.fxh"
#include "input_data.fxh"
#include "lightning.fxh"
#include "procedure_material_manager.fxh"
#include "fogging.fxh"

Texture2D < float > DepthMap;

VS_OUTPUT main_vertex_shader(float4 Pos : POSITION, float3 n : NORMAL) {
	VS_OUTPUT Output = (VS_OUTPUT)0;
	Output.RelativePos = Pos.xyz;
	Pos = mul(Pos, ObjectPosition);
	Output.RealPos = (float3)Pos;
	Output.Pos = mul(Pos, MainTransformation);
	Output.Norm = (float3)normalize(mul(float4(n, 0.0f), ObjectPosition));
	Output.Color = float4(1.0f, 1.0f, 1.0f, opaque);
	return Output;
}

float4 main_pixel_shader(VS_OUTPUT Input) : SV_Target {
	float4 result;
	Input.Norm = normalize(Input.Norm);
	Input = procedure_material_process(Input);
	result.rgb = calculateVertexLight(Input.RealPos, Input.Norm, CameraPosition, ObjectMaterial);
	if(procedure_material_enable){
		result.rgb = Input.Color.rgb * result.rgb;
	}
	if(result.r != .0f || result.g != .0f || result.b != .0f) {
		result.rgb = fogVertex(result.rgb, Input.RealPos, CameraPosition);
	}
	result.a = Input.Color.a;
	return result;
}

SamplerState LinearSampler{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

TEX_VS_OUTPUT main_texture_vertex_shader(float4 Pos : POSITION, float3 n : NORMAL, float2 texcoord : TEXTURECOORD) {
	TEX_VS_OUTPUT Output = (TEX_VS_OUTPUT)0;
	Output.RelativePos = Pos.xyz;
	Pos = mul(Pos, ObjectPosition);
	Output.RealPos = (float3)Pos;
	Output.Pos = mul(Pos, MainTransformation);
	Output.Norm = (float3)normalize(mul(float4(n, 0.0f), ObjectPosition));
	Output.Color = float4(.0f, .0f, .0f, opaque);
	Output.TexCoord.x = texcoord.x;
	Output.TexCoord.y = texcoord.y;
	return Output;
}

float4 main_texture_pixel_shader(TEX_VS_OUTPUT Input) : SV_Target {
	float4 result;
	Input.Norm = normalize(Input.Norm);
	result.rgb = calculateVertexLight(Input.RealPos, Input.Norm, CameraPosition, ObjectMaterial);
	float3 texture_color = (float3)current_texture.Sample(LinearSampler, Input.TexCoord);
	result.rgb = result.rgb * texture_color;
	result.a = Input.Color.a;
	return result;
}


DepthStencilState EnableDepth{
	DepthEnable	 = TRUE;
	DepthWriteMask = ALL;
	DepthFunc = LESS_EQUAL;
	StencilEnable = false;
};

RasterizerState DisableCulling{
    CullMode = NONE;
};

RasterizerState EnableCulling{
    CullMode = BACK;
};


BlendState AlphaBlending{
    AlphaToCoverageEnable = FALSE;
    BlendEnable[0] = TRUE;
    SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
    BlendOp = ADD;
    SrcBlendAlpha = SRC_ALPHA;
    DestBlendAlpha = INV_SRC_ALPHA;
    BlendOpAlpha = ADD;
    RenderTargetWriteMask[0] = 0x0F;
};

technique10 Render {
	pass P0 {
		SetDepthStencilState(EnableDepth, 0);
		SetRasterizerState(EnableCulling);
		SetVertexShader(CompileShader(vs_4_0, main_vertex_shader()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, main_pixel_shader()));
	}
}

technique10 TransparentRender {
	pass P0 {
		SetDepthStencilState(EnableDepth, 0);
		SetBlendState(AlphaBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetRasterizerState(EnableCulling);
		SetVertexShader(CompileShader(vs_4_0, main_vertex_shader()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, main_pixel_shader()));
	}
}

technique10 TexturingRender {
	pass P0 {
		SetDepthStencilState(EnableDepth, 0);
		SetRasterizerState(EnableCulling);
		SetVertexShader(CompileShader(vs_4_0, main_texture_vertex_shader()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, main_texture_pixel_shader()));
	}
}


SamplerState Linear_Sampler{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

struct VS_INPUT{
    float4 Position    : POSITION; 
    float2 TexCord	   : TEXCOORD;
};


DepthStencilState DepthStencil{
    DepthEnable = true;
    DepthWriteMask = all;
    DepthFunc = less;
};

DepthStencilState BillboardDepth{
	DepthEnable = true;
	DepthWriteMask = zero;
};

RasterizerState LightRender_RS{
    CullMode = Back;
    FillMode = Solid;
    MultisampleEnable = FALSE;
    DepthBias = 1.e5;
    SlopeScaledDepthBias = 8.0;
};

RasterizerState Depth_Rasterizer{
	FillMode = SOLID;
	CullMode = BACK;
};

RasterizerState Rasterizer{
	FillMode = SOLID;
	CullMode = BACK;
	DepthBias = 0;
	SlopeScaledDepthBias = 0.0f;
	DepthBiasClamp = 0.0f;
};

struct vertex_position{
    float4 position : SV_POSITION;
};

vertex_position shadow_vertex_depth_picker(float4 position : POSITION, float3 normal : NORMAL){
	vertex_position result = (vertex_position)0;
	position = mul(position, ObjectPosition);
	result.position = mul(position, FullShadowTransformation);
	return result;
}

float shadow_pixel_depth_picker(vertex_position input) : SV_Target{
	return input.position.w;
}

BlendState NoBlending_BS{
    BlendEnable[0] = FALSE;
    RenderTargetWriteMask[0] = 0x0F;
};

technique10 shadow_depth_picking{
    pass P0{
        SetVertexShader(CompileShader(vs_4_0, shadow_vertex_depth_picker()));
        SetPixelShader(CompileShader(ps_4_0, shadow_pixel_depth_picker()));
        SetGeometryShader(NULL);
        SetDepthStencilState(DepthStencil, 0);
        SetRasterizerState(Depth_Rasterizer);
        SetBlendState(NoBlending_BS, float4(1.0f, 1.0f, 1.0f, 1.0f), 0xffffffff);
    }
}

vertex_position camera_vertex_depth_picker(float4 position : POSITION, float3 normal : NORMAL){
	vertex_position result = (vertex_position)0;
	position = mul(position, ObjectPosition);
	result.position = mul(position, MainTransformation);
	return result;
}

float camera_pixel_depth_picker(vertex_position IN) : SV_Target{
	return IN.position.z;
}

technique10 camera_depth_picker{
    pass P0{
        SetVertexShader(CompileShader(vs_4_0, camera_vertex_depth_picker()));
        SetPixelShader(CompileShader(ps_4_0, camera_pixel_depth_picker()));
        SetGeometryShader(NULL);
        SetDepthStencilState(DepthStencil, 0);
        SetRasterizerState(Depth_Rasterizer);
        SetBlendState(NoBlending_BS, float4(1.0f, 1.0f, 1.0f, 1.0f), 0xffffffff);
    }
}

VS_OUTPUT shadow_vertex_renderer(float4 Pos : POSITION, float3 n : NORMAL){
	VS_OUTPUT Output = (VS_OUTPUT)0;
	Output.RelativePos = Pos.xyz;
	Pos = mul(Pos, ObjectPosition);
	Output.TexCoord = mul(Pos, TextureTransformation);
	Output.RealPos = (float3)Pos;
	Output.Pos = mul(Pos, MainTransformation);
	Output.Norm = (float3)normalize(mul(float4(n, 0.0f), ObjectPosition));
	Output.Color = float4(1.0f, 1.0f, 1.0f, opaque);
	return Output;
}

float calculate_soft_shadow(Texture2D < float > DepthMapTex, float4 coords){
	float2 uv = (coords.xy / coords.w);
	float zCoord = coords.z;
	float fragment_depth = zCoord - DEPTH_BIAS;
	float BlockerDepth = 0;
	float BlockerSum = 0;
	float numBlockers = 0;
	float searchWidth = SCENE_SCALE * LIGHT_SIZE / zCoord;
	float2 Block_uv = uv - float2(searchWidth, searchWidth);
	int i = 0, j = 0;
	for (i = 0; i < SHADOW_RADIUS_SAMPLES; i++){
		for(j = 0; j < SHADOW_RADIUS_SAMPLES; j++){
			float2 texOffset = Block_uv + float2(i * searchWidth,j * searchWidth);
			BlockerDepth = DepthMapTex.SampleLevel(Point_Sampler, texOffset, 0);
			if(BlockerDepth < fragment_depth){
				BlockerSum += BlockerDepth;
				numBlockers += 1.0f;
			}
		}
    }
	BlockerDepth = BlockerSum / numBlockers;
	if(numBlockers < 1){
		return 1.0f;
	}
	/*
		Starting smoothing shadow
	*/
	float ShadowRadius =((zCoord - BlockerDepth) * LIGHT_SIZE / BlockerDepth);
	float shadowColor = 0.0f; 
	float stepSize = 2.0f * ShadowRadius / (PCF_SAMPLES - 1.0f);
	uv -= float2(ShadowRadius, ShadowRadius);
	for (i = 0; i < PCF_SAMPLES; i++){
		for (j = 0; j < PCF_SAMPLES; j++){
			float2 offset = float2(i * stepSize, j * stepSize);
			shadowColor += DepthMapTex.SampleCmpLevelZero(PCF_Sampler, uv + offset, fragment_depth);
		}
	}
	shadowColor = shadowColor / (PCF_SAMPLES * PCF_SAMPLES);
	return shadowColor;
}


float4 shadow_pixel_renderer(VS_OUTPUT Input) : SV_Target{
	float shadowColor = 0.0f;
	float4 result;
	float3 black = float3(.0f, .0f, .0f);
	float alpha = calculateIntensity(Input.RealPos, Input.Norm);
	Input.Norm = normalize(Input.Norm);
	Input = procedure_material_process(Input);
	result.rgb = calculateVertexLight(Input.RealPos, Input.Norm, CameraPosition, ObjectMaterial);
	if(procedure_material_enable){
		result.rgb = Input.Color.rgb * result.rgb;
	}
	result.a = Input.Color.a;
	if(result.r != .0f || result.g != .0f || result.b != .0f){
		shadowColor = calculate_soft_shadow(DepthMap, Input.TexCoord);
		result.rgb = fogVertex(result.rgb, Input.RealPos, CameraPosition);
		result.rgb = lerp(black, result.rgb, alpha);
		result.rgb *= shadowColor;
	}
	return result;
}


technique10 shadow_rendering{
    pass P0{
        SetVertexShader( CompileShader(vs_4_0, shadow_vertex_renderer()));
        SetPixelShader(CompileShader(ps_4_0, shadow_pixel_renderer()));
        SetGeometryShader(NULL);
        SetDepthStencilState(DepthStencil, 0);
        SetRasterizerState(Rasterizer);
    }
}

billboard_fragment billboard_vs(float3 position : POSITION, float3 center : NORMAL, float4 color : COLOR, float radius : PSIZE, int type : BLENDINDICES){
	billboard_fragment result = (billboard_fragment)0;
	result.position = position;
	result.center = center;
	result.color = color;
	result.type = type;
	result.radius = radius;
	result.normal = normalize(result.position - CameraPosition);
	result.target_position = mul(float4(result.position, 1.0f), MainTransformation);
	return result;
}

float3 project_vector(float3 plane_dot, float3 plane_normal, float3 input){
	float3 result;
	input += plane_dot;
	result = input + plane_normal * dot(plane_normal, plane_dot - input);
	return result - plane_dot;
}

float3 make_ortogonal_companion(float3 input, float3 axis){
	float3 result = (float3)0;
	float3x3 rotation_matrix;
	rotation_matrix[0][0] = axis.x * axis.x;
	rotation_matrix[0][1] = axis.x * axis.y - axis.z;
	rotation_matrix[0][2] = axis.x * axis.z + axis.y;
	
	rotation_matrix[1][0] = axis.x * axis.y + axis.z;
	rotation_matrix[1][1] = axis.y * axis.y;
	rotation_matrix[1][2] = axis.z * axis.y - axis.x;
	
	rotation_matrix[2][0] = axis.z * axis.x - axis.y;
	rotation_matrix[2][1] = axis.z * axis.y + axis.x;
	rotation_matrix[2][2] = axis.z * axis.z;
	result = mul(input, rotation_matrix);
	return result;
}

/*
	By the way: vertici means vertex (in Italian).
*/

[maxvertexcount(6)]
void billboard_gs(point billboard_fragment input[1], inout TriangleStream < billboard_fragment > stream){
	billboard_fragment new_vertices[6];
	int i = 0;
	float3 basis[2];
	float4 red = float4(1.0f, .0f, .0f, 1.0f);
	
	[unroll]
	for(i = 0; i < 6; i++){
		new_vertices[i] = (billboard_fragment)0;
	}

	[unroll]
	for(i = 0; i < 6; i++){
		new_vertices[i].center = input[0].center;
		new_vertices[i].color = input[0].color;
		new_vertices[i].type = input[0].type;
		new_vertices[i].normal = input[0].normal;
		new_vertices[i].position = input[0].position;
		new_vertices[i].radius = input[0].radius;
	}
	if(abs(dot(CameraUp, input[0].normal)) < .9f){
		basis[0] = project_vector(input[0].center, input[0].normal, CameraRight);
	} else {
		basis[0] = project_vector(input[0].center, input[0].normal, CameraUp);
	}
	basis[0] = normalize(basis[0]);
	basis[1] = make_ortogonal_companion(basis[0], input[0].normal);
	basis[1] = normalize(basis[1]);
	//basis[0] *= input[0].radius;
	//basis[1] *= input[0].radius;
	
	new_vertices[0].position = input[0].position + basis[0] + basis[1];
	new_vertices[1].position = input[0].position + basis[0] - basis[1];
	new_vertices[2].position = input[0].position - basis[0] - basis[1];
	
	new_vertices[3].position = input[0].position + basis[0] + basis[1];
	new_vertices[4].position = input[0].position - basis[0] - basis[1];
	new_vertices[5].position = input[0].position - basis[0] + basis[1];
	
	[unroll]
	for(i = 0; i < 6; i++){
		new_vertices[i].target_position = mul(float4(new_vertices[i].position, 1.0f), MainTransformation);
	}

	stream.Append(new_vertices[0]);
	stream.Append(new_vertices[1]);
	stream.Append(new_vertices[2]);
	stream.RestartStrip();

	stream.Append(new_vertices[3]);
	stream.Append(new_vertices[4]);
	stream.Append(new_vertices[5]);
	stream.RestartStrip();
}

float4 billboard_ps(billboard_fragment input) : SV_Target{
	float4 result = float4(.0f, .0f, .0f, 1.0f);
	float4 black = float4(.0f, .0f, .0f, .0f);
	float3 dist = input.position - input.center;
	float r = dot(dist, dist);
	float shiness = 1.0f;
	switch(input.type){
		case 0:
			clip(-1.0f);
			break;
		case 1:
			if(r > input.radius){
				clip(-1.0f);
			} else {
				shiness = 1.0f - r / input.radius;
			}
			break;
	}
	result = lerp(black, input.color, shiness);
	return result;
}

DepthStencilState DisableDepth{
	DepthEnable	= false;
	DepthWriteMask = zero;
};

technique10 billboard_rendering{
	pass P0{
		SetVertexShader(CompileShader(vs_4_0, billboard_vs()));
		SetGeometryShader(CompileShader(gs_4_0, billboard_gs()));
        SetPixelShader(CompileShader(ps_4_0, billboard_ps()));
        SetDepthStencilState(EnableDepth, 0);
		SetRasterizerState(DisableCulling);
		SetBlendState(AlphaBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
	}
}

[maxvertexcount(6)]
void particle_gs(point billboard_fragment input[1], inout TriangleStream < billboard_fragment > stream){
	billboard_fragment new_vertices[6];
	int i = 0;
	float3 basis[2];
	float4 red = float4(1.0f, .0f, .0f, 1.0f);
	
	input[0].position.x += sin(time) * 2.0f;
	input[0].center.x += sin(time) * 2.0f;
	
	[unroll]
	for(i = 0; i < 6; i++){
		new_vertices[i] = (billboard_fragment)0;
	}

	[unroll]
	for(i = 0; i < 6; i++){
		new_vertices[i].center = input[0].center;
		new_vertices[i].color = input[0].color;
		new_vertices[i].type = input[0].type;
		new_vertices[i].normal = input[0].normal;
		new_vertices[i].position = input[0].position;
		new_vertices[i].radius = input[0].radius;
	}
	if(abs(dot(CameraUp, input[0].normal)) < .9f){
		basis[0] = project_vector(input[0].center, input[0].normal, CameraRight);
	} else {
		basis[0] = project_vector(input[0].center, input[0].normal, CameraUp);
	}
	basis[0] = normalize(basis[0]);
	basis[1] = make_ortogonal_companion(basis[0], input[0].normal);
	basis[1] = normalize(basis[1]);
	//basis[0] *= input[0].radius;
	//basis[1] *= input[0].radius;
	
	new_vertices[0].position = input[0].position + basis[0] + basis[1];
	new_vertices[1].position = input[0].position + basis[0] - basis[1];
	new_vertices[2].position = input[0].position - basis[0] - basis[1];
	
	new_vertices[3].position = input[0].position + basis[0] + basis[1];
	new_vertices[4].position = input[0].position - basis[0] - basis[1];
	new_vertices[5].position = input[0].position - basis[0] + basis[1];
	
	[unroll]
	for(i = 0; i < 6; i++){
		new_vertices[i].target_position = mul(float4(new_vertices[i].position, 1.0f), MainTransformation);
	}

	stream.Append(new_vertices[0]);
	stream.Append(new_vertices[1]);
	stream.Append(new_vertices[2]);
	stream.RestartStrip();

	stream.Append(new_vertices[3]);
	stream.Append(new_vertices[4]);
	stream.Append(new_vertices[5]);
	stream.RestartStrip();
}

technique10 particle_rendering{
	pass P0{
		SetVertexShader(CompileShader(vs_4_0, billboard_vs()));
		SetGeometryShader(CompileShader(gs_4_0, particle_gs()));
        SetPixelShader(CompileShader(ps_4_0, billboard_ps()));
        SetDepthStencilState(BillboardDepth, 0);
		SetRasterizerState(DisableCulling);
		SetBlendState(AlphaBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
	}
}