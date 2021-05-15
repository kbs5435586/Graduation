
#include "../Value.hlsl"
RWTexture2D<float4>	g_rwtex_0	: register(u0);


[numthreads(5, 1, 1)] // 1024 -> Group, hlsl 5.0 기준 그룹당 스레드 최대 개수는 1024
void CS_Main(int3 _iThreadIdx : SV_DispatchThreadID)
{



	if (_iThreadIdx.x < g_int_0)
	{
		g_rwtex_0[_iThreadIdx.xy] = float4(0.f, 0.f, 1.f, 1.f);
	}
	else
	{
		g_rwtex_0[_iThreadIdx.xy] = float4(0.f, 0.f, 0.f, 1.f);
	}
}
 