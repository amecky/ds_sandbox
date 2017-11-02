cbuffer cbChangesPerObject : register( b0 ) {
    matrix mvp;
    matrix world;   
	matrix invWorld; 
	float3 eyePos;
	float padding;
	float3 lightPos;
	float more;
};

struct VS_Input {
    float4 position  : POSITION;
    float2 texcoord : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
};

struct PS_Input {
    float4 pos  : SV_POSITION;
	float3 PosW : POSITION;
    float2 texcoord : TEXCOORD0;
	float3 Light : TEXCOORD1;
	//float3 View : TEXCOORD2;
	float3 normalW : NORMAL;
	float3 tangentW : TANGENT;
};

//---------------------------------------------------------------------------------------
// Transforms a normal map sample to world space.
//---------------------------------------------------------------------------------------
float3 NormalSampleToWorldSpace(float3 normalMapSample, float3 unitNormalW, float3 tangentW) {
	// Uncompress each component from [0,1] to [-1,1].
	float3 normalT = 2.0f*normalMapSample - 1.0f;
	// Build orthonormal basis.
	float3 N = unitNormalW;
	float3 T = normalize(tangentW - dot(tangentW, N)*N);
	float3 B = cross(N, T);
	float3x3 TBN = float3x3(T, B, N);
	// Transform from tangent space to world space.
	float3 bumpedNormalW = mul(normalT, TBN);
	return bumpedNormalW;
}

//---------------------------------------------------------------------------------------
// Computes the ambient, diffuse, and specular terms in the lighting equation
// from a directional light.  We need to output the terms separately because
// later we will modify the individual terms.
//---------------------------------------------------------------------------------------
void ComputeDirectionalLight(float3 lightDirection, float3 normal, float3 toEye,out float4 ambient, out float4 diffuse) {
	// Initialize outputs.
	ambient = float4(0.1f, 0.1f, 0.1f, 1.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 diffuseColor = float4(1.0,1.0,1.0,1.0);
	// The light vector aims opposite the direction the light rays travel.
	float3 lightVec = -lightDirection;
	// Add ambient term.
	//ambient = mat.Ambient * L.Ambient;	
	// Add diffuse and specular term, provided the surface is in 
	// the line of site of the light.
	float diffuseFactor = dot(lightVec, normal);
	// Flatten to avoid dynamic branching.
	[flatten]
	if( diffuseFactor > 0.0f ) {
		diffuse = diffuseFactor * diffuseColor;
	}
}


PS_Input VS_Main( VS_Input vertex ) {
	float3 vecLightDir = normalize(lightPos);
    PS_Input vsOut = ( PS_Input )0;
	//vertex.position.w = 1.0;
	vsOut.pos = mul( vertex.position, world);
	vsOut.pos = mul(vsOut.pos, mvp);
    vsOut.texcoord = vertex.texcoord;
	vsOut.PosW = mul(vertex.position, world);
	// Pass out light and view directions, pre-normalized
	vsOut.Light = normalize(vsOut.PosW - lightPos);
	//vsOut.View = normalize(mul(worldToTangentSpace, eyePos - PosWorld));
	vsOut.normalW = normalize(mul(vertex.normal, (float3x3)world)); // FIXME: use gWorldInvTranspose
	vsOut.tangentW = normalize(mul(vertex.tangent, (float3x3)world));
    return vsOut;
}

Texture2D colorMap : register(t0);
SamplerState colorSampler_ : register(s0);

float4 PS_Main( PS_Input frag ) : SV_TARGET {
	
	float4 Color = colorMap.Sample(colorSampler_, frag.texcoord);
	float2 nuv = frag.texcoord;
	nuv.y += 0.5;
	float3 normalMapSample = colorMap.Sample(colorSampler_, nuv).rgb;
	float3 bumpedNormalW = NormalSampleToWorldSpace(normalMapSample,frag.normalW,frag.tangentW);

	float4 A = float4(0.0,0.0,0.0,0.0);
	float4 D = float4(0.0,0.0,0.0,0.0);

	float3 toEye = normalize(eyePos - frag.PosW);
	

	ComputeDirectionalLight(frag.Light, bumpedNormalW, toEye, A, D);
	//return Color*Ambient + Color * D + Color*S;
	float3 tmp = 0.5 + 0.5 * bumpedNormalW;
	//float3 tmp = float3(nuv,0.0);
	//float3 tmp = 0.5 + 0.5 * frag.normalW;
	float4 ret = Color * saturate( A + D);
	ret.a = Color.a;
	//ret = D;
	return ret;
}

