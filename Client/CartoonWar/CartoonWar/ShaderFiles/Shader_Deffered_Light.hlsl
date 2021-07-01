#include "Value.hlsl"
#include "Function.hlsl"

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
	
	if (vPosition.z <= 1.f)
	{
		clip(-1);
	}

	LIGHT tCurCol = Calculate_Light(0, vNormalTex.xyz, vPosition.xyz);

	if (dot(tCurCol.vDiffuse, tCurCol.vDiffuse) != 0.f)
	{
		float4 vWorldPos = mul(vPosition, matViewInv); // 메인카메라 view 역행렬을 곱해서 월드좌표를 알아낸다.
		float4 vShadowProj = mul(vWorldPos, g_mat_0); // 광원 시점으로 투영시킨 좌표 구하기
		float fDepth = vShadowProj.z / vShadowProj.w; // w 로 나눠서 실제 투영좌표 z 값을 구한다.(올바르게 비교하기 위해서)

		// 계산된 투영좌표를 UV 좌표로 변경해서 ShadowMap 에 기록된 깊이값을 꺼내온다.
		float2 vShadowUV = float2((vShadowProj.x / vShadowProj.w) * 0.5f + 0.5f
			, (vShadowProj.y / vShadowProj.w) * -0.5f + 0.5f);

		if (0.01f < vShadowUV.x && vShadowUV.x < 0.99f && 0.01f < vShadowUV.y && vShadowUV.y < 0.99f)
		{
			float fShadowDepth = g_texture2.Sample(Sampler0, vShadowUV).r;

			// 그림자인 경우 빛을 약화시킨다.
			if (fShadowDepth!=0.f && (fDepth > fShadowDepth + 0.0000125f))
			{
				tCurCol.vDiffuse *= 0.1f;
				//tCurCol.vSpec = (float4) 0.f;
			}
			//else if((fDepth <= fShadowDepth + 0.00001f))
			//{
			//	fTemp = 0.f;
			//}
		}


	}


	float4 vWorldPos = mul(vPosition, matViewInv);
	vOut.vDiffuse = tCurCol.vDiffuse + tCurCol.vAmbient;

	return vOut;
}
