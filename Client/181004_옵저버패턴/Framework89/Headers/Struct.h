#pragma once

typedef struct tagInfo
{
	D3DXVECTOR3		vPos;		// 위치벡터
	D3DXVECTOR3		vDir;		// 방향벡터
	D3DXVECTOR3		vLook;		// 기준벡터
	D3DXVECTOR3		vSize;	
	D3DXMATRIX		matWorld;	// 월드행렬
}INFO;

typedef struct tagStat
{
	int iHp;
	int iMp;
	int iAtt;
	int iDef;
}STAT;

typedef struct tagFrame
{
	float fFrame;
	float fMax;
}FRAME;

typedef struct tagTexInfo
{
	// 이미지 한장을 제어할 Com객체
	LPDIRECT3DTEXTURE9	pTexture;

	// 불러온 이미지의 정보.
	D3DXIMAGE_INFO		tImgInfo;

}TEXINFO;

typedef struct tagTile
{
	D3DXVECTOR3 vPos;
	D3DXVECTOR3 vSize;
	BYTE		byDrawID;
	BYTE		byOption;

	int			iIndex = 0;	// 현재 자신의 인덱스
	int			iParentIndex = 0; // 부모의 인덱스
}TILE;

typedef struct tagUnit
{
#ifndef _AFX
	wstring strName;	
#else
	CString strName;
#endif

	int		iAtt;
	int		iDef;
	BYTE	byJobIndex;
	BYTE	byItem;
}UNITDATA;

typedef struct tagImgPathInfo
{
	wstring wstrObjKey;
	wstring wstrStateKey;
	wstring wstrPath;
	int		iCount;
}IMGPATHINFO;