#include "framework.h"
#include "FBX_Loader.h"

FbxManager* CFBX_Loader::m_pFbxManager = nullptr;
CFBX_Loader::CFBX_Loader()
{
}

CFBX_Loader::~CFBX_Loader()
{
}

HRESULT CFBX_Loader::LoadFBX(string strFileName, vector<Vertex>& outVertexVector, vector<uint32_t>& outIndexVector, vector<Material>& outMaterial)
{
	if (FAILED(Load_Mesh(strFileName, outVertexVector, outIndexVector, &outMaterial)))
		return E_FAIL;
	if (FAILED(Load_Mesh(strFileName, outVertexVector, outIndexVector)))
		return E_FAIL;

	
	if (m_pFbxManager == nullptr)
	{
		m_pFbxManager = FbxManager::Create();

		FbxIOSettings* pIOsettings = FbxIOSettings::Create(m_pFbxManager, IOSROOT);
		m_pFbxManager->SetIOSettings(pIOsettings);
	}

	FbxImporter* pImporter = FbxImporter::Create(m_pFbxManager, "");
	string fbxFileName = strFileName + ".fbx";
	bool bSuccess = pImporter->Initialize(fbxFileName.c_str(), -1, m_pFbxManager->GetIOSettings());
	if (!bSuccess) return E_FAIL;

	FbxScene* pFbxScene = FbxScene::Create(m_pFbxManager, "");
	bSuccess = pImporter->Import(pFbxScene);
	if (!bSuccess) return E_FAIL;

	pImporter->Destroy();

	FbxAxisSystem sceneAxisSystem = pFbxScene->GetGlobalSettings().GetAxisSystem();
	FbxAxisSystem::MayaZUp.ConvertScene(pFbxScene); // Delete?

													// Convert quad to triangle
	FbxGeometryConverter geometryConverter(m_pFbxManager);
	geometryConverter.Triangulate(pFbxScene, true);

	FbxNode* pFbxRootNode = pFbxScene->GetRootNode();
	if (pFbxRootNode)
	{
		// Bone offset, Control point, Vertex, Index Data
		int ii = pFbxRootNode->GetChildCount();
		for (int i = 0; i < pFbxRootNode->GetChildCount(); i++)
		{
			FbxNode* pFbxChildNode = pFbxRootNode->GetChild(i);
			FbxMesh* pMesh = (FbxMesh*)pFbxChildNode->GetNodeAttribute();
			if (!pMesh) { continue; }
			FbxNodeAttribute::EType AttributeType = pMesh->GetAttributeType();
			if (!AttributeType) { continue; }

			if (AttributeType == FbxNodeAttribute::eMesh)
			{
				GetControlPoints(pFbxChildNode);

				// Get Vertices and indices info
				GetVerticesAndIndice(pMesh, outVertexVector, outIndexVector);

				GetMaterials(pFbxChildNode, outMaterial);

				break;
			}
		}
	}

	ExportMesh(strFileName, outVertexVector, outIndexVector, outMaterial);


	return S_OK;
}

HRESULT CFBX_Loader::Load_Mesh(string strFileName, vector<Vertex>& outVertexVector, vector<uint32_t>& outIdxVector, vector<Material>* outMertarial)
{
	strFileName = strFileName + ".mesh";
	ifstream Open(strFileName);

	uint32_t	iVertexSize = 0;
	uint32_t	iInxSize = 0;
	uint32_t	iMatSize = 0;

	string		strIgnore;

	if (Open)
	{
		Open >> strIgnore >> iVertexSize;
		Open >> strIgnore >> iInxSize;
		Open >> strIgnore >> iMatSize;

		if (iVertexSize == 0 || iInxSize == 0)
			return E_FAIL;

		Open >> strIgnore;
		for (uint32_t i = 0; i < iMatSize; ++i)
		{
			Material tempMaterial;

			Open >> strIgnore >> tempMaterial.Name;
			Open >> strIgnore >> tempMaterial.Ambient.x >> tempMaterial.Ambient.y >> tempMaterial.Ambient.z;
			Open >> strIgnore >> tempMaterial.DiffuseAlbedo.x >> tempMaterial.DiffuseAlbedo.y >> tempMaterial.DiffuseAlbedo.z >> tempMaterial.DiffuseAlbedo.w;
			Open >> strIgnore >> tempMaterial.FresnelR0.x >> tempMaterial.FresnelR0.y >> tempMaterial.FresnelR0.z;
			Open >> strIgnore >> tempMaterial.Specular.x >> tempMaterial.Specular.y >> tempMaterial.Specular.z;
			Open >> strIgnore >> tempMaterial.Emissive.x >> tempMaterial.Emissive.y >> tempMaterial.Emissive.z;
			Open >> strIgnore >> tempMaterial.Roughness;
			Open >> strIgnore;
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					Open >> tempMaterial.MatTransform.m[i][j];
				}
			}
			(*outMertarial).push_back(tempMaterial);
		}


		for (uint32_t i = 0; i < iVertexSize; ++i)
		{
			Vertex vertex;
			Open >> strIgnore >> vertex.Pos.x >> vertex.Pos.y >> vertex.Pos.z;
			Open >> strIgnore >> vertex.Normal.x >> vertex.Normal.y >> vertex.Normal.z;
			Open >> strIgnore >> vertex.TexC.x >> vertex.TexC.y;

			// push_back
			outVertexVector.push_back(vertex);
		}

		Open >> strIgnore;
		for (uint32_t i = 0; i < iInxSize; ++i)
		{
			uint32_t index;
			Open >> index;
			outIdxVector.push_back(index);
		}


		return S_OK;
	}
	return E_FAIL;
}

void CFBX_Loader::GetControlPoints(FbxNode* pFbxRootNode)
{
	FbxMesh* pCurrMesh = (FbxMesh*)pFbxRootNode->GetNodeAttribute();

	unsigned int ctrlPointCount = pCurrMesh->GetControlPointsCount();
	for (unsigned int i = 0; i < ctrlPointCount; ++i)
	{
		CtrlPoint* currCtrlPoint = new CtrlPoint();

		DirectX::XMFLOAT3 currPosition;
		currPosition.x = static_cast<float>(pCurrMesh->GetControlPointAt(i).mData[0]);
		currPosition.y = static_cast<float>(pCurrMesh->GetControlPointAt(i).mData[1]);
		currPosition.z = static_cast<float>(pCurrMesh->GetControlPointAt(i).mData[2]);

		currCtrlPoint->mPosition = currPosition;
		mControlPoints[i] = currCtrlPoint;
	}
}

void CFBX_Loader::GetVerticesAndIndice(FbxMesh* pMesh, vector<Vertex>& outVertexVector, vector<uint32_t>& outIndexVector)
{	// Vertex and Index
	map<Vertex, uint32_t> IndexMapping;
	map<string, int>		mapTemp;

	uint32_t VertexIndex = 0;
	int tCount = pMesh->GetPolygonCount(); // Triangle

	for (int i = 0; i < tCount; ++i)
	{
		// Vertex and Index info
		for (int j = 0; j < 3; ++j)
		{
			int controlPointIndex = pMesh->GetPolygonVertex(i, j);
			CtrlPoint* CurrCtrlPoint = mControlPoints[controlPointIndex];

			// Normal
			FbxVector4 pNormal;
			pMesh->GetPolygonVertexNormal(i, j, pNormal);

			// UV
			float* lUVs = NULL;
			FbxStringList lUVNames;
			pMesh->GetUVSetNames(lUVNames);
			const char* lUVName = NULL;
			if (lUVNames.GetCount())
			{
				lUVName = lUVNames[0];
			}

			FbxVector2 pUVs;
			bool bUnMappedUV;
			if (!pMesh->GetPolygonVertexUV(i, j, lUVName, pUVs, bUnMappedUV))
			{
				MessageBox(0, L"UV not found", 0, 0);
			}

			Vertex Temp;
			// Position
			Temp.Pos.x = CurrCtrlPoint->mPosition.x;
			Temp.Pos.y = CurrCtrlPoint->mPosition.y;
			Temp.Pos.z = CurrCtrlPoint->mPosition.z;

			// Normal
			Temp.Normal.x = pNormal.mData[0];
			Temp.Normal.y = pNormal.mData[1];
			Temp.Normal.z = pNormal.mData[2];

			// UV
			Temp.TexC.x = pUVs.mData[0];
			Temp.TexC.y = 1.0f - pUVs.mData[1];

			// push vertex and index
			auto lookup = IndexMapping.find(Temp);
			if (lookup != IndexMapping.end())
			{
				// Index
				outIndexVector.push_back(lookup->second);
			}
			else
			{
				// Index
				outIndexVector.push_back(VertexIndex);
				IndexMapping[Temp] = VertexIndex;

				VertexIndex++;
				outVertexVector.push_back(Temp);
			}

		}
	}
}

void CFBX_Loader::GetMaterials(fbxsdk::FbxNode* pNode, std::vector<Material>& outMaterial)
{
	int MaterialCount = pNode->GetMaterialCount();

	for (int i = 0; i < MaterialCount; ++i)
	{
		Material tempMaterial;
		FbxSurfaceMaterial* SurfaceMaterial = pNode->GetMaterial(i);
		GetMaterialAttribute(SurfaceMaterial, tempMaterial);
		GetMaterialTexture(SurfaceMaterial, tempMaterial);

		if (tempMaterial.Name != "")
		{
			outMaterial.push_back(tempMaterial);
		}
	}
}

void CFBX_Loader::GetMaterialAttribute(FbxSurfaceMaterial* pMaterial, Material& outMaterial)
{
	FbxDouble3 double3;
	FbxDouble double1;
	if (pMaterial->GetClassId().Is(FbxSurfacePhong::ClassId))
	{
		// Amibent Color
		double3 = reinterpret_cast<FbxSurfacePhong*>(pMaterial)->Ambient;
		outMaterial.Ambient.x = static_cast<float>(double3.mData[0]);
		outMaterial.Ambient.y = static_cast<float>(double3.mData[1]);
		outMaterial.Ambient.z = static_cast<float>(double3.mData[2]);

		// Diffuse Color
		double3 = reinterpret_cast<FbxSurfacePhong*>(pMaterial)->Diffuse;
		outMaterial.DiffuseAlbedo.x = static_cast<float>(double3.mData[0]);
		outMaterial.DiffuseAlbedo.y = static_cast<float>(double3.mData[1]);
		outMaterial.DiffuseAlbedo.z = static_cast<float>(double3.mData[2]);

		// Roughness 
		double1 = reinterpret_cast<FbxSurfacePhong*>(pMaterial)->Shininess;
		outMaterial.Roughness = 1 - double1;

		// Reflection
		double3 = reinterpret_cast<FbxSurfacePhong*>(pMaterial)->Reflection;
		outMaterial.FresnelR0.x = static_cast<float>(double3.mData[0]);
		outMaterial.FresnelR0.y = static_cast<float>(double3.mData[1]);
		outMaterial.FresnelR0.z = static_cast<float>(double3.mData[2]);

		// Specular Color
		double3 = reinterpret_cast<FbxSurfacePhong*>(pMaterial)->Specular;
		outMaterial.Specular.x = static_cast<float>(double3.mData[0]);
		outMaterial.Specular.y = static_cast<float>(double3.mData[1]);
		outMaterial.Specular.z = static_cast<float>(double3.mData[2]);

		// Emissive Color
		double3 = reinterpret_cast<FbxSurfacePhong*>(pMaterial)->Emissive;
		outMaterial.Emissive.x = static_cast<float>(double3.mData[0]);
		outMaterial.Emissive.y = static_cast<float>(double3.mData[1]);
		outMaterial.Emissive.z = static_cast<float>(double3.mData[2]);

	}
	else if (pMaterial->GetClassId().Is(FbxSurfaceLambert::ClassId))
	{
		// Amibent Color
		double3 = reinterpret_cast<FbxSurfaceLambert*>(pMaterial)->Ambient;
		outMaterial.Ambient.x = static_cast<float>(double3.mData[0]);
		outMaterial.Ambient.y = static_cast<float>(double3.mData[1]);
		outMaterial.Ambient.z = static_cast<float>(double3.mData[2]);

		// Diffuse Color
		double3 = reinterpret_cast<FbxSurfaceLambert*>(pMaterial)->Diffuse;
		outMaterial.DiffuseAlbedo.x = static_cast<float>(double3.mData[0]);
		outMaterial.DiffuseAlbedo.y = static_cast<float>(double3.mData[1]);
		outMaterial.DiffuseAlbedo.z = static_cast<float>(double3.mData[2]);

		// Emissive Color
		double3 = reinterpret_cast<FbxSurfaceLambert*>(pMaterial)->Emissive;
		outMaterial.Emissive.x = static_cast<float>(double3.mData[0]);
		outMaterial.Emissive.y = static_cast<float>(double3.mData[1]);
		outMaterial.Emissive.z = static_cast<float>(double3.mData[2]);
	}
}

void CFBX_Loader::GetMaterialTexture(FbxSurfaceMaterial* pMaterial, Material& Mat)
{
	unsigned int textureIndex = 0;
	FbxProperty property;

	FBXSDK_FOR_EACH_TEXTURE(textureIndex)
	{
		property = pMaterial->FindProperty(FbxLayerElement::sTextureChannelNames[textureIndex]);
		if (property.IsValid())
		{
			unsigned int textureCount = property.GetSrcObjectCount<FbxTexture>();
			for (unsigned int i = 0; i < textureCount; ++i)
			{
				FbxLayeredTexture* layeredTexture = property.GetSrcObject<FbxLayeredTexture>(i);
				if (layeredTexture)
				{
					throw std::exception("Layered Texture is currently unsupported\n");
				}
				else
				{
					FbxTexture* texture = property.GetSrcObject<FbxTexture>(i);
					if (texture)
					{
						std::string textureType = property.GetNameAsCStr();
						FbxFileTexture* fileTexture = FbxCast<FbxFileTexture>(texture);

						if (fileTexture)
						{
							if (textureType == "DiffuseColor")
							{
								Mat.Name = fileTexture->GetFileName();
							}

						}
					}
				}
			}
		}
	}
}

void CFBX_Loader::ExportMesh(string fileName, vector<Vertex>& outVertexVector, vector<uint32_t>& outIndexVector, vector<Material>& outMaterial)
{
	std::ofstream fileOut(fileName + ".mesh");

	if (outVertexVector.empty() || outIndexVector.empty())
		return;

	if (fileOut)
	{
		uint32_t vertexSize = outVertexVector.size();
		uint32_t indexSize = outIndexVector.size();
		uint32_t materialSize = outMaterial.size();

		fileOut << "VertexSize " << vertexSize << "\n";
		fileOut << "IndexSize " << indexSize << "\n";
		fileOut << "MaterialSize " << materialSize << "\n";

		fileOut << "Material " << "\n";
		for (auto& e : outMaterial)
		{
			fileOut << "Name " << e.Name << "\n";
			fileOut << "Ambient " << e.Ambient.x << " " << e.Ambient.y << " " << e.Ambient.z << "\n";
			fileOut << "Diffuse " << e.DiffuseAlbedo.x << " " << e.DiffuseAlbedo.y << " " << e.DiffuseAlbedo.z << " " << e.DiffuseAlbedo.w << "\n";
			fileOut << "Fresnel " << e.FresnelR0.x << " " << e.FresnelR0.y << " " << e.FresnelR0.z << "\n";
			fileOut << "Specular " << e.Specular.x << " " << e.Specular.y << " " << e.Specular.z << "\n";
			fileOut << "Emissive " << e.Emissive.x << " " << e.Emissive.y << " " << e.Emissive.z << "\n";
			fileOut << "Roughness " << e.Roughness << "\n";
			fileOut << "MatTransform ";
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					fileOut << e.MatTransform.m[i][j] << " ";
				}
			}
			fileOut << "\n";
		}

		for (auto& e : outVertexVector)
		{
			fileOut << "Pos " << e.Pos.x << " " << e.Pos.y << " " << e.Pos.z << "\n";
			fileOut << "Normal " << e.Normal.x << " " << e.Normal.y << " " << e.Normal.z << "\n";
			fileOut << "TexC " << e.TexC.x << " " << e.TexC.y << "\n";
		}

		fileOut << "Indices " << "\n";
		for (uint32_t i = 0; i < indexSize / 3; ++i)
		{
			fileOut << outIndexVector[3 * i] << " " << outIndexVector[3 * i + 1] << " " << outIndexVector[3 * i + 2] << "\n";
		}
	}
}
