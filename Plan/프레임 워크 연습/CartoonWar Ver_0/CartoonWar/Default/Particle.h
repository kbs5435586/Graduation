#pragma once
#include "GameObject.h"

class CParticle
	:public CGameObject
{
private:
	CParticle();
	~CParticle();

public:
	bool Init();
	void Release();
	bool Frame();
	void Render();

private:
	bool Initialize();

private:
	struct ParticleType 
	{ 
		float positionX, positionY, positionZ; 
		float red, green, blue; 
		float velocity; 
		bool active; 
	};
	struct VertexType 
	{ 
		Vector3 position;
		Vector2 Texture;
		Vector4 Color;
	};

private:
	//편차
	Vector3 m_Deviation;
	float m_ParticleDeviationX, m_ParticleDeviationY, m_ParticleDeviationZ;
	//속도
	float m_ParticleVelocity;
	float m_ParticleVelocityVariation;
	//사이즈
	float m_ParticleSize;
	float m_ParticlePerSec;

	int m_MaxParticle;

	//누적 갯수 및 시간
	int m_CurrentParticleCount;
	float m_AccumulatedTime;

	//텍스쳐 정보 
	CTexture* m_pTextureCom = nullptr;
	vector<CTexture*> vParticleList;
	//texetureClass* m_texture;
	//ParticleType* m_ParticleList;

	CBuffer_RectCol* m_pBufferCom = nullptr;

	int m_VertexCnt;
	int m_IdxCnt;

	VertexType* m_vertices;

	ID3D12Resource* m_VertexBuffer;
	ID3D12Resource* m_IdxBuffer;

};

