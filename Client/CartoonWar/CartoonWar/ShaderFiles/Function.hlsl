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





static float gaussian5x5[25] =
{
	0.003765, 0.015019, 0.023792, 0.015019, 0.003765,
	0.015019, 0.059912, 0.094907, 0.059912, 0.015019,
	0.023792, 0.094907, 0.150342, 0.094907, 0.023792,
	0.015019, 0.059912, 0.094907, 0.059912, 0.015019,
	0.003765, 0.015019, 0.023792, 0.015019, 0.003765,
};

static float gaussian3x3[9] =
{
	0.035f, 0.16f, 0.035f,
	0.16f, 0.22f, 0.16f,
	0.035f, 0.16f, 0.035f,
};

float4 gaussian3x3Sample(in int2 _uv, in RWTexture2D<float4> _tex)
{
	float4 fOut = (float4) 0.f;
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			fOut += _tex[_uv + int2(i - 1, j - 1)] * gaussian3x3[i * 3 + j];
		}
	}
	return fOut;
}


float gaussian5x5Sample(in int2 _uv, in Texture2D _tex)
{
	float4 fOut = (float4) 0.f;
	for (int i = 0; i < 5; ++i)
	{
		for (int j = 0; j < 5; ++j)
		{
			fOut += _tex[_uv + int2(i - 2, j - 2)] * gaussian5x5[i * 5 + j];
		}
	}
	return fOut.x;
}

struct tSkinningInfo
{
	float3 vPos;
	float3 vTangent;
	float3 vBinormal;
	float3 vNormal;
};

matrix GetBoneMat(int _iBoneIdx, int _iRowIdx)
{
	return g_arrFinalBoneMat[_iBoneIdx];
}

void Skinning(inout float3 _vPos, inout float3 _vTangent, inout float3 _vBinormal, inout float3 _vNormal
	, inout float4 _vWeight, inout float4 _vIndices
	, int _iRowIdx)
{
	tSkinningInfo info = (tSkinningInfo)0.f;

	for (int i = 0; i < 4; ++i)
	{
		if (0.f == _vWeight[i])
			continue;

		matrix matBone = GetBoneMat((int)_vIndices[i], _iRowIdx);

		info.vPos += (mul(float4(_vPos, 1.f), matBone) * _vWeight[i]).xyz;
		info.vTangent += (mul(float4(_vTangent, 0.f), matBone) * _vWeight[i]).xyz;
		info.vBinormal += (mul(float4(_vBinormal, 0.f), matBone) * _vWeight[i]).xyz;
		info.vNormal += (mul(float4(_vNormal, 0.f), matBone) * _vWeight[i]).xyz;
	}

	_vPos = normalize(info.vPos);
	_vTangent = normalize(info.vTangent);
	_vBinormal = normalize(info.vBinormal);
	_vNormal = normalize(info.vNormal);
}


void Skinning(inout float3 _vPos, inout float4 _vWeight, inout float4 _vIndices, int _iRowIdx)
{
	tSkinningInfo info = (tSkinningInfo)0.f;

	for (int i = 0; i < 4; ++i)
	{
		if (0.f == _vWeight[i])
			continue;

		matrix matBone = GetBoneMat((int)_vIndices[i], _iRowIdx);

		info.vPos += (mul(float4(_vPos, 1.f), matBone) * _vWeight[i]).xyz;
	}

	_vPos = info.vPos;
}

#endif