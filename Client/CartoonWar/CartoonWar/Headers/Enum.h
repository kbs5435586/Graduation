#pragma once

enum class SCENEID { SCENE_STATIC, SCENE_LOGO, SCENE_STAGE, SCENE_END };
enum class TEXTURE_TYPE { TEXTURE_TYPE_DDS, TEXTURE_TYPE_PNG_JPG, TEXTURE_TGA, TEXTURE_TYPE_END };

enum class ROOT_TYPE { ROOT_TYPE_COLOR = 0, ROOT_TYPE_TEXTURE = 1, ROOT_TYPE_MULTI_TEXTURE, ROOT_TYPE_END };

enum class LIGHT_TYPE { LIGHT_DIRECTIONAL = 0, LIGHT_POINT = 1, LIGHT_SPOT = 2, LIGHT_END };
enum class DATA_TYPE { DATA_INFO, DATA_MATRIX, DATA_END };

enum class TEAM { TEAM_RED, TEAM_BLUE, TEAM_C, TEAM_END };

enum class ROTATE_DIR { ROT_DIR_LEFT, ROT_DIR_RIGHT, ROT_DIR_END };

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
	ALPHABLEND,
	ONEBLEND,
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
	b5 = 5,
	b6 = 6,
	b7 = 7,
	b8 = 8,
	b9 = 9,
	b10 = 10,

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

enum class UAV_REGISTER
{
	u0 = (UINT)TEXTURE_REGISTER::END,
	u1,
	u2,
	u3,
	END,
};




enum class POINT_ { POINT_A, POINT_B, POINT_C, POINT_END };
enum class NEIGHBOR { NEIGHBOR_AB, NEIGHBOR_BC, NEIGHBOR_CA, NEIGHBOR_END };
enum class LINE { LINE_AB, LINE_BC, LINE_CA, LINE_END };


enum class MRT { MRT_SWAPCHAIN, MRT_DEFFERD, MRT_LIGHT, MRT_SHADOW, MRT_END };


enum class SHADER_TYPE { SHADER_FORWARD, SHADER_DEFFERED, SHADER_PARTICLE, SHADER_POST_EFFECT, SHADER_LIGHT, SHADER_SHADOW, SHADER_COMPUTE, SHADER_END };

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



enum class COLLIDER_TYPE
{
	COLLIDER_AABB,
	COLLIDER_OBB,
	COLLIDER_SPHERE,
	COLLIDER_END
};


enum class SHADER_PARAM
{
	INT_0,
	INT_1,
	INT_2,
	INT_3,
	INT_END,

	FLOAT_0,
	FLOAT_1,
	FLOAT_2,
	FLOAT_3,
	FLOAT_END,

	VEC2_0,
	VEC2_1,
	VEC2_2,
	VEC2_3,
	VEC2_END,

	VEC4_0,
	VEC4_1,
	VEC4_2,
	VEC4_3,
	VEC4_END,

	MATRIX_0,
	MATRIX_1,
	MATRIX_2,
	MATRIX_3,
	MATRIX_END,

	TEX_0,
	TEX_1,
	TEX_2,
	TEX_3,
	TEX_END,
};



enum class SPECIES { SPECIES_HUMAN, SPECIES_UNDEAD, SPECIES_END };
enum class CLASS { CLASS_WORKER, CLASS_CAVALRY, CLASS_INFANTRY=3, CLASS_SPEARMAN=5, CLASS_MAGE=6, CLASS_MMAGE=7, CLASS_ARCHER=8, CLASS_PRIEST=9, CLASS_END=10 };
enum class COLOR { COLOR_WHITE, COLOR_TAN, COLOR_RED, COLOR_PURPLE, COLOR_GREEN, COLOR_BROWN, COLOR_BLUE, COLOR_BLACK, COLOR_END };
enum class HORSE { HORSE_A, HORSE_B, HORSE_C, HORSE_D, HORSE_END };