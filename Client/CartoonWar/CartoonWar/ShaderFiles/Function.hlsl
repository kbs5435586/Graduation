#ifndef _FUNCTION
#define _FUNCTION
#include "Value.hlsl"

LIGHT Calculate_Light(int _iLightIdx, float3 _vViewNormal, float3 _vViewPos)
{
	LIGHT tCol = (LIGHT)0.f;


	float3 vViewLightDir = (float3) 0.f;
	float fDiffPow = 0.f;
	float fSpecPow = 0.f;
	float fRatio = 1.f;

	// Directional Light
	if (tLight[_iLightIdx].iLightType == 0)
	{
		// 광원의 방향   
		vViewLightDir = normalize(mul(float4(tLight[_iLightIdx].vLightDir.xyz, 0.f), matView).xyz);
		fDiffPow = saturate(dot(-vViewLightDir, _vViewNormal));
	}
	else if (tLight[_iLightIdx].iLightType == 1)
	{
		float3 vViewLightPos = mul(float4(tLight[_iLightIdx].vLightPos.xyz, 1.f), matView).xyz;
		vViewLightDir = normalize(_vViewPos - vViewLightPos);

		fDiffPow = saturate(dot(-vViewLightDir, _vViewNormal));

		// Ratio 계산
		float fDistance = distance(_vViewPos, vViewLightPos);
		if (0.f == tLight[_iLightIdx].fRange)
			fRatio = 0.f;
		else
			fRatio = saturate(1.f - fDistance / tLight[_iLightIdx].fRange);
	}

	// 반사 방향
	float3 vReflect = normalize(vViewLightDir + 2 * (dot(-vViewLightDir, _vViewNormal) * _vViewNormal));
	float3 vEye = normalize(_vViewPos);
	fSpecPow = saturate(dot(-vEye, vReflect));
	fSpecPow = pow(fSpecPow, 100.f);

	tCol.vDiffuse = fDiffPow * tLight[_iLightIdx].tColor.vDiffuse * fRatio;
	tCol.vSpecular = fSpecPow * tLight[_iLightIdx].tColor.vSpecular * fRatio;
	tCol.vAmbient = tLight[_iLightIdx].tColor.vAmbient;

	return tCol;
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

AD_Light Calculate_Light_Upgrade_V2(int iLightIdx, float4 vNormal, float4 vWorldPos)
{
	AD_Light tCol = (AD_Light)0;

	float	fDiffusePower = 0.f;
	float	fSpecularPower = 0.f;
	float	fRatio = 0.f;


	if (tLight[iLightIdx].iLightType == 0)
	{
		float4	vLightDir = normalize(tLight[iLightIdx].vLightDir);
		float4	vShade = max(dot(-vLightDir, normalize(vNormal)), 0.f);


		fDiffusePower = saturate(dot(-vLightDir, normalize(vNormal)));


		float4	vReflect = reflect(vLightDir, normalize(vNormal));
		float4	vLook = vWorldPos - vCamPos;
		float4	vEye = normalize(vLook);

		fSpecularPower = max(dot(-vEye, normalize(vReflect)), 0.f);
		fSpecularPower = pow(fSpecularPower, 1000.f);


		tCol.vDiffuse = fDiffusePower * tLight[iLightIdx].tColor.vDiffuse * fRatio;
		tCol.vSpecular = fSpecularPower * tLight[iLightIdx].tColor.vSpecular * fRatio;
		tCol.vAmbient = vShade + tLight[iLightIdx].tColor.vAmbient;
		tCol.vShade = vShade;
	}
	else if (tLight[iLightIdx].iLightType == 1)
	{
		float4	vLightWorldPos = tLight[iLightIdx].vLightPos;
		float4	vLightDir = normalize(vWorldPos - vLightWorldPos);
		float4	vShade = max(dot(-vLightDir, normalize(vNormal)), 0.f);

		fDiffusePower = saturate(dot(-vLightDir, normalize(vNormal)));




		float4	vReflect = reflect(vLightDir, normalize(vNormal));
		float4	vLook = vWorldPos - vCamPos;
		vLook = normalize(vLook);

		fSpecularPower = max(dot(-vLook, normalize(vReflect)), 0.f);
		fSpecularPower = pow(fSpecularPower, 1000.f);

		float fDistance = distance(vWorldPos, vLightWorldPos);
		if (0.f == tLight[iLightIdx].fRange)
			fRatio = 0.f;
		else
			fRatio = saturate(1.f - fDistance / tLight[iLightIdx].fRange);


		tCol.vDiffuse = fDiffusePower * tLight[iLightIdx].tColor.vDiffuse * fRatio;
		tCol.vSpecular = fSpecularPower * tLight[iLightIdx].tColor.vSpecular * fRatio;
		tCol.vAmbient = vShade + tLight[iLightIdx].tColor.vAmbient;
		tCol.vShade = vShade;
	}
	else
	{

	}

	return tCol;
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

float3 gaussian5x5Sample_2(in int2 _uv, in Texture2D _tex)
{
	float4 fOut = (float4) 0.f;
	for (int i = 0; i < 5; ++i)
	{
		for (int j = 0; j < 5; ++j)
		{
			fOut += _tex[_uv + int2(i - 2, j - 2)] * gaussian5x5[i * 5 + j];
		}
	}
	return fOut.xyz;
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
	return g_arrFinalBoneMat[(g_int_1 * _iRowIdx) + _iBoneIdx];
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

	_vPos = (info.vPos);
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

float CalTessLevel(in float3 _vWorldCamPos, float3 _vPatchPos, float _fMin, float _fMax, float _fMaxLv)
{
	float fLen = length(_vPatchPos - _vWorldCamPos);

	float fLevel = (_fMaxLv - 1.f) * ((1.f - saturate((fLen - _fMin) / (_fMax - _fMin))));
	if (fLevel == _fMaxLv - 1.f)
	{
		fLevel += 1.f;
	}

	fLevel = pow(2, fLevel);

	return fLevel;
}

#endif