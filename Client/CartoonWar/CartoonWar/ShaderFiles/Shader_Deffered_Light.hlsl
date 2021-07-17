#include "Value.hlsl"
#include "Function.hlsl"


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
};


VS_OUT	VS_DirLight(VS_IN vIn)
{
	VS_OUT	vOut;

	vOut.vPosition = float4(vIn.vPosition*2.f ,1.f);
	//vOut.vPosition = mul(float4(vIn.vPosition, 1.f), matWVP);
	vOut.vTexUV = vIn.vTexUV;
	return vOut;
}


PS_OUT	PS_DirLight(VS_OUT vIn)
{
	PS_OUT vOut = (PS_OUT)0;

	float4 vNormalTex = g_texture0.Sample(Sampler0, vIn.vTexUV);
	float4 vPosition = g_texture1.Sample(Sampler0, vIn.vTexUV);
	//vNormalTex = float4(vNormalTex.xyz * 2.f - 1.f, 0.f);
	//vPosition = float4(vPosition.xyz * 2.f - 1.f, 0.f);
	if (vPosition.z <= 1.f)
	{
		clip(-1);
	}

	LIGHT tCurCol = Calculate_Light(0, vNormalTex.xyz, vPosition.xyz);
	if (dot(tCurCol.vDiffuse, tCurCol.vDiffuse) != 0.f)
	{
		float4 vWorldPos = mul(vPosition, matViewInv); // ����ī�޶� view ������� ���ؼ� ������ǥ�� �˾Ƴ���.
		//vWorldPos = mul(vPosition, matWorld); // ����ī�޶� view ������� ���ؼ� ������ǥ�� �˾Ƴ���.
		float4 vShadowProj = mul(vWorldPos, g_mat_0); // ���� �������� ������Ų ��ǥ ���ϱ�
		float fDepth = vShadowProj.z / vShadowProj.w; // w �� ������ ���� ������ǥ z ���� ���Ѵ�.(�ùٸ��� ���ϱ� ���ؼ�)

		// ���� ������ǥ�� UV ��ǥ�� �����ؼ� ShadowMap �� ��ϵ� ���̰��� �����´�.
		float2 vShadowUV = float2((vShadowProj.x / vShadowProj.w) * 0.5f + 0.5f
			, (vShadowProj.y / vShadowProj.w) * -0.5f + 0.5f);

		if (0.01f < vShadowUV.x && vShadowUV.x < 0.99f && 0.01f < vShadowUV.y && vShadowUV.y < 0.99f)
		{
			float fShadowDepth = g_texture2.Sample(Sampler0, vShadowUV).r;

			// �׸����� ��� ���� ��ȭ��Ų��.
			if (fShadowDepth && (fDepth > fShadowDepth + 0.00001f))
			{
				tCurCol.vDiffuse = 0.f;
				tCurCol.vSpecular *= 0.01f;
			}

		}
	}
	vOut.vDiffuse = tCurCol.vDiffuse + tCurCol.vAmbient;
	vOut.vSpecular = tCurCol.vSpecular;

	return vOut;
}
 