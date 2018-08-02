cbuffer cbChangesPerFrame : register(b0) {	
	float4 screenDimension;
	matrix wvp;
};

struct Particle {
	float2 position;
	float2 velocity;
    float2 acceleration;
    float2 timer;
	float2 scale;
	float4 startColor;
	float4 endColor;
	float rotation;
	float rotationSpeed;
	float4 textureRect;
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
	float2 t[4];
	float4 ret = ParticlesRO[particleIndex].textureRect;
	float u1 = ret.x / screenDimension.z;
	float v1 = ret.y / screenDimension.w;
	float width = ret.z / screenDimension.z;
	float height = ret.w / screenDimension.w;
	float u2 = u1 + width;
	float v2 = v1 + height;
	t[0] = float2(u1, v1);
	t[1] = float2(u2, v1);
	t[2] = float2(u1, v2);
	t[3] = float2(u2, v2);

	float3 position;
	position.x = (vertexIndex % 2) ? 0.5 : -0.5;
	position.y = (vertexIndex & 2) ? -0.5 : 0.5;
	position.z = 0.0;

	position.x *= ret.z;
	position.y *= ret.w;

	position += float3(ParticlesRO[particleIndex].velocity * elapsed ,0.0);
    position += float3(ParticlesRO[particleIndex].acceleration * elapsed * elapsed, 0.0);

	float rot = ParticlesRO[particleIndex].rotation + ParticlesRO[particleIndex].rotationSpeed * elapsed;
	float s = sin(rot);
	float c = cos(rot);

	float sx = position.x * ParticlesRO[particleIndex].scale.x;
	float sy = position.y * ParticlesRO[particleIndex].scale.y;

	float xt = c * sx - s * sy;
	float yt = s * sx + c * sy;

	float3 sp = float3(ParticlesRO[particleIndex].position, 0.0);

	sp.x -= screenDimension.x;
	sp.y -= screenDimension.y;

	vsOut.pos = mul(float4(xt + sp.x, yt + sp.y, 0.0, 1.0f), wvp);
	vsOut.pos.z = 1.0;
	vsOut.tex = t[vertexIndex];
	vsOut.color = lerp(ParticlesRO[particleIndex].startColor,ParticlesRO[particleIndex].endColor,norm);
	return vsOut;
}

Texture2D colorMap : register(t0);
SamplerState colorSampler : register(s0);

float4 PS_Main(PS_Input frag) : SV_TARGET{
	return colorMap.Sample(colorSampler, frag.tex) * frag.color;
}
