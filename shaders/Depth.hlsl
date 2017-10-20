cbuffer cbChangesPerObject : register( b0 ) {
    matrix mvp;
    matrix world;    
};

struct VertexInputType {
    float4 position  : POSITION;
    float4 color : COLOR;
	float3 normal : NORMAL;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType VS_Main(VertexInputType input) {
    PixelInputType output;
	// Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;
    output.position = mul( input.position, world);
	output.position = mul(output.position, mvp);
	return output;
}


