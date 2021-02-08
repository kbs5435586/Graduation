#ifndef _FUNCTION
#define _FUNCTION
#include "Value.hlsl"


tagLightColor Calculate_Light(int _iLightIdx, float3 _vViewNormal, float3 _vViewPos)
{
	tagLightColor tColor = (tagLightColor)0.f;


	float3	vViewLightDir = (float3)0.f;
	float	fDiffusePower = 0.f;
	float	fSpecularPower = 0.f;
	float	fRatio = 1.f;

	if (tLight.iLightType == 0)
	{
		vViewLightDir = normalize(mul(float4(tLight.vLightDir.xyz, 0.f), matView).xyz);
		fDiffusePower = saturate(dot(-vViewLightDir, _vViewNormal));
	}
	else if (tLight.iLightType == 1)
	{


	}
	else
	{

	}

	float3	vReflect = normalize(vViewLightDir + 2 * (dot(-vViewLightDir, _vViewNormal) * _vViewNormal));
	float	vEye = normalize(_vViewPos);
	fSpecularPower = saturate(dot(-vEye, vReflect));
	fSpecularPower = pow(fSpecularPower, 20);

	tColor.vDiffuse = fDiffusePower * tLight.tColor.vDiffuse * fRatio;
	tColor.vSpecular = fSpecularPower * tLight.tColor.vSpecular * fRatio;
	tColor.vAmbient = tLight.tColor.vAmbient;
	return tColor;

}
#endif