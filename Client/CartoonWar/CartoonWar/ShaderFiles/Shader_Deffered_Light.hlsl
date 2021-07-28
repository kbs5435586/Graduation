#include "Value.hlsl"
#include "Function.hlsl"

float	g_fRadius = 0.001f;
float	g_fFar = 300.f;
float	g_fFallOff = 0.000002f;
float	g_fStrength = 0.0007;
float	g_fTotStrength = 1.38f;
float	g_fInSamples = 1.f / 16.f;

float3 g_vRandom[16] =
{
	float3(0.2024537f, 0.841204f, -0.9060141f),
	float3(-0.2200423f, 0.6282339f, -0.8275437f),
	float3(0.3677659f, 0.1086345f, -0.4466777f),
	float3(0.8775856f, 0.4617546f, -0.6427765f),
	float3(0.7867433f, -0.141479f, -0.1567597f),
	float3(0.4839356f, -0.8253108f, -0.1563844f),
	float3(0.4401554f, -0.4228428f, -0.3300118f),
	float3(0.0019193f, -0.8048455f, 0.0726584f),
	float3(-0.7578573f, -0.5583301f, 0.2347527f),
	float3(-0.4540417f, -0.252365f, 0.0694318f),
	float3(-0.0483353f, -0.2527294f, 0.5924745f),
	float3(-0.4192392f, 0.2084218f, -0.3672943f),
	float3(-0.8433938f, 0.1451271f, 0.2202872f),
	float3(-0.4037157f, -0.8263387f, 0.4698132f),
	float3(-0.6657394f, 0.6298575f, 0.6342437f),
	float3(-0.0001783f, 0.2834622f, 0.8343929f),


};
struct tagSSAO_In
{
	float2 vUV;
	float fDepth;
	float fViewZ;
	float3 vNormal;
};

struct tagSSAO_Out
{
	float4 vAmbient;
};

float3 randomNormal(float2 tex)
{
	float noiseX = (frac(sin(dot(tex, float2(15.8989f, 76.132f) * 1.0f)) * 46336.23745f));
	float noiseY = (frac(sin(dot(tex, float2(11.9899f, 62.223f) * 2.0f)) * 34748.34744f));
	float noiseZ = (frac(sin(dot(tex, float2(13.3238f, 63.122f) * 3.0f)) * 59998.47362f));
	return normalize(float3(noiseX, noiseY, noiseZ));
}


tagSSAO_Out Get_SSAO(tagSSAO_In In)
{
	tagSSAO_Out Out = (tagSSAO_Out)0.f;

	float3 vRay;
	float3 vReflect;
	float2 vRandomUV;
	float fOccNorm;

	int iColor = 0;

	for (int i = 0; i < 16; ++i)
	{
		vRay = reflect(randomNormal(In.vUV), g_vRandom[i]);
		vReflect = normalize(reflect(vRay, In.vNormal)) * g_fRadius;
		vReflect.x *= -1.f;
		vRandomUV = In.vUV + vReflect.xy;
		fOccNorm = g_texture3.Sample(Sampler0, vRandomUV).r * g_fFar * In.fViewZ;

		if (fOccNorm <= In.fDepth + 0.0003f)
			++iColor;
	}

	Out.vAmbient = abs((iColor / 16.f) );
	return Out;

}

float4 Calculate_Toon(float4 InvNormal, float4 InvPosition)
{
	float4 vOut;
	matrix matInvView = g_mat_1;

	float4 vNormal = mul(InvNormal, matInvView);
	float4 vPosition = mul(InvPosition, matInvView);

	float4	vLightDir = -1.f * normalize(tLight[0].vLightDir);
	float	fDot = saturate(dot(vNormal, vLightDir));
	fDot = (ceil(fDot * 3.f) / 3.f);

	float4	vMtrlDif = float4(fDot, fDot, fDot, 0.f);
	float3	fRimColor = float3(-2.f,-2.f,-2.f);

	float4	vView = normalize(vCamPos - vPosition);
	float	fRim = saturate(dot(vNormal, vView));

	if (fRim > 0.3f)
		fRim = 1.f;
	else
		fRim = -1.f;

	float	fRimPower = 10.f;

	float4	vMtrlEmiv = float4(pow(1.f - fRim, fRimPower) * fRimColor, 1.f);

	vOut = vMtrlDif + vMtrlEmiv;

	return vOut;
	
}




struct VS_IN
{
	float3	vPosition	: POSITION;
	float2	vTexUV		: TEXCOORD0;
};

struct VS_OUT
{
	float4	vPosition	: SV_POSITION;
	float2	vTexUV		: TEXCOORD0;
};

struct PS_OUT
{
	float4	vDiffuse	: SV_TARGET0;
	float4	vSpecular	: SV_TARGET1;
	float4	vPointLight	: SV_TARGET2;
};


VS_OUT	VS_DirLight(VS_IN vIn)
{
	VS_OUT	vOut;

	vOut.vPosition = float4(vIn.vPosition*2.f ,1.f);
	vOut.vTexUV = vIn.vTexUV;
	return vOut;
}


PS_OUT	PS_DirLight(VS_OUT vIn)
{
	PS_OUT vOut = (PS_OUT)0;

	float4 vPosition = g_texture1.Sample(Sampler0, vIn.vTexUV);
	if (vPosition.z <= 1.f)
	{
		clip(-1);
	}
	float4 vNormalTex = g_texture0.Sample(Sampler0, vIn.vTexUV);
	float4 vDepth = g_texture3.Sample(Sampler0, vIn.vTexUV);
	float4 vNormal = mul(vNormalTex, matViewInv);


	LIGHT tCurCol = Calculate_Light(0, vNormalTex.xyz, vPosition.xyz);
	LIGHT tPointCurCol = Calculate_Light(1, vNormalTex.xyz, vPosition.xyz);


	if (dot(tCurCol.vDiffuse, tCurCol.vDiffuse) != 0.f)
	{
		float4 vWorldPos = mul(vPosition, matViewInv); 
		float4 vShadowProj = mul(vWorldPos, g_mat_0);
		float fDepth = vShadowProj.z / vShadowProj.w;
		float2 vShadowUV = float2((vShadowProj.x / vShadowProj.w) * 0.5f + 0.5f
			, (vShadowProj.y / vShadowProj.w) * -0.5f + 0.5f);

		if (0.01f < vShadowUV.x && vShadowUV.x < 0.99f && 0.01f < vShadowUV.y && vShadowUV.y < 0.99f)
		{
			float fShadowDepth = g_texture2.Sample(Sampler0, vShadowUV).r;

			// 그림자인 경우 빛을 약화시킨다.
			if (fShadowDepth && (fDepth > fShadowDepth + 0.00001f))
			{
				tCurCol.vDiffuse *= 0.01f;
				tCurCol.vSpecular = 0.f;
			}

		}
	}


	vOut.vDiffuse = tCurCol.vDiffuse + tCurCol.vAmbient;
	vOut.vSpecular = tCurCol.vSpecular;
	vOut.vPointLight = tPointCurCol.vDiffuse;

	return vOut;
}



VS_OUT	VS_PointLight(VS_IN vIn)
{
	VS_OUT	vOut;

	vOut.vPosition = float4(vIn.vPosition * 2.f, 1.f);
	vOut.vTexUV = vIn.vTexUV;
	return vOut;
}


PS_OUT	PS_PointLight(VS_OUT vIn)
{
	PS_OUT vOut = (PS_OUT)0;

	float4 vPosition = g_texture1.Sample(Sampler0, vIn.vTexUV);
	if (vPosition.z <= 1.f)
	{
		clip(-1);
	}
	float4 vNormalTex = g_texture0.Sample(Sampler0, vIn.vTexUV);
	float4 vDepth = g_texture3.Sample(Sampler0, vIn.vTexUV);
	float4 vNormal = mul(vNormalTex, matViewInv);

	LIGHT tCurCol = Calculate_Light(1, vNormalTex.xyz, vPosition.xyz);

	if (dot(tCurCol.vDiffuse, tCurCol.vDiffuse) != 0.f)
	{
		float4 vWorldPos = mul(vPosition, matViewInv);
		float4 vShadowProj = mul(vWorldPos, g_mat_0);
		float fDepth = vShadowProj.z / vShadowProj.w;
		float2 vShadowUV = float2((vShadowProj.x / vShadowProj.w) * 0.5f + 0.5f
			, (vShadowProj.y / vShadowProj.w) * -0.5f + 0.5f);

		if (0.01f < vShadowUV.x && vShadowUV.x < 0.99f && 0.01f < vShadowUV.y && vShadowUV.y < 0.99f)
		{
			float fShadowDepth = g_texture2.Sample(Sampler0, vShadowUV).r;

			// 그림자인 경우 빛을 약화시킨다.
			if (fShadowDepth && (fDepth > fShadowDepth + 0.00001f))
			{
				tCurCol.vDiffuse *= 0.01f;
				tCurCol.vSpecular = 0.f;
			}

		}
	}


	vOut.vPointLight = tCurCol.vDiffuse;
	return vOut;
}
 