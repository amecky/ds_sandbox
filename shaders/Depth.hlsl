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
    float4 depthPosition : TEXTURE0;
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
	// Store the position value in a second input value for depth value calculations.
	output.depthPosition = output.position;
	
	return output;
}

float4 PS_Main(PixelInputType input) : SV_TARGET {
	float depthValue;
	float4 color;
	// Get the depth value of the pixel by dividing the Z pixel depth by the homogeneous W coordinate.
	depthValue = input.depthPosition.z / input.depthPosition.w;
	color = float4(depthValue, depthValue, depthValue, 1.0f);
	return color;
}


