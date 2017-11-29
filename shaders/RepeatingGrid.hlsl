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
    float4 color = float4(0,0,0,1);
	float2 tex = frag.tex;
	float2 coords = frac(frag.tex);
    float dx = 0.1;
    float2 weights = saturate(smoothstep(float2(0.0,0.0), float2(dx,dx), coords) * (1.0 - smoothstep(float2(1.0-dx,1.0-dx), float2(1.0,1.0), coords)));
    float4 bgColor = float4(0.05f, 0.05f, 0.05f, 1.0f);
    float4 grColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
    float2 tmp = saturate(dot(weights,float2(1.0,1.0)));
    color = lerp( grColor, bgColor, saturate(dot(weights,float2(1.0,1.0))) );
    //color = float4(tmp,0.0,1.0);
	return color;
}

