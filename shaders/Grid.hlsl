cbuffer cbChangesPerObject : register( b0 ) {
    matrix mvp;
    matrix world;    
};

struct VS_Input {
    float4 position  : POSITION;
    float2 tex : TEXCOORD;
};

struct PS_Input {
    float4 pos  : SV_POSITION;
    float2 tex : COLOR;
};


PS_Input VS_Main( VS_Input vertex ) {
    PS_Input vsOut = ( PS_Input )0;
	vertex.position.w = 1.0;
    vsOut.pos = mul( vertex.position, world);
	vsOut.pos = mul(vsOut.pos, mvp);
    vsOut.tex = vertex.tex;
    return vsOut;
}

float4 PS_Main( PS_Input frag ) : SV_TARGET {
	float2 tex = frag.tex;
	float sx = step(0.98,tex.x);
	float sy = step(0.98,tex.y);
	float4 bgColor = float4(0.3f, 0.3f, 0.3f, 1.0f);
	float4 grColor = float4(0.7f, 0.7f, 0.7f, 1.0f);
	float4 color = lerp( bgColor, grColor, saturate(dot(float2(sx,sy),float2(1.0,1.0))) );
	return color;
}

