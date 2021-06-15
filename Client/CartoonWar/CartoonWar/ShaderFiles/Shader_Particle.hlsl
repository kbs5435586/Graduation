#include "Value.hlsl"
#include "Function.hlsl"

struct tParticle
{
	float3 vWorldPos; // 위치	
	float3 vWorldDir; // 이동 방향

	float m_fCurTime; // 현재 시간
	float m_fLifeTime; // 최대 생명주기

	int iAlive;
	int arrPading[3]; // 16 바이트 배수 패딩 바이트
};

struct tParticleShared
{
	int iAddCount;
	int iCurCount;
	float fRange; // 랜덤 범위
	int arrPading;
};



StructuredBuffer<tParticle> tData : register(t10);
RWStructuredBuffer<tParticle> tRWData : register(u0);
RWStructuredBuffer<tParticleShared> tRWSharedData : register(u1);

struct VS_IN
{
	float3 vPosition : POSITION;
	float2 vUV : TEXCOORD;

	uint iID : SV_InstanceID;
};
struct VS_OUT
{
	float4 vViewPos : POSITION;
	float2 vUV : TEXCOORD;
	float iInstID : FOG;
};

struct GS_OUT
{
	float4 vPosition : SV_Position;
	float2 vUV : TEXCOORD;
	uint iInstID : SV_InstanceID;
};

VS_OUT	VS_Main(VS_IN vIn)
{
	VS_OUT	vOut;


	float3 vWorldPos = mul(float4(vIn.vPosition, 1.f), matWorld).xyz;
	vWorldPos += tData[vIn.iID].vWorldPos;

	vOut.vViewPos = mul(float4(vWorldPos, 1.f), matView);
	vOut.vUV = vIn.vUV;
	vOut.iInstID = vIn.iID;

	return vOut;
}

[maxvertexcount(6)]
void GS_Main(point VS_OUT _in[1], inout TriangleStream<GS_OUT> OutputStream)
{
	GS_OUT output[4] =
	{
		(GS_OUT)0.f, (GS_OUT)0.f, (GS_OUT)0.f, (GS_OUT)0.f
	};
	uint iInstID = (uint) _in[0].iInstID;

	if (0 == tData[iInstID].iAlive)
		return;

	float fRatio = tData[iInstID].m_fCurTime / tData[iInstID].m_fLifeTime;
	float fCurScale = ((g_float_1 - g_float_0) * fRatio + g_float_0) / 2.f;

	output[0].vPosition = _in[0].vViewPos + float4(-fCurScale, fCurScale, 0.f, 0.f);
	output[1].vPosition = _in[0].vViewPos + float4(fCurScale, fCurScale, 0.f, 0.f);
	output[2].vPosition = _in[0].vViewPos + float4(fCurScale, -fCurScale, 0.f, 0.f);
	output[3].vPosition = _in[0].vViewPos + float4(-fCurScale, -fCurScale, 0.f, 0.f);

	output[0].vPosition = mul(output[0].vPosition, matProj);
	output[1].vPosition = mul(output[1].vPosition, matProj);
	output[2].vPosition = mul(output[2].vPosition, matProj);
	output[3].vPosition = mul(output[3].vPosition, matProj);

	output[0].vUV = float2(0.f, 0.f);
	output[1].vUV = float2(1.f, 0.f);
	output[2].vUV = float2(1.f, 1.f);
	output[3].vUV = float2(0.f, 1.f);

	output[0].iInstID = iInstID;
	output[1].iInstID = iInstID;
	output[2].iInstID = iInstID;
	output[3].iInstID = iInstID;


	OutputStream.Append(output[0]);
	OutputStream.Append(output[1]);
	OutputStream.Append(output[2]);
	OutputStream.RestartStrip();

	OutputStream.Append(output[0]);
	OutputStream.Append(output[2]);
	OutputStream.Append(output[3]);
	OutputStream.RestartStrip();
}


float4	PS_Main(GS_OUT _in) : SV_TARGET
{
	float4 vOut = (float4)0;


	float fRatio = tData[_in.iInstID].m_fCurTime / tData[_in.iInstID].m_fLifeTime;
	float4 vCurColor = (g_vec4_1 - g_vec4_0) * fRatio + g_vec4_0;
	 vCurColor *= g_texture0.Sample(Sampler0, _in.vUV);

	vOut = vCurColor;

	return vOut;
}




[numthreads(1024, 1, 1)]
void CS_Main(int3 _iThreadIdx : SV_DispatchThreadID)
{
    if (_iThreadIdx.x >= g_int_0)
        return;

    tRWSharedData[0].iAddCount = g_int_1;

    // Dead 파티클 살리기
    if (0 == tRWData[_iThreadIdx.x].iAlive)
    {
        int iOrigin = tRWSharedData[0].iAddCount;
        int iExchange = 0;
        while (0 < iOrigin)
        {
            int iInputValue = iOrigin - 1;
            InterlockedExchange(tRWSharedData[0].iAddCount, iInputValue, iExchange);

            if (iExchange == iOrigin)
            {
                tRWData[_iThreadIdx.x].iAlive = 1;
                break;
            }

            iOrigin = iInputValue;
        }

        if (1 == tRWData[_iThreadIdx.x].iAlive)
        {
            // 랜덤 생성 위치, 방향           
            float2 vUV = float2(((float)_iThreadIdx.x / (float)g_int_0) + g_fAccTime, g_fAccTime);
            vUV.y += sin(vUV.x * 2 * 3.141592);

            if (vUV.x > 0)
                vUV.x = frac(vUV.x);
            else
                vUV.x = ceil(abs(vUV.x)) - abs(vUV.x);

            if (vUV.y > 0)
                vUV.y = frac(vUV.y);
            else
                vUV.y = ceil(abs(vUV.y)) - abs(vUV.y);

            vUV = vUV * g_vec2_0;

            float3 vNoise =
            {
                gaussian5x5Sample(vUV + int2(0, -100), g_texture0)
                , gaussian5x5Sample(vUV + int2(0, 0), g_texture0)
                , gaussian5x5Sample(vUV + int2(0, 100), g_texture0)
            };


            float3 vDir = (float4) 0.f;

            vDir = (vNoise - 0.5f) * 2.f;

            //vDir = float3(0.f, 1.f, 0.f);
            //vDir.x = (vNoise.x - 0.5f) * 2.f * sin(3.141592 / 8.f);
            //vDir.z = (vNoise.z - 0.5f) * 2.f * sin(3.141592 / 8.f);

            tRWData[_iThreadIdx.x].vWorldDir = normalize(vDir); //normalize((vNoise.xyz - 0.5f) * 2.f);
            tRWData[_iThreadIdx.x].vWorldPos = (vNoise.xyz - 0.5f) * 50;
            tRWData[_iThreadIdx.x].m_fLifeTime = ((g_float_1 - g_float_0) * vNoise.x) + g_float_0;
            tRWData[_iThreadIdx.x].m_fCurTime = 0.f;
        }
    }
    else // Particle Update 하기
    {
        tRWData[_iThreadIdx.x].m_fCurTime += g_fDT;
        if (tRWData[_iThreadIdx.x].m_fLifeTime < tRWData[_iThreadIdx.x].m_fCurTime)
        {
            tRWData[_iThreadIdx.x].iAlive = 0;
            return;
        }

        float fRatio = tRWData[_iThreadIdx.x].m_fCurTime / tRWData[_iThreadIdx.x].m_fLifeTime;
        float fSpeed = (g_float_3 - g_float_2) * fRatio + g_float_2;
		float3 vNormalizeDir = normalize(tRWData[_iThreadIdx.x].vWorldDir);
        tRWData[_iThreadIdx.x].vWorldPos += vNormalizeDir * fSpeed * g_fDT;
		//tRWData[_iThreadIdx.x].vWorldPos.y += 2.f;
        // 생존 파티클 개수 확인
        tRWSharedData[0].iCurCount = 0;
        InterlockedAdd(tRWSharedData[0].iCurCount, 1);
    }
}
