#ifndef _FUNCTION
#define _FUNCTION
#include "Value.hlsl"


LIGHT Calculate_Light(int _iLightIdx, float3 _vNormal, float4 _vWorldPos)
{
	LIGHT tColor = (LIGHT)0.f;


	float4	vLightDir = normalize(tLight[_iLightIdx].vLightDir);
	float4	vNormal = float4(_vNormal, 0.f);
	float	fDiffusePower = 0.f;
	float	fSpecularPower = 0.f;
	float	fRatio = 1.f;


	float4	vShade = max(dot(-vLightDir, normalize(vNormal)), 0.f);


	if (tLight[_iLightIdx].iLightType == 0)
	{
		//fDiffusePower = saturate(dot(-vLightDir, normalize(_vNormal)) );
		fDiffusePower = saturate(dot(-vLightDir, normalize(vNormal)));
		
	}
	else if (tLight[_iLightIdx].iLightType == 1)
	{


	}
	else
	{

	}
	float4	vReflect = reflect(vLightDir, normalize(vNormal));
	float4	vLook	 = _vWorldPos - vCamPos;
	float4	vEye	 = normalize(vLook);

	fSpecularPower	 = max(dot(-vEye, normalize(vReflect)), 0.f);
	fSpecularPower	 = pow(fSpecularPower, 200.f);


	tColor.vDiffuse = fDiffusePower * tLight[_iLightIdx].tColor.vDiffuse * fRatio;
	tColor.vSpecular = fSpecularPower * tLight[_iLightIdx].tColor.vSpecular * fRatio;
	tColor.vAmbient = vShade + tLight[_iLightIdx].tColor.vAmbient;
	return tColor;

}

#endif