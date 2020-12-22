struct Material {
    float4 Ambient;
    float4 Diffuse;
    float4 Specular; // w = SpecPower
    float4 Reflect;
};

cbuffer MatrixBuffer : register(b0) {
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
    float2 tex      : TEXCOORD;
    float3 PosW     : POSITION;
    float3 NormalW  : NORMAL;
    float3 TangentW : TANGENT;
};

PixelInputType VertexShaderFX(VertexInputType input) {
    PixelInputType output;

    // Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.PosW = output.position.xyz;
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    // Transform normals to world space.
    output.NormalW = mul(input.NormalL, (float3x3)worldInvTranspose);
    output.TangentW = mul(input.TangentL, (float3x3)worldMatrix);

    // Store the texture coordinates for the pixel shader.
    output.tex = input.tex;

    return output;
}