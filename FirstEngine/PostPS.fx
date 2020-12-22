Texture2D shaderTexture : register(t0);

SamplerState SampleType;

struct PixelInputType {
	float4 position : SV_POSITION;
    float2 tex      : TEXCOORD;
    float3 PosW     : POSITION;
    float3 NormalW  : NORMAL;
    float3 TangentW : TANGENT;
};

static const float2 poisson[12] = {
        float2(-0.326212f, -0.40581f),
        float2(-0.840144f, -0.07358f),
        float2(-0.695914f, 0.457137f),
        float2(-0.203345f, 0.620716f),
        float2(0.96234f, -0.194983f),
        float2(0.473434f, -0.480026f),
        float2(0.519456f, 0.767022f),
        float2(0.185461f, -0.893124f),
        float2(0.507431f, 0.064425f),
        float2(0.89642f, 0.412458f),
        float2(-0.32194f, -0.932615f),
        float2(-0.791559f, -0.59771f)
};

float4 PixelShaderFX(PixelInputType input) : SV_TARGET {

	float4 cOut = 0;
	//float2 Center = float2(0.5f, 0.5f);
	//input.tex -= Center;

	//float BlurAmount = 0.1f;
	//for (int i = 0; i < 15; i++) {
	//	float scale = 1.0 + BlurAmount * (i / 14.0);
	//	cOut += shaderTexture.Sample(SampleType, input.tex * scale + Center);
	//}
   
	//cOut /= 15;
    
    float Threshold = 0.95f;
    // Look up the original image color.
    float4 originalColor = shaderTexture.Sample(SampleType, input.tex);
    
	float DiskRadius = 5.0f;
	float2 InputSize = float2(400.0f, 400.0f);
	// Center tap
	float3 rgb = originalColor.rgb;
    rgb = saturate((rgb - Threshold) / (1 - Threshold));
	cOut = float4(rgb, originalColor.a);
	for (int tap = 0; tap < 12; tap++) {
		float2 coord = input.tex.xy + (poisson[tap] / InputSize * DiskRadius);
		float3 rgb2 = shaderTexture.Sample(SampleType, coord).rgb;
		rgb2 = saturate((rgb2 - Threshold) / (1 - Threshold));
		cOut += float4(rgb2, originalColor.a);
	}

	cOut /= 13.0f;
	
	float4 litColor = cOut;

	float Contrast = 1.0f;
	litColor = float4(((litColor.rgb - 0.5f) * max(Contrast, 0)) + 0.5f, 1.0f);
	
	float Brightness = 2.5f;
	litColor.rgb *= Brightness;
	
    return litColor + originalColor * 0.5f;;
}