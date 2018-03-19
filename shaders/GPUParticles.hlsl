cbuffer cbChangesPerFrame : register(b0) {	
	matrix wvp;
	matrix world;
	float3 eyePos;
	float padding; 
	float4 textureRect;
};

struct Particle {
	float3 position;
	float3 velocity;
    float3 acceleration;
    float2 timer;
	float2 scale;
	float2 growth;
	float rotation;
	float rotationSpeed;
	float4 startColor;
	float4 endColor;
};

StructuredBuffer<Particle> ParticlesRO : register(t1);

struct PS_Input {
	float4 pos  : SV_POSITION;
	float2 tex : TEXCOORD0;
	float4 color : COLOR0;
};

PS_Input VS_Main(uint id:SV_VERTEXID) {
	PS_Input vsOut = (PS_Input)0;
	uint particleIndex = id / 4;
	uint vertexIndex = id % 4;	
	float elapsed = ParticlesRO[particleIndex].timer.x;
    float norm = ParticlesRO[particleIndex].timer.y;

	float2 scaling = ParticlesRO[particleIndex].scale;
	scaling += ParticlesRO[particleIndex].growth * elapsed;
	scaling = saturate(scaling * 0.5 * 0.5);	

	float3 position;
	//position.x = (vertexIndex % 2) ? 0.5 : -0.5;
	//position.y = (vertexIndex & 2) ? -0.5 : 0.5;
	position.x = (vertexIndex % 2) ? scaling.x : -scaling.x;
	position.y = (vertexIndex & 2) ? -scaling.y : scaling.y;
	position.z = 1.0;

    float3 pos = ParticlesRO[particleIndex].position;

	



	float rot = ParticlesRO[particleIndex].rotation + ParticlesRO[particleIndex].rotationSpeed * elapsed;
	float s = sin(rot);
	float c = cos(rot);

	float sx = position.x;// * scaling.x;
	float sy = position.y;// * scaling.y;

	float xt = c * sx - s * sy;
	float yt = s * sx + c * sy;

    pos += ParticlesRO[particleIndex].velocity * elapsed;
    pos += ParticlesRO[particleIndex].acceleration * elapsed * elapsed;
 	vsOut.pos = mul(float4(xt + pos.x, yt + pos.y, pos.z, 1.0f), wvp);
	float4 rect = textureRect; 
	vsOut.tex.x = (vertexIndex % 2) ? rect.z : rect.x;
    vsOut.tex.y = (vertexIndex & 2) ? rect.w : rect.y;
	vsOut.color = lerp(ParticlesRO[particleIndex].startColor,ParticlesRO[particleIndex].endColor,norm);
	return vsOut;
}

Texture2D colorMap : register(t0);
SamplerState colorSampler : register(s0);

float4 PS_Main(PS_Input frag) : SV_TARGET{
	return colorMap.Sample(colorSampler, frag.tex) * frag.color;
}
