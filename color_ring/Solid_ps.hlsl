struct PS_Input {
    float4 pos  : SV_POSITION;
    float2 texcoord : TEXCOORD;
	float4 color : COLOR;
};

Texture2D colorMap : register(t0);
SamplerState colorSampler : register(s0);

float4 PS_Main( PS_Input frag ) : SV_TARGET {
	return colorMap.Sample(colorSampler, frag.texcoord) * frag.color;
}
