cbuffer cbChangesPerFrame : register(b0) {
	float4 screenDimension;
	float4 screenCenter;
	matrix wvp;
};

struct Billboard {
	float3 position; 
	float2 dimension;
	float rotation;
	float2 scaling;
	float4 tex;
	float4 color;
};

StructuredBuffer<Billboard> BillboardsRO : register(t1);

struct GSPS_INPUT {
	float4 Pos : SV_POSITION;
	float4 Tex : COLOR0;
	float3 Size : NORMAL0;
	float4 Color : COLOR1;
};

struct PS_Input {
	float4 pos  : SV_POSITION;
	float2 tex0 : TEXCOORD0;
	float4 color : COLOR0;
};

PS_Input VS_Main(uint id:SV_VERTEXID) {
	PS_Input vsOut = (PS_Input)0;
	uint spriteIndex = id / 4;
	uint vertexIndex = id % 4;	
	float2 t[4];
	float4 ret = BillboardsRO[spriteIndex].tex;
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

	position.x *= BillboardsRO[spriteIndex].dimension.x;
	position.y *= BillboardsRO[spriteIndex].dimension.y;

	float rot = BillboardsRO[spriteIndex].rotation;
	float s = sin(rot);
	float c = cos(rot);

	float sx = position.x * BillboardsRO[spriteIndex].scaling.x;
	float sy = position.y * BillboardsRO[spriteIndex].scaling.y;

	float xt = c * sx - s * sy;
	float yt = s * sx + c * sy;

	float3 sp = BillboardsRO[spriteIndex].position;

	//sp.x -= screenCenter.x;
	//sp.y -= screenCenter.y;

	vsOut.pos = mul(float4(xt + sp.x, yt + sp.y, sp.z, 1.0f), wvp);
	//vsOut.pos.z = 0.0;
	vsOut.tex0 = t[vertexIndex];
	vsOut.color = BillboardsRO[spriteIndex].color;
	return vsOut;
}

Texture2D colorMap : register(t0);
SamplerState colorSampler : register(s0);

float4 PS_Main(PS_Input frag) : SV_TARGET{
	return colorMap.Sample(colorSampler, frag.tex0) * frag.color;
	//return frag.color;
}
