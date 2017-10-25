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
    output.position = mul(float4(input.position,1.0), worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
	//output.ShadowPosH.w = 1.0;
    output.ShadowPosH = mul(float4(input.position,1.0), shadowTransform);
    output.color = input.color;
    output.normal = mul(input.normal, (float3x3)worldMatrix);
    output.normal = normalize(output.normal);
    worldPosition = mul(float4(input.position,1.0), worldMatrix);
    output.lightPos = lightPosition.xyz - worldPosition.xyz;
    //output.lightPos = normalize(output.lightPos);
	output.eyePos = eyePosition.xyz - worldPosition.xyz;
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
	float diff = depth - ret;
	return diff;
}
//---------------------------------------------------------------------------------------
// Computes the ambient, diffuse, and specular terms in the lighting equation
// from a directional light.  We need to output the terms separately because
// later we will modify the individual terms.
//---------------------------------------------------------------------------------------
void ComputeDirectionalLight(float4 diffuseColor, float3 lightDirection,
                             float3 normal, float3 toEye,
					         out float4 ambient,
						     out float4 diffuse)
{
	// Initialize outputs.
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// The light vector aims opposite the direction the light rays travel.
	float3 lightVec = -lightDirection;
	float diffuseFactor = dot(lightVec, normal);
	[flatten]
	if( diffuseFactor > 0.0f ) {
		float3 v = reflect(-lightVec, normal);
		diffuse = diffuseFactor * diffuseColor;
	}
}

float calcPCF(SamplerState samLin, Texture2D shadowMap, float2 uv, float depth) {
	float visibility = 1.0f;
	const float dx = SMAP_DX;
	float bias = 0.005;
	const float2 offsets[9] = {
		float2(-dx,  -dx), float2(0.0f,  -dx), float2(dx,  -dx),
		float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
		float2(-dx,  +dx), float2(0.0f,  +dx), float2(dx,  +dx)
	};
	[unroll]
	for(int i = 0; i < 9; ++i) 	{
		float dp = shadowMap.Sample(samLin, uv + offsets[i]).r;
		if ( dp  <  depth - bias ) {
    		visibility -= 0.1;
		}
	}
	return visibility;
}

Texture2D depthMapTexture : register(t0);
SamplerComparisonState compSampler:register(s0);
SamplerState linSampler:register(s1);

float4 PS_Main(PixelInputType input) : SV_TARGET {

	float2 shadowTexCoord = input.ShadowPosH.xy;
	float dp =  depthMapTexture.Sample(linSampler, input.ShadowPosH.xy).r;
	float pixelDepth = input.ShadowPosH.z / input.ShadowPosH.w;
	float bias = 0.005;
	float visibility = 1.0;
	if ( dp  <  input.ShadowPosH.z - bias ){
    	visibility = 0.5;
	}

	visibility = calcPCF(linSampler,depthMapTexture,input.ShadowPosH.xy,input.ShadowPosH.z);
	//float shadow = CalcShadowFactor(compSampler, depthMapTexture, input.ShadowPosH);

    float4 color = ambientColor;

	float4 ambient = float4(0.1f, 0.1f, 0.1f, 0.0f);
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// Cache the distance to the eye from this surface point.
	float distToEye = length(input.eyePos);

	// Normalize.
	float3 ne = input.eyePos / distToEye;
	float4 A, D;
	ComputeDirectionalLight(float4(1.0,1.0,1.0,1.0), lightDirection.xyz,input.normal, ne, A, D);
	ambient += A;    
	diffuse += visibility * D;
	color = input.color * (ambient + diffuse);
	color = saturate(color);
	color.a = input.color.a;
	//if ((saturate(shadowTexCoords.x) == shadowTexCoords.x) && (saturate(shadowTexCoords.y) == shadowTexCoords.y) &&  (pixelDepth > 0)) {
		//float depthValue = CalcShadowFactor(compSampler, depthMapTexture, input.ShadowPosH);
		//float lightIntensity = saturate(dot(input.normal, input.lightPos));
		//if(lightIntensity > 0.0f) {
		//	color += diffuseColor * lightIntensity * sf;
			//color = saturate(color);
		//}
		//color = float4(1.0,0.0,0.0,1.0);
	//}
	//float4 textureColor = input.color;
	//color = color * textureColor;
	//color = float4(visibility,visibility,visibility,1.0);
    return color;
}