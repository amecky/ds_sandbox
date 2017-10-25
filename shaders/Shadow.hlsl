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
	float4 pos = float4(input.position,1.0);
	float4 modelPos = mul(pos, worldMatrix);
    //output.position = mul(modelPos, worldMatrix);
    output.position = mul(modelPos, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    output.ShadowPosH = mul(modelPos, shadowTransform);
    
	output.color = input.color;

	float4 norm = float4(input.normal, 1.0f);
	norm = mul(norm,worldMatrix);
    output.normal = normalize(norm.xyz);
    
    output.lightPos = lightPosition.xyz - modelPos.xyz;
    //output.lightPos = normalize(output.lightPos);
	output.eyePos = lightPosition.xyz - modelPos.xyz;
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
	float bias = 0.001;
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

	shadowTexCoord.x = 0.5f + (input.ShadowPosH.x / input.ShadowPosH.w * 0.5f);
	shadowTexCoord.y = 0.5f - (input.ShadowPosH.y / input.ShadowPosH.w * 0.5f);
	float pixelDepth = input.ShadowPosH.z / input.ShadowPosH.w;

	float dp = depthMapTexture.Sample(linSampler, shadowTexCoord).r;
	
	float bias = 0.001;
	float visibility = 1.0;
	if ( dp  <  pixelDepth - bias ){
    	visibility = 0.2;
	}

	visibility = calcPCF(linSampler,depthMapTexture,input.ShadowPosH.xy,pixelDepth);
	//float shadow = CalcShadowFactor(compSampler, depthMapTexture, input.ShadowPosH);

    float4 color = ambientColor;

	float4 ambient = float4(0.1f, 0.1f, 0.1f, 0.0f);
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// Cache the distance to the eye from this surface point.
	float distToEye = length(input.eyePos);
	float3 L = normalize(input.lightPos.xyz);//-normalize(input.lightPos.xyz);
	//float3 L = normalize(-lightDirection.xyz);//-normalize(input.lightPos.xyz);
	float NdotL = saturate(dot(input.normal,L));	
	float margin = acos(saturate(NdotL));
	float epsilon = 0.0005 / margin;
	epsilon = clamp(epsilon, 0, 0.1);
	float lighting = 1;

	if ((saturate(shadowTexCoord.x) == shadowTexCoord.x) && (saturate(shadowTexCoord.y) == shadowTexCoord.y) && (pixelDepth > 0)) {
		lighting = float(depthMapTexture.SampleCmpLevelZero(compSampler,shadowTexCoord,pixelDepth + epsilon));
	}
	// Normalize.
	float3 ne = input.eyePos / distToEye;
	float4 A, D;
	ComputeDirectionalLight(float4(1.0,1.0,1.0,1.0), lightDirection.xyz,input.normal, ne, A, D);
	//ambient += A;    
	//diffuse += dp * D;

	color = input.color * NdotL;
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
	//color = float4(pixelDepth,pixelDepth,pixelDepth,1.0);
	NdotL = 0.5f + 0.5f * NdotL;
	L = 0.5f + 0.5f * L;
	float3 np = input.normal * 0.5;
	//color = float4(np.x + 0.5,np.y + 0.5,np.z+0.5,1.0);
	color = float4(NdotL,NdotL,NdotL,1.0);
    return color;
}