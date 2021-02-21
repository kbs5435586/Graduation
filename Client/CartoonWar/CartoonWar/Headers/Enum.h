#pragma once

enum class SCENEID { SCENE_STATIC, SCENE_LOGO, SCENE_STAGE, SCENE_END };
enum class TEXTURE_TYPE { TEXTURE_TYPE_DDS, TEXTURE_TYPE_PNG_JPG, TEXTURE_TGA, TEXTURE_TYPE_END };

enum class ROOT_TYPE { ROOT_TYPE_COLOR = 0, ROOT_TYPE_TEXTURE = 1, ROOT_TYPE_MULTI_TEXTURE, ROOT_TYPE_END };

enum class LIGHT_TYPE { LIGHT_DIRECTIONAL = 0, LIGHT_POINT = 1 , LIGHT_SPOT = 2,  LIGHT_END };


enum class ROOT_SIG_TYPE
{
	RENDER,
	COMPUTE,
	END,
};

enum class RS_TYPE
{
	DEFAULT,
	SKYBOX,
	WIREFRAME,
	COUNTERCLOCK,
	END,
};
enum class BLEND_TYPE
{
	DEFAULT,
	END,
};


enum class SwapChainBitDepth
{
	_8 = 0,
	_10,
	_16,
	SwapChainBitDepthCount
};


enum class RootConstants
{
	ReferenceWhiteNits = 0,
	DisplayCurve,
	RootConstantsCount
};


enum class DisplayCurve
{
	sRGB = 0,    // The display expects an sRGB signal.
	ST2084,        // The display expects an HDR10 signal.
	None        // The display expects a linear signal.
};


struct DisplayChromaticities
{
	float RedX;
	float RedY;
	float GreenX;
	float GreenY;
	float BlueX;
	float BlueY;
	float WhiteX;
	float WhiteY;
};

enum class CONST_REGISTER
{
	b0 = 0,
	b1 = 1,
	b2 = 2,
	b3 = 3,
	b4 = 4,

	END,
};

enum class TEXTURE_REGISTER
{
	t0 = (UINT)CONST_REGISTER::END,
	t1,
	t2,
	t3,
	t4,
	t5,
	t6,
	t7,
	t8,
	t9,
	t10,
	t11,
	t12,
	END,
};




enum class POINT_ { POINT_A, POINT_B, POINT_C, POINT_END };
enum class NEIGHBOR { NEIGHBOR_AB, NEIGHBOR_BC, NEIGHBOR_CA, NEIGHBOR_END };
enum class LINE { LINE_AB, LINE_BC, LINE_CA, LINE_END };


enum class MRT {MRT_SWAPCHAIN, MRT_DEFFERD, MRT_LIGHT, MRT_END};


enum class SHADER_TYPE {SHADER_FORWARD ,SHADER_DEFFERED, SHADER_POST_EFFECT, SHADER_LIGHT, SHADER_COMPUTR, SHADER_END};

enum class DEPTH_STENCIL_TYPE
{
	LESS,
	LESS_EQUAL,
	GRATER,
	GRATER_EQUAL,

	NO_DEPTHTEST,	//±Ì¿Ã ≈◊Ω∫∆Æ X, ±Ì¿Ã±‚∑œ o
	NO_DEPTHTEST_NO_WRITE, // ±Ì¿Ã≈◊Ω∫∆Æ X, ±Ì¿Ã±‚∑œ X
	LESS_NO_WRITE,	// Less ±Ì¿Ã≈◊Ω∫∆Æ, ±Ì¿Ã±‚∑œ X
	END,
};