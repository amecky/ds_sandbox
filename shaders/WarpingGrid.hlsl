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
	float width;
	float base;
	float amplitude;	
	float padding;
	float4 baseColor;
};

float4 PS_Main( PS_Input frag ) : SV_TARGET {
	float4 color = float4(1.0,1.0,1.0,1.0);
	if ( frag.color.r == 1.0) {
		float r = (base - saturate((step(width,frag.tex.x) + step(width,frag.tex.y))) * amplitude) * frag.color.g;		
 		color = baseColor * r;
	}
	else {
		float r = base - (step(frag.tex.x,width) + step(frag.tex.y,width)) * amplitude;		
 		color = baseColor * r;
	}
	 color.a = 1.0;
	return color;
}

