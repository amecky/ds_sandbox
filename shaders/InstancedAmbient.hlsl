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

cbuffer cbChangesPerObject : register(b0) {
	float4 ambientColor;
	float4 diffuseColor;
	float3 lightDirection;
	float padding;
};

float4 PS_Main( PS_Input frag ) : SV_TARGET {
	float4 textureColor = frag.color;
	float4 color = ambientColor;
	float3 lightDir = -lightDirection;
	float lightIntensity = saturate(dot(frag.normal, lightDir));
	if (lightIntensity > 0.0f) {
		color += (diffuseColor * lightIntensity);
	}
	color = saturate(color);
	color = color * textureColor;
	return color;
}

