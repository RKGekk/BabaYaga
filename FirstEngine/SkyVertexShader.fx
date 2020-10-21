struct Material {
    float4 Ambient;
    float4 Diffuse;
    float4 Specular; // w = SpecPower
    float4 Reflect;
};

cbuffer MatrixBuffer {
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    matrix worldInvTranspose; // for converting normals
    Material gMaterial;
};

struct VertexInputType {
    float4 position : POSITION;
    float3 NormalL  : NORMAL;
    float2 tex      : TEXCOORD;
    float4 Color    : COLOR;
    float3 TangentL : TANGENT;
};

struct PixelInputType {
    float4 position : SV_POSITION;
    float3 PosL     : POSITION;
};

PixelInputType VertexShaderFX(VertexInputType input) {
    PixelInputType output;

    // Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    // Set z = w so that z/w = 1 (i.e., skydome always on far plane).
    output.position = output.position.xyww;
    
    // Use local vertex position as cubemap lookup vector.
    output.PosL = float3(input.position.x, input.position.y, input.position.z);

    return output;
}