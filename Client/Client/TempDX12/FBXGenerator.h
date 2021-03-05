#pragma once
#include "Skinned_Data.h"
class CGameObject;
class CFBXGenerator
{

private:
	ID3D12Device* mDevice;
	ID3D12GraphicsCommandList* mCommandList;
	ID3D12DescriptorHeap* mCbvHeap;

	bool mInBeginEndPair;
};

