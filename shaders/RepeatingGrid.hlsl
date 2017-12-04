cbuffer cbChangesPerObject : register( b0 ) {
    matrix mvp;
    matrix world;    
};

struct VS_Input {
    float4 position  : POSITION;
};

struct PS_Input {
    float4 pos  : SV_POSITION;
    float3 wp  : NORMAL;
};


PS_Input VS_Main( VS_Input vertex ) {
    PS_Input vsOut = ( PS_Input )0;
	vertex.position.w = 1.0;
    vsOut.pos = mul( vertex.position, world);
    vsOut.wp = mul( vertex.position, world).xyz;
	vsOut.pos = mul(vsOut.pos, mvp);
    return vsOut;
}

cbuffer gridSettings : register( b0 ) {
    float amplitude;
    float base;
    float one;
    float two;  
};


float4 PS_MainXZ( PS_Input frag ) : SV_TARGET {
    float2 coord = frag.wp.xz;
    float2 gr = abs(frac(coord - 0.5) - 0.5) / fwidth(coord);
    float ln = min(gr.x, gr.y);
    float v = (1.0 - min(ln, 1.0) * base) * amplitude;
    float4 color = float4(float3(v,v,v), 1.0);
	return color;
}

float4 PS_MainXY( PS_Input frag ) : SV_TARGET {
    float2 coord = frag.wp.xy;
    float2 gr = abs(frac(coord - 0.5) - 0.5) / fwidth(coord);
    float ln = min(gr.x, gr.y);
    float v = (1.0 - min(ln, 1.0) * base) * amplitude;
    float4 color = float4(float3(v,v,v), 1.0);
	return color;
}

float4 PS_MainYZ( PS_Input frag ) : SV_TARGET {
    float2 coord = frag.wp.yz;
    float2 gr = abs(frac(coord - 0.5) - 0.5) / fwidth(coord);
    float ln = min(gr.x, gr.y);
    float v = (1.0 - min(ln, 1.0) * base) * amplitude;
    float4 color = float4(float3(v,v,v), 1.0);
	return color;
}

