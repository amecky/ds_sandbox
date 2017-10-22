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
	float4 eyePosition;
};

struct VertexInputType {
    float4 position : POSITION;
    float4 color : COLOR;
	float3 normal : NORMAL;
};

struct PixelInputType {
    float4 position : SV_POSITION;
    float4 color : COLOR;
	float3 normal : NORMAL;
    float4 ShadowPosH : TEXCOORD1;
	float3 lightPos : TEXCOORD2;
	float3 eyePos : TEXCOORD3;
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
	float4 worldPosition;
    input.position.w = 1.0;
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
	//output.ShadowPosH.w = 1.0;
    output.ShadowPosH = mul(input.position, shadowTransform);
    output.color = input.color;
    output.normal = mul(input.normal, (float3x3)worldMatrix);
    output.normal = normalize(output.normal);
    worldPosition = mul(input.position, worldMatrix);
    output.lightPos = lightPosition.xyz - worldPosition.xyz;
    //output.lightPos = normalize(output.lightPos);
	output.eyePos = eyePosition.xyz - worldPosition.xyz;
	return output;
}

static const float SMAP_SIZE = 1024.0f;
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
		//percentLit += shadowMap.Sample(samLinear, shadowPosH.xy + offsets[i]).r;
		percentLit += shadowMap.SampleCmpLevelZero(samShadow, shadowPosH.xy + offsets[i], depth).r;
	}
	//return percentLit /= 9.0f;
	return shadowMap.SampleCmpLevelZero(samShadow, shadowPosH.xy, depth).r;
}

Texture2D depthMapTexture : register(t0);
SamplerComparisonState compSampler:register(s0);

float4 PS_Main(PixelInputType input) : SV_TARGET {

	float2 shadowTexCoords;
	shadowTexCoords.x = 0.5f + (input.ShadowPosH.x / input.ShadowPosH.w * 0.5f);
	shadowTexCoords.y = 0.5f - (input.ShadowPosH.y / input.ShadowPosH.w * 0.5f);
	float pixelDepth = input.ShadowPosH.z / input.ShadowPosH.w;


    float4 color = ambientColor;
	if ((saturate(shadowTexCoords.x) == shadowTexCoords.x) && (saturate(shadowTexCoords.y) == shadowTexCoords.y) &&  (pixelDepth > 0)) {
		//float depthValue = CalcShadowFactor(compSampler, depthMapTexture, input.ShadowPosH);
		float lightIntensity = saturate(dot(input.normal, input.lightPos));
		if(lightIntensity > 0.0f) {
			color += diffuseColor * lightIntensity * pixelDepth;
			color = saturate(color);
		}
		//color = float4(1.0,0.0,0.0,1.0);
	}
	float4 textureColor = input.color;
	color = color * textureColor;
	color = float4(pixelDepth,pixelDepth,pixelDepth,1.0);
    return color;
}