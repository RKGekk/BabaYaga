// Nonnumeric values cannot be added to a cbuffer.
Texture2D shaderTexture : register(t0);

SamplerState SampleType;

struct PixelInputType {
	float4 position : SV_POSITION;
    float2 tex      : TEXCOORD;
    float3 PosW     : POSITION;
    float3 NormalW  : NORMAL;
    float3 TangentW : TANGENT;
};

float4 PixelShaderFX(PixelInputType input) : SV_TARGET {

	float4 c = 0;
	float2 Center = float2(0.5f, 0.5f);
	input.tex -= Center;

	float BlurAmount = 0.1f;
	for (int i = 0; i < 15; i++) {
		float scale = 1.0 + BlurAmount * (i / 14.0);
		c += shaderTexture.Sample(SampleType, input.tex * scale + Center);
	}
   
	c /= 15;
	
	float4 litColor = c;

	float Contrast = 1.0f;
	litColor = float4(((litColor.rgb - 0.5f) * max(Contrast, 0)) + 0.5f, 1.0f);
	
	float Brightness = 0.0f;
	litColor.rgb += Brightness;
	
    return litColor;
}