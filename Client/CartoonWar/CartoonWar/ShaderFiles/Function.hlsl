#ifndef _FUNCTION
#define _FUNCTION
#include "Value.hlsl"


LIGHT Calculate_Light(int _iLightIdx, float4 _vNormal, float4 _vWorldPos)
{
	LIGHT tColor = (LIGHT)0.f;


	float4	vLightDir = normalize(tLight[_iLightIdx].vLightDir);
	float4	vNormal = _vNormal;
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
		float4	vWorldLightPos = mul(tLight[_iLightIdx].vLightPos, matWorld);

		vLightDir = normalize(_vWorldPos - vWorldLightPos);
		fDiffusePower = saturate(dot(-vLightDir, _vNormal));

		float fDistance = distance(_vWorldPos, vWorldLightPos);
		if (0.f == tLight[_iLightIdx].fRange)
			fRatio = 0.f;
		else
			fRatio = saturate(1.f - fDistance / tLight[_iLightIdx].fRange);

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

float4 Calculate_Shade(float4 vNormal, float4 _vWorldPos)
{
	float4	vTotalShade;
	float4	vLightDir;
	float	fRatio;
	for (int i = 0; i < iNumLight; ++i)
	{
		if (tLight[i].iLightType == 0)
		{
			vLightDir = normalize(tLight[i].vLightDir);
			vTotalShade += saturate(dot(-vLightDir, normalize(vNormal)));
		}
		else if (tLight[i].iLightType == 1)
		{
			float4	vWorldLightPos = mul(tLight[i].vLightPos, matWorld);
			float fDistance = distance(_vWorldPos, vWorldLightPos);

			if (0.f == tLight[i].fRange)
				continue;
			else
			{
				vLightDir = normalize(tLight[i].vLightDir);
				fRatio = saturate(1.f - fDistance / tLight[i].fRange);;
				vTotalShade = max(dot(-vLightDir, normalize(vNormal)), 0.f);
				vTotalShade *= fRatio;
			}
		
		}

	}

	return vTotalShade;
}



AD_Light Calculate_Light_Upgrade(int LightType, float4 _vNormal, float4 vWorldPos)
{
	AD_Light tColor = (AD_Light)0.f;


	float4	vNormal = _vNormal;
	float	fDiffusePower = 0.f;
	float	fSpecularPower = 0.f;
	float	fRatio = 1.f;


	int		iLightType = LightType;

	for (int i = 0; i < iNumLight; ++i)
	{
		if (tLight[i].iLightType == iLightType)
		{
			if (tLight[i].iLightType == 0)
			{
				float4	vLightDir = normalize(tLight[i].vLightDir);
				float4	vShade = max(dot(-vLightDir, normalize(vNormal)), 0.f);

				fDiffusePower = saturate(dot(-vLightDir, normalize(vNormal)));

				float4	vReflect = reflect(vLightDir, normalize(vNormal));
				float4	vLook = vWorldPos - vCamPos;
				float4	vEye = normalize(vLook);

				fSpecularPower = max(dot(-vEye, normalize(vReflect)), 0.f);
				fSpecularPower = pow(fSpecularPower, 1000.f);


				tColor.vDiffuse += fDiffusePower * tLight[i].tColor.vDiffuse * fRatio;
				tColor.vSpecular += fSpecularPower * tLight[i].tColor.vSpecular * fRatio;
				tColor.vAmbient += vShade + tLight[i].tColor.vAmbient;
				tColor.vShade += vShade;
			}
			else if (tLight[i].iLightType == 1)
			{
				float4	vLightWorldPos = tLight[i].vLightPos;
				float4	vLightDir = normalize(vWorldPos - vLightWorldPos);
				float4	vShade = max(dot(-vLightDir, normalize(vNormal)), 0.f);

				fDiffusePower = saturate(dot(-vLightDir, normalize(vNormal)));


		

				float4	vReflect = reflect(vLightDir, normalize(vNormal));
				float4	vLook = vWorldPos - vCamPos;
				vLook = normalize(vLook);

				fSpecularPower = max(dot(-vLook, normalize(vReflect)), 0.f);
				fSpecularPower = pow(fSpecularPower, 1000.f);

				float fDistance = distance(vWorldPos, vLightWorldPos);
				if (0.f == tLight[i].fRange)
					fRatio = 0.f;
				else
					fRatio = saturate(1.f - fDistance / tLight[i].fRange);


				tColor.vDiffuse += fDiffusePower * tLight[i].tColor.vDiffuse * fRatio;
				tColor.vSpecular += fSpecularPower * tLight[i].tColor.vSpecular * fRatio;
				tColor.vAmbient += vShade + tLight[i].tColor.vAmbient;
				tColor.vShade += vShade;
			}
			else
			{

			}
		}
		else continue;
	}

	return tColor;
}

#endif