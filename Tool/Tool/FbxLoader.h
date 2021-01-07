#pragma once
#include "Base.h"
class CFbxLoader
{
private:
	CFbxLoader(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CFbxLoader() = default;
public:
	HRESULT					Ready_FbxLoader(string strFilePath, FbxScene*& pScene);
	HRESULT					Ready_LoadFbx(FbxNode* pNode);

};

