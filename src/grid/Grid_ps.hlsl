struct PS_Input {
    float4 pos  : SV_POSITION;
    float2 texcoord : TEXCOORD;
	float4 color : COLOR;
};

float4 PS_Main( PS_Input frag ) : SV_TARGET {
	float2 t = frag.texcoord;
	float w = 0.02;
	float dw = 3.0 * w;
	float rx = 1.0 - step(dw, t.x) + step(1.0 - dw, t.x);
	float ry = 1.0 - step(dw, t.y) + step(1.0 - dw, t.y);
	return saturate(frag.color * (rx + ry));
}
