#include "framework.h"
#include "FontMgr.h"
#include "Management.h"
#include "StructedBuffer.h"
#include "Shader.h"
#include "Texture.h"
#include "Transform.h"

_IMPLEMENT_SINGLETON(CFontMgr)
CFontMgr::CFontMgr()
{

}

HRESULT CFontMgr::Ready_FontMgr(const char* pFilePath)
{

	// 문자 사이의 글꼴 크기와 간격을 읽습니다.
	ifstream fin;
	fin.open(pFilePath);
	if (fin.fail())
	{
		return E_FAIL;
	}

	// 텍스트에 사용 된 ASCII 문자 95 개를 읽습니다.
	for (int i = 0; i < 95; i++)
	{
		FontType pFont;
		char temp = 0;
		fin.get(temp);
		while (temp != ' ')
		{
			fin.get(temp);
		}
		fin.get(temp);
		while (temp != ' ')
		{
			fin.get(temp);
		}

		fin >> pFont.left;
		fin >> pFont.right;
		fin >> pFont.size;

		m_vecFont.push_back(pFont);
	}

	// 파일을 닫습니다.
	fin.close();



	m_pShaderCom = CShader::Create(L"../ShaderFiles/Shader_Font.hlsl", "VS_Main", "PS_Main");
	m_pTextureCom = CTexture::Create(L"../Bin/Resource/Texture/Font/Font%d.png", 1, TEXTURE_TYPE::TEXTURE_TYPE_PNG_JPG);
	m_pTransformCom = CTransform::Create();

	if (FAILED(CreateInputLayout()))
		return E_FAIL;

	_vec3 vPos = _vec3(0.f, 10.f, 0.f);
	m_pTransformCom->Set_StateInfo(CTransform::STATE_POSITION, &vPos);
	return S_OK;
}

HRESULT CFontMgr::Create_Buffer(const char* sentence, float drawX, float drawY)
{

	int numLetters = (int)strlen(sentence);

	// 정점 배열에 대한 인덱스를 초기화합니다.
	int index = 0;
	int letter = 0;

	m_vecFontInfo.resize(numLetters);
	// 각 문자를 쿼드 위에 그립니다.
	for (int i = 0; i < numLetters; i++)
	{
		letter = ((int)sentence[i]) - 32;

		// 문자가 공백이면 3 픽셀 위로 이동합니다.
		if (letter == 0)
		{
			drawX = drawX + 3.0f;
		}
		else
		{
			m_iNumVertices = 4;
			m_iStride = sizeof(VTXTEX);
			m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			vector<VTXTEX>		vecVertices;
			vecVertices.resize(m_iNumVertices);

			vecVertices[0].vPosition = XMFLOAT3(drawX, drawY, 0.0f);  // 왼쪽 위
			vecVertices[0].vTex = XMFLOAT2(m_vecFont[letter].left, 0.0f);

			vecVertices[1].vPosition = XMFLOAT3((drawX + m_vecFont[letter].size), (drawY - 16), 0.0f);  // 오른쪽 아래
			vecVertices[1].vTex = XMFLOAT2(m_vecFont[letter].right, 1.0f);

			vecVertices[2].vPosition = XMFLOAT3(drawX, (drawY - 16), 0.0f);  // 왼쪽 아래
			vecVertices[2].vTex = XMFLOAT2(m_vecFont[letter].left, 1.0f);

			vecVertices[3].vPosition = XMFLOAT3(drawX + m_vecFont[letter].size, drawY, 0.0f);  // 오른쪽 위
			vecVertices[3].vTex = XMFLOAT2(m_vecFont[letter].right, 0.0f);


			drawX = drawX + m_vecFont[letter].size + 1.0f;


			m_vecFontInfo[i].iIndices = 6;

			vector<_uint>	vecIndices;
			vecIndices.resize(m_vecFontInfo[i].iIndices);
			vecIndices[0] = 0; vecIndices[1] = 1; vecIndices[2] = 2;
			vecIndices[3] = 0; vecIndices[4] = 2; vecIndices[5] = 3;

			D3D12_HEAP_PROPERTIES tHeap_Pro_Default = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
			D3D12_HEAP_PROPERTIES tHeap_Pro_Upload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
			CDevice::GetInstance()->Open();
			{
				D3D12_RESOURCE_DESC		tResource_Desc = CD3DX12_RESOURCE_DESC::Buffer(m_iStride * m_iNumVertices);



				if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&tHeap_Pro_Default, D3D12_HEAP_FLAG_NONE,
					&tResource_Desc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_pVertexBuffer))))
					return E_FAIL;

				if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&tHeap_Pro_Upload, D3D12_HEAP_FLAG_NONE,
					&tResource_Desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_pVertexUploadBuffer))))
					return E_FAIL;

				D3D12_SUBRESOURCE_DATA vertexData = {};
				vertexData.pData = (void*)(vecVertices.data());
				vertexData.RowPitch = m_iStride * m_iNumVertices;;
				vertexData.SlicePitch = m_iStride * m_iNumVertices;

				UpdateSubresources(CDevice::GetInstance()->GetCmdLst().Get(), m_pVertexBuffer.Get(), m_pVertexUploadBuffer.Get(), 0, 0, 1, &vertexData);
				D3D12_RESOURCE_BARRIER	tResource_Barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_pVertexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
				CDevice::GetInstance()->GetCmdLst()->ResourceBarrier(1, &tResource_Barrier);
			}
			{
				D3D12_RESOURCE_DESC		tResource_Desc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(_uint) * m_vecFontInfo[i].iIndices);


				if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&tHeap_Pro_Default, D3D12_HEAP_FLAG_NONE,
					&tResource_Desc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_pIndexBuffer))))
					return E_FAIL;
				if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&tHeap_Pro_Upload, D3D12_HEAP_FLAG_NONE,
					&tResource_Desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_pIndexUploadBuffer))))
					return E_FAIL;

				D3D12_SUBRESOURCE_DATA indexData = {};
				indexData.pData = (void*)(vecIndices.data());
				indexData.RowPitch = sizeof(_uint) * m_vecFontInfo[i].iIndices;
				indexData.SlicePitch = sizeof(_uint) * m_vecFontInfo[i].iIndices;

				UpdateSubresources(CDevice::GetInstance()->GetCmdLst().Get(), m_pIndexBuffer.Get(), m_pIndexUploadBuffer.Get(), 0, 0, 1, &indexData);
				D3D12_RESOURCE_BARRIER	tResource_Barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_pIndexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER);
				CDevice::GetInstance()->GetCmdLst()->ResourceBarrier(1, &tResource_Barrier);
			}
			CDevice::GetInstance()->Close();

			m_vecFontInfo[i].tVertexBufferView.BufferLocation = m_pVertexBuffer->GetGPUVirtualAddress();
			m_vecFontInfo[i].tVertexBufferView.StrideInBytes = m_iStride;
			m_vecFontInfo[i].tVertexBufferView.SizeInBytes = m_iStride * m_iNumVertices;

			m_vecFontInfo[i].tIndexBufferView.BufferLocation = m_pIndexBuffer->GetGPUVirtualAddress();
			m_vecFontInfo[i].tIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
			m_vecFontInfo[i].tIndexBufferView.SizeInBytes = sizeof(_uint) * m_vecFontInfo[i].iIndices;

			CDevice::GetInstance()->WaitForFenceEvent();
		}
	}



	return S_OK;
}

void CFontMgr::Render_Font()
{
	for (auto& iter : m_vecFontInfo)
	{
		MAINPASS tMainPass = {};
		_matrix matWorld = m_pTransformCom->Get_Matrix();
		_matrix matView = CCamera_Manager::GetInstance()->GetMatView();
		_matrix matProj = CCamera_Manager::GetInstance()->GetMatOrtho();

		m_pShaderCom->SetUp_OnShader(matWorld, matView, matProj, tMainPass);


		_uint iOffeset = CManagement::GetInstance()->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);
		CDevice::GetInstance()->SetConstantBufferToShader(CManagement::GetInstance()->GetConstantBuffer((_uint)CONST_REGISTER::b0)->GetCBV().Get(), iOffeset, CONST_REGISTER::b0);
		CDevice::GetInstance()->SetTextureToShader(m_pTextureCom, TEXTURE_REGISTER::t0);




		CDevice::GetInstance()->UpdateTable();

		CDevice::GetInstance()->GetCmdLst()->IASetPrimitiveTopology(m_PrimitiveTopology);
		CDevice::GetInstance()->GetCmdLst()->IASetVertexBuffers(m_iSlot, 1, &iter.tVertexBufferView);
		CDevice::GetInstance()->GetCmdLst()->IASetIndexBuffer(&iter.tIndexBufferView);
		CDevice::GetInstance()->GetCmdLst()->DrawIndexedInstanced(iter.iIndices, 1, 0, 0, 0);
	}
}

HRESULT CFontMgr::CreateInputLayout()
{
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc = {};
	vector<D3D12_INPUT_ELEMENT_DESC>  vecDesc;
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

	if (FAILED(m_pShaderCom->Create_Shader(vecDesc, RS_TYPE::DEFAULT, DEPTH_STENCIL_TYPE::LESS, SHADER_TYPE::SHADER_FORWARD, BLEND_TYPE::DEFAULT)))
		return E_FAIL;

	return S_OK;
}

CFontMgr* CFontMgr::Create(const char* pFilePath)
{
	CFontMgr* pInstance = new CFontMgr();
	if (FAILED(pInstance->Ready_FontMgr(pFilePath)))
		Safe_Release(pInstance);
	return pInstance;
}

void CFontMgr::Free()
{
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTransformCom);
}
