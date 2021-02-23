#pragma once

typedef struct tagInfo
{
	D3DXVECTOR3		vPos;		// ��ġ����
	D3DXVECTOR3		vDir;		// ���⺤��
	D3DXVECTOR3		vLook;		// ���غ���
	D3DXVECTOR3		vSize;	
	D3DXMATRIX		matWorld;	// �������
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
	// �̹��� ������ ������ Com��ü
	LPDIRECT3DTEXTURE9	pTexture;

	// �ҷ��� �̹����� ����.
	D3DXIMAGE_INFO		tImgInfo;

}TEXINFO;

typedef struct tagTile
{
	D3DXVECTOR3 vPos;
	D3DXVECTOR3 vSize;
	BYTE		byDrawID;
	BYTE		byOption;

	int			iIndex = 0;	// ���� �ڽ��� �ε���
	int			iParentIndex = 0; // �θ��� �ε���
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