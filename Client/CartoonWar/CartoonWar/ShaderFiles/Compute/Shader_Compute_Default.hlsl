
#include "../Value.hlsl"
RWTexture2D<float4>	g_rwtex_0	: register(u0);


[numthreads(1024, 1, 1)] // 1024 -> Group, hlsl 5.0 ���� �׷�� ������ �ִ� ������ 1024
void CS_Main(int3 _iThreadIdx : SV_DispatchThreadID)
{
    g_rwtex_0[_iThreadIdx.xy] = float4(0.f, 1.f, 0.f, 1.f);

}
