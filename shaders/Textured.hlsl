cbuffer cbChangesPerObject : register( b0 ) {
    matrix mvp;
    matrix world;    
};

struct VS_Input {
    float4 position  : POSITION;
    float2 texcoord : TEXCOORD;
};

struct PS_Input {
    float4 pos  : SV_POSITION;
    float2 texcoord : TEXCOORD;
};


PS_Input VS_Main( VS_Input vertex ) {
    PS_Input vsOut = ( PS_Input )0;
	vertex.position.w = 1.0;
    vsOut.pos = mul( vertex.position, world);
	vsOut.pos = mul(vsOut.pos, mvp);
    vsOut.texcoord = vertex.texcoord;
    return vsOut;
}

Texture2D colorMap_ : register(t0);

SamplerState samLinear {
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

float4 PS_Main( PS_Input frag ) : SV_TARGET {
    float4 color = colorMap_.Sample(samLinear, frag.texcoord);
    //float r = color.r - 0.999;
    //float d = r / 0.001;
    float d = color.r;
    return saturate(float4(d,d,d,1));
}

