
#include "../Value.hlsl"
RWTexture2D<float4>	g_rwtex_0	: register(u0);


[numthreads(1000, 1, 1)] // 1024 -> Group, hlsl 5.0 ���� �׷�� ������ �ִ� ������ 1024
void CS_Main(int3 _iThreadIdx : SV_DispatchThreadID)
{
    if(g_int_3)
        g_rwtex_0[_iThreadIdx.xy] = float4(0.f, 0.f, 0.f, 1.f);

    else
    {
        if (g_int_0)    // Team Red
        {
            if (_iThreadIdx.y <= g_float_0 * 100.f)
                g_rwtex_0[_iThreadIdx.xy] = float4(1.f, 0.f, 0.f, 1.f);
        }
        if (g_int_1)   // Team Blue
        {
            if (_iThreadIdx.y <= g_float_0 * 100.f)
                g_rwtex_0[_iThreadIdx.xy] = float4(0.f, 0.f, 1.f, 1.f);
        }
    }


}
 