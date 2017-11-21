cbuffer cbChangesPerObject : register( b0 ) {
    matrix mvp;
    matrix world;    
};

struct VS_Input {
    float3 position  : POSITION;
    float4 color : COLOR;
	float2 tex : TEXCOORD;
};

struct PS_Input {
    float4 pos  : SV_POSITION;
    float4 color : COLOR;
	float2 tex : TEXCOORD;
};


PS_Input VS_Main( VS_Input vertex ) {
    PS_Input vsOut = ( PS_Input )0;
	float4 p = float4(vertex.position,1.0);
    vsOut.pos = mul( p, world);
	vsOut.pos = mul(vsOut.pos, mvp);
    vsOut.color = vertex.color;
	vsOut.tex = vertex.tex;
    return vsOut;
}

cbuffer cbChangesPerObject : register(b0) {
	float4 ambientColor;
	float4 diffuseColor;
	float3 lightDirection;
	float padding;
};

float4 PS_Main( PS_Input frag ) : SV_TARGET {
	float r = (step(frag.tex.x,0.9) + step(frag.tex.y,0.9)) * 0.5;
	r = frag.tex.x;
 	float4 color = float4(r,0.0,0.0, 1.0);
	return color;
}

