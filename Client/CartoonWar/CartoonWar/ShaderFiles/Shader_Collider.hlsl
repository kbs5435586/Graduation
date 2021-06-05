#include "Value.hlsl"
#include "Function.hlsl"

struct VS_IN
{
	float3	vPosition	: POSITION;
	float4	vColor		: COLOR;
};
struct VS_OUT
{
	float4	vPosition	: SV_POSITION;
	float4	vColor		: COLOR;
};
struct PS_OUT
{
	float4 vDiffuseTex			: SV_TARGET0;
	float4 vDiffuseTex_			: SV_TARGET1;
};


//_vec3 vRight = _vec3(1.f, 0.f, 0.f);
//_vec3 vUp = _vec3(0.f, 1.f, 0.f);
//_vec3 vLook = _vec3(0.f, 0.f, 1.f);
//
//_vec3 vRightTemp = _vec3(matWorld.m[0][0], matWorld.m[0][1], matWorld.m[0][2]);
//_vec3 vUpTemp = _vec3(matWorld.m[1][0], matWorld.m[1][1], matWorld.m[1][2]);
//_vec3 vLookTemp = _vec3(matWorld.m[2][0], matWorld.m[2][1], matWorld.m[2][2]);
//
//vRight *= Vector3_::Length(vRightTemp);
//vUp *= Vector3_::Length(vUpTemp);
//vLook *= Vector3_::Length(vLookTemp);

VS_OUT	VS_Main(VS_IN vIn)
{
	VS_OUT	vOut;
	if (g_int_0)
	{
		matrix matTemp = (g_matTemp[g_int_1]);
		//Remove Rotate
		{
			//float3	vRight = float3(1.f, 0.f, 0.f);
		//float3	vUp = float3(0.f, 1.f, 0.f);
		//float3	vLook = float3(0.f, 0.f, 1.f);

		//float3	vRightTemp = float3(matTemp._11, matTemp._12, matTemp._13);
		//float3	vUpTemp = float3(matTemp._21, matTemp._22, matTemp._23);
		//float3	vLookTemp = float3(matTemp._31, matTemp._32, matTemp._33);

		//vRight *= length(vRightTemp);
		//vUp *= length(vUpTemp);
		//vLook *= length(vLookTemp);

		//matTemp._11 = vRight.x;
		//matTemp._12 = vRight.y;
		//matTemp._13 = vRight.z;

		//matTemp._21 = vUp.x;
		//matTemp._22 = vUp.y;
		//matTemp._23 = vUp.z;

		//matTemp._31 = vLook.x;
		//matTemp._32 = vLook.y;
		//matTemp._33 = vLook.z;
		}

		matTemp._44 = 1.f;

		matrix matW =
		{
			10.f,0.f,0.f,0.f,
			0.f,10.f,0.f,0.f,
			0.f,0.f,10.f,0.f,
			0.f,0.f,0.f,1.f
		};
		vOut.vPosition = mul(float4(vIn.vPosition, 1.f), matW);
		vOut.vPosition = mul(vOut.vPosition, matTemp);
		vOut.vPosition = mul(vOut.vPosition, matWorld);


		vOut.vPosition = mul(vOut.vPosition, matView);
		vOut.vPosition = mul(vOut.vPosition, matProj);
		vOut.vColor = vIn.vColor;
	}

	else
	{
		vOut.vPosition = mul(float4(vIn.vPosition, 1.f), matWVP);
		vOut.vColor = vIn.vColor;
	}
	return vOut;
}
PS_OUT	PS_Main(VS_OUT vIn)
{
	PS_OUT vOut = (PS_OUT)0;

	vOut.vDiffuseTex = float4(0.f, 0.f, 0.f, 1.f);
	vOut.vDiffuseTex_ = float4(0.f, 0.f, 0.f, 1.f);
	return vOut;
}

