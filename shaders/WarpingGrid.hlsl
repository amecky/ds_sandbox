cbuffer cbChangesPerObject : register( b0 ) {
    matrix mvp;
    matrix world;    
};

struct VS_Input {
    float3 position  : POSITION;
    float4 color : COLOR;
};

struct PS_Input {
    float4 pos  : SV_POSITION;
    float4 color : COLOR;
};

PS_Input VS_Main( VS_Input vertex ) {
    PS_Input vsOut = ( PS_Input )0;
	float4 p = float4(vertex.position,1.0);
    vsOut.pos = mul( p, world);
	vsOut.pos = mul(vsOut.pos, mvp);
    vsOut.color = vertex.color;
    return vsOut;
}

float4 PS_Main( PS_Input frag ) : SV_TARGET {
	float4 color = frag.color;	
	return color;
}

