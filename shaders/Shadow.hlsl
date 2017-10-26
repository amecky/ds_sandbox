cbuffer MatrixBuffer  : register(b0) {
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix shadowTransform;
};

cbuffer LightBuffer  : register(b1) {
	float4 ambientColor;
	float4 diffuseColor;
	float4 lightPosition;
	float4 lightDirection;	
};

struct VertexInputType {
    float3 position : POSITION;
    float4 color : COLOR;
	float3 normal : NORMAL;
};

struct PixelInputType {
    float4 position : SV_POSITION;
    float4 color : COLOR;
	float3 normal : NORMAL;
    float4 ShadowPosH : TEXCOORD1;
	float3 lightPos : TEXCOORD2;
};

SamplerState samLinear {
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType VS_Main(VertexInputType input) {
    PixelInputType output;
	float4 pos = float4(input.position,1.0);
	float4 modelPos = mul(pos, worldMatrix);
    output.position = mul(modelPos, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    output.ShadowPosH = mul(modelPos, shadowTransform);
	output.color = input.color;
	output.normal = normalize(mul(input.normal, (float3x3)worldMatrix));
    output.lightPos = lightPosition.xyz - modelPos.xyz;
	return output;
}

static const float SMAP_SIZE = 2048.0f;
static const float SMAP_DX = 1.0f / SMAP_SIZE;

float CalcShadowFactor(SamplerComparisonState samShadow, Texture2D shadowMap, float4 shadowPosH) {
	shadowPosH.xyz /= shadowPosH.w;
	float depth = shadowPosH.z;
	const float dx = SMAP_DX;
	float percentLit = 0.0f;
	const float2 offsets[9] = {
		float2(-dx,  -dx), float2(0.0f,  -dx), float2(dx,  -dx),
		float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
		float2(-dx,  +dx), float2(0.0f,  +dx), float2(dx,  +dx)
	};
	[unroll]
	for(int i = 0; i < 9; ++i) 	{
		percentLit += shadowMap.SampleCmpLevelZero(samShadow, shadowPosH.xy + offsets[i], depth).r;
	}
	float ret = percentLit /= 9.0f;
	return ret;
}

Texture2D depthMapTexture : register(t0);
SamplerComparisonState compSampler:register(s0);
SamplerState linSampler:register(s1);

float4 PS_Main(PixelInputType input) : SV_TARGET {
	float shadow = CalcShadowFactor(compSampler, depthMapTexture, input.ShadowPosH);
    float4 color = float4(0,0,0,1);
	float3 L = normalize(input.lightPos.xyz);
	float NdotL = saturate(dot(input.normal,L));	
	if ( NdotL > 0.0 ) {
		color = input.color * shadow * NdotL;
		color = saturate(color);
	}
	color.a = input.color.a;
    return color;
}