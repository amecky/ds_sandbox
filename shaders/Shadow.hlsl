cbuffer MatrixBuffer {
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix shadowTransform;
};

cbuffer LightBuffer {
	float4 ambientColor;
	float4 diffuseColor;
};

cbuffer LightBuffer2 {
    float3 lightPosition;
	float padding;
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
};

SamplerState samLinear {
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

SamplerComparisonState samShadow {
	Filter   = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	AddressU = BORDER;
	AddressV = BORDER;
	AddressW = BORDER;
	BorderColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
    ComparisonFunc = LESS_EQUAL;
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType VS_Main(VertexInputType input) {
    PixelInputType output;
	float4 worldPosition;
    input.position.w = 1.0f;
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    output.ShadowPosH = mul(input.position, shadowTransform);
    output.color = input.color;
    output.normal = mul(input.normal, (float3x3)worldMatrix);
    output.normal = normalize(output.normal);
    worldPosition = mul(input.position, worldMatrix);
    output.lightPos = lightPosition.xyz - worldPosition.xyz;
    output.lightPos = normalize(output.lightPos);
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
		percentLit += shadowMap.Sample(samLinear, shadowPosH.xy + offsets[i]).r;
	}
	return percentLit /= 9.0f;
}

Texture2D depthMapTexture : register(t0);

float4 PS_Main(PixelInputType input) : SV_TARGET {
    float4 color = ambientColor;
	float depthValue = CalcShadowFactor(samShadow, depthMapTexture, input.ShadowPosH);
	float lightIntensity = saturate(dot(input.normal, input.lightPos));
	if(lightIntensity > 0.0f) {
		color += diffuseColor * lightIntensity * depthValue;
		color = saturate(color);
	}
	float4 textureColor = input.color;
	color = color * textureColor;
    return color;
}