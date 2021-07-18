#include "framework.h"
#include "Font.h"
#include "Management.h"
#include "Shader.h"
#include "Texture.h"

CFont::CFont()
	: CBase()
{
}


HRESULT CFont::Create_Buffer(const char* pSentence, _float fDrawX, _float fDrawY, vector<FontType> vecFontInfo)
{
	int numLetters = (int)strlen(pSentence);

	// 정점 배열에 대한 인덱스를 초기화합니다.
	int index = 0;
	int letter = 0;

	m_vecFontInfo.resize(numLetters);
	// 각 문자를 쿼드 위에 그립니다.
	for (int i = 0; i < numLetters; i++)
	{
		letter = ((int)pSentence[i]) - 32;

		// 문자가 공백이면 3 픽셀 위로 이동합니다.
		if (letter == 0)
		{
			fDrawX = fDrawX + 3.0f;
		}
		else
		{
			m_iNumVertices = 6;
			m_iStride = sizeof(VTXTEX);
			m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			vector<VTXTEX>		vecVertices;
			vecVertices.resize(m_iNumVertices);

			vecVertices[0].vPosition = XMFLOAT3(fDrawX, fDrawY, 0.0f);  // 왼쪽 위
			vecVertices[0].vTex = XMFLOAT2(vecFontInfo[letter].left, 0.0f);
			
			vecVertices[1].vPosition = XMFLOAT3((fDrawX + vecFontInfo[letter].size), (fDrawY - 16), 0.0f);  // 오른쪽 아래
			vecVertices[1].vTex = XMFLOAT2(vecFontInfo[letter].right, 1.0f);
			
			vecVertices[2].vPosition = XMFLOAT3(fDrawX, (fDrawY - 16), 0.0f);  // 왼쪽 아래
			vecVertices[2].vTex = XMFLOAT2(vecFontInfo[letter].left, 1.0f);
			
			vecVertices[3].vPosition = XMFLOAT3(fDrawX + vecFontInfo[letter].size , fDrawY, 0.0f);  // 오른쪽 위
			vecVertices[3].vTex = XMFLOAT2(vecFontInfo[letter].right, 0.0f);


			fDrawX = fDrawX + vecFontInfo[letter].size + 1.0f;


			m_vecFontInfo[i].iIndices = 6;

			vector<_uint>	vecIndices;
			vecIndices.resize(m_vecFontInfo[i].iIndices);
			vecIndices[0] = 0; vecIndices[1] = 1; vecIndices[2] = 2;
			vecIndices[3] = 0; vecIndices[4] = 3; vecIndices[5] = 1;

			D3D12_HEAP_PROPERTIES tHeap_Pro_Default = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
			D3D12_HEAP_PROPERTIES tHeap_Pro_Upload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
			CDevice::GetInstance()->Open();
			{
				D3D12_RESOURCE_DESC		tResource_Desc = CD3DX12_RESOURCE_DESC::Buffer(m_iStride * m_iNumVertices);



				if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&tHeap_Pro_Default, D3D12_HEAP_FLAG_NONE,
					&tResource_Desc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_vecFontInfo[i].pVertexBuffer))))
					return E_FAIL;

				if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&tHeap_Pro_Upload, D3D12_HEAP_FLAG_NONE,
					&tResource_Desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_vecFontInfo[i].pVertexUploadBuffer))))
					return E_FAIL;

				D3D12_SUBRESOURCE_DATA vertexData = {};
				vertexData.pData = (void*)(vecVertices.data());
				vertexData.RowPitch = m_iStride * m_iNumVertices;;
				vertexData.SlicePitch = m_iStride * m_iNumVertices;

				UpdateSubresources(CDevice::GetInstance()->GetCmdLst().Get(), m_vecFontInfo[i].pVertexBuffer.Get(), m_vecFontInfo[i].pVertexUploadBuffer.Get(), 0, 0, 1, &vertexData);
				D3D12_RESOURCE_BARRIER	tResource_Barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_vecFontInfo[i].pVertexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
				CDevice::GetInstance()->GetCmdLst()->ResourceBarrier(1, &tResource_Barrier);
			}
			{
				D3D12_RESOURCE_DESC		tResource_Desc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(_uint) * m_vecFontInfo[i].iIndices);


				if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&tHeap_Pro_Default, D3D12_HEAP_FLAG_NONE,
					&tResource_Desc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_vecFontInfo[i].pIndexBuffer))))
					return E_FAIL;
				if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&tHeap_Pro_Upload, D3D12_HEAP_FLAG_NONE,
					&tResource_Desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_vecFontInfo[i].pIndexUploadBuffer))))
					return E_FAIL;

				D3D12_SUBRESOURCE_DATA indexData = {};
				indexData.pData = (void*)(vecIndices.data());
				indexData.RowPitch = sizeof(_uint) * m_vecFontInfo[i].iIndices;
				indexData.SlicePitch = sizeof(_uint) * m_vecFontInfo[i].iIndices;

				UpdateSubresources(CDevice::GetInstance()->GetCmdLst().Get(), m_vecFontInfo[i].pIndexBuffer.Get(), m_vecFontInfo[i].pIndexUploadBuffer.Get(), 0, 0, 1, &indexData);
				D3D12_RESOURCE_BARRIER	tResource_Barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_vecFontInfo[i].pIndexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER);
				CDevice::GetInstance()->GetCmdLst()->ResourceBarrier(1, &tResource_Barrier);
			}
			CDevice::GetInstance()->Close();

			m_vecFontInfo[i].tVertexBufferView.BufferLocation = m_vecFontInfo[i].pVertexBuffer->GetGPUVirtualAddress();
			m_vecFontInfo[i].tVertexBufferView.StrideInBytes = m_iStride;
			m_vecFontInfo[i].tVertexBufferView.SizeInBytes = m_iStride * m_iNumVertices;

			m_vecFontInfo[i].tIndexBufferView.BufferLocation = m_vecFontInfo[i].pIndexBuffer->GetGPUVirtualAddress();
			m_vecFontInfo[i].tIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
			m_vecFontInfo[i].tIndexBufferView.SizeInBytes = sizeof(_uint) * m_vecFontInfo[i].iIndices;

			CDevice::GetInstance()->WaitForFenceEvent();
		}
	}
	return S_OK;
}

void CFont::Render_Font(CShader* pShader, CTexture* pTexture)
{
	for (auto& iter : m_vecFontInfo)
	{
		_float m_fX = (_float)WINCX/2;
		_float m_fY = (_float)WINCY / 2;
		_float m_fSizeX = 1.5f;
		_float m_fSizeY = 1.5f;

		MAINPASS tMainPass = {};
		_matrix matWorld = Matrix_::Identity();
		_matrix matView = Matrix_::Identity();
		_matrix matProj = CCamera_Manager::GetInstance()->GetMatOrtho();
		matWorld._11 = m_fSizeX;
		matWorld._22 = m_fSizeY;
		//
		//matWorld._41 = m_fX - (WINCX >> 1);
		//matWorld._42 = -m_fY + (WINCY >> 1);


		pShader->SetUp_OnShader(matWorld, matView, matProj, tMainPass);


		_uint iOffeset = CManagement::GetInstance()->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);
		CDevice::GetInstance()->SetConstantBufferToShader(CManagement::GetInstance()->GetConstantBuffer((_uint)CONST_REGISTER::b0)->GetCBV().Get(), iOffeset, CONST_REGISTER::b0);
		CDevice::GetInstance()->SetTextureToShader(pTexture, TEXTURE_REGISTER::t0);




		CDevice::GetInstance()->UpdateTable();

		CDevice::GetInstance()->GetCmdLst()->IASetPrimitiveTopology(m_PrimitiveTopology);
		CDevice::GetInstance()->GetCmdLst()->IASetVertexBuffers(m_iSlot, 1, &iter.tVertexBufferView);
		CDevice::GetInstance()->GetCmdLst()->IASetIndexBuffer(&iter.tIndexBufferView);
		CDevice::GetInstance()->GetCmdLst()->DrawIndexedInstanced(iter.iIndices, 1, 0, 0, 0);
	}
}

CFont* CFont::Create(const char* pSentence, _float fDrawX, _float fDrawY, vector<FontType> vecFontInfo)
{
	CFont* pInstance = new CFont();

	if (FAILED(pInstance->Create_Buffer(pSentence, fDrawX, fDrawY, vecFontInfo)))
	{
		MessageBox(0, L"CFont CFont Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFont::Free()
{

}
