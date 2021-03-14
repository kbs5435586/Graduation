#include "Value.hlsl"
#include "Function.hlsl"
RWStructuredBuffer<tParticle> tRWData : register(u0);
RWStructuredBuffer<tParticleShared> tRWSharedData : register(u1);

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
            //InterlockedExchange(tRWSharedData[0].iAddCount, iInputValue, iExchange);
            InterlockedCompareExchange(tRWSharedData[0].iAddCount, iOrigin, iInputValue, iExchange);

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
        tRWData[_iThreadIdx.x].vWorldPos += tRWData[_iThreadIdx.x].vWorldDir * fSpeed * g_fDT;

        // 생존 파티클 개수 확인
        //tRWSharedData[0].iCurCount = 0;
        //InterlockedAdd(tRWSharedData[0].iCurCount, 1);
    }
}
