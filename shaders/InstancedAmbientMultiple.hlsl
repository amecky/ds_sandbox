cbuffer cbChangesPerObject : register( b0 ) {
    matrix mvp;
    matrix world;    
};

struct VS_Input {
    float3 position  : POSITION;
	float4 color : COLOR0;
	float3 normal : NORMAL;	
	matrix world : WORLD;
	float4 icolor : COLOR1;
};
struct PS_Input {
    float4 pos  : SV_POSITION;
    float4 color : COLOR;
	float3 normal : NORMAL;
};


PS_Input VS_Main( VS_Input vertex ) {
    PS_Input vsOut = ( PS_Input )0;
	float4 p = float4(vertex.position,1.0);
    vsOut.pos = mul( p, vertex.world);
	vsOut.pos = mul(vsOut.pos, mvp);
    vsOut.color = vertex.color * vertex.icolor;
	vsOut.normal = mul(vertex.normal, (float3x3)vertex.world);
	vsOut.normal = normalize(vsOut.normal);
    return vsOut;
}

struct DirectionalLight {
	float4 Ambient;
	float4 Diffuse;
	float3 Direction;
	float pad;
};

cbuffer cbChangesPerObject : register(b0) {
	DirectionalLight lights[3];
};

float4 PS_Main( PS_Input frag ) : SV_TARGET {
	float4 textureColor = frag.color;
	float4 color = float4(0,0,0,1);
    float4 A = float4(0,0,0,0);
    float4 D = float4(0,0,0,0);
    [unroll] 
	for(int i = 0; i < 3; ++i) {
        A += lights[i].Ambient;
		float3 lightDir = normalize(-lights[i].Direction);
	    float lightIntensity = saturate(dot(frag.normal, lightDir));
	    if (lightIntensity > 0.0f) {
		    D += (lights[i].Diffuse* lightIntensity);
	    }
	}
	//color = saturate(color);
	color = (A + D) * textureColor;
	return color;
}

