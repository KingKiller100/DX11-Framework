//--------------------------------------------------------------------------------------
// File: DX11 Framework.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer : register( b0 )
{
	matrix World;
	matrix View;
	matrix Projection;

	float4 diffuseMtrl;
	float4 diffuseLight;
	float3 lightVecW;
	
	float padding;

	float4 ambientMtrl;
	float4 ambientLight;

	float4 specularMtrl;
	float4 specularLight;
	float specularPower;
	float3 eyePosW; 	// Camera position in world space
}

//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
	float4 PosW : POSITION;
	float2 Tex : TEXCOORD0;
	float3 Norm : NORMAL;
};

/*
//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS( float4 Pos : POSITION, float4 Color : COLOR )
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Pos = mul( Pos, World );
    output.Pos = mul( output.Pos, View );
    output.Pos = mul( output.Pos, Projection );
    output.Color = Color;
    return output;
}
*/

//------------------------------------------------------------------------------------
// Vertex Shader - Implements Gouraud Shading using Diffuse lighting only
//------------------------------------------------------------------------------------
VS_OUTPUT VS(float4 Pos : POSITION, float3 NormalL : NORMAL, float2 texC : TEXCOORD)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.PosW = mul(Pos, World);
	output.Pos = mul(output.PosW, View);
	output.Pos = mul(output.Pos, Projection);

	// Convert from local space to world space 
	// W component of vector is 0 as vectors cannot be translated
	float3 normalW = mul(float4(NormalL, 0.0f), World).xyz;
	output.Norm = normalize(normalW);


	output.Tex = texC;

	return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( VS_OUTPUT input ) : SV_Target
{
	float3 toEye = normalize(eyePosW - input.PosW.xyz);

	float3 normalW = normalize(input.Norm);
	
	float4 textureColour = txDiffuse.Sample(samLinear, input.Tex);

	//
	// Compute Colour	
	//

	//Compute the reflection vector
	float3 r = reflect(-lightVecW, normalW);

	//Determine how much (if any) specular light makes it into camera eye
	float specularAmount = pow(max(dot(r, toEye), 0.0f), specularPower);

	//Diffuse Lighting
	float diffuseAmount = max(dot(lightVecW, normalW), 0.0f);
	float3 diffuse = diffuseAmount * (diffuseMtrl * diffuseLight).rgb;

	//Added Ambient Lighting
	float3 ambient = (ambientMtrl * ambientLight).rgb;

	// Compute the ambient, diffuse, and specular terms separately.
	float3 specular = specularAmount * (specularMtrl * specularLight).rgb;

	float4 colour;

	// Multiply the texture colour to the sum of amient and diffuse lighting then 
	// add finally add on specular light and copy over the diffuse alpha.
	colour.rgb = (textureColour * (ambient + diffuse)) + specular;
	colour.a = diffuseMtrl.a;

    return colour;
}
