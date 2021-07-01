#include "Value.hlsl"
#include "Function.hlsl"
struct VS_IN
{
    float3	vPosition	: POSITION;
    float4	vColor		: COLOR;
    float2	vTexUV		: TEXCOORD;
    float3	vNormal		: NORMAL;
    float3	vTangent	: TANGENT;
    float3	vBinormal	: BINORMAL;
    float4	vWeight		: BLENDWEIGHT;
    float4	vIndices	: BLENDINDICES;
};


struct VS_OUT
{
    float4 vPosition : SV_Position;
    float4 vProj : POSITION;
};

VS_OUT VS_Main(VS_IN vIn)
{
    VS_OUT output = (VS_OUT)0.f;

    if (g_int_0)
    {
        Skinning(vIn.vPosition, vIn.vWeight, vIn.vIndices, 0);
    }

    //output.vPosition = mul(float4(vIn.vPosition, 1.f), matWVP);
    //output.vProj = output.vPosition;

    return output;
}

float4 PS_Main(VS_OUT vIn) : SV_Target
{
    float Depth = vIn.vProj.z / vIn.vProj.w;
    Depth -= 1;
    Depth *= -1;
    return float4(Depth.xxx, 1);
}
