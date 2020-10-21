TextureCube gCubeMap;

SamplerState SampleType;

struct PixelInputType {
    float4 position : SV_POSITION;
    float3 PosL     : POSITION;
};

float4 PixelShaderFX(PixelInputType input) : SV_TARGET{

    float Brightness = -0.2f;
    float Contrast = 1.3f;
    float4 pixelColor = gCubeMap.Sample(SampleType, input.PosL);

    // Apply contrast.
    pixelColor.rgb = ((pixelColor.rgb - 0.5f) * max(Contrast, 0)) + 0.5f;

    // Apply brightness.
    pixelColor.rgb += Brightness;

    return pixelColor;
}