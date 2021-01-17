#pragma once

namespace std {
	template <>
	struct hash<DirectX::XMFLOAT2>
	{
		std::size_t operator()(const DirectX::XMFLOAT2& k) const
		{
			using std::size_t;
			using std::hash;

			// Compute individual hash values for first,
			// second and third and combine them using XOR
			// and bit shifting:

			return ((hash<float>()(k.x)
				^ (hash<float>()(k.y) << 1)) >> 1);
		}
	};

	template <>
	struct hash<DirectX::XMFLOAT3>
	{
		std::size_t operator()(const DirectX::XMFLOAT3& k) const
		{
			using std::size_t;
			using std::hash;

			// Compute individual hash values for first,
			// second and third and combine them using XOR
			// and bit shifting:

			return ((hash<float>()(k.x)
				^ (hash<float>()(k.y) << 1)) >> 1)
				^ (hash<float>()(k.z) << 1);
		}
	};

	template <>
	struct hash<MESH>
	{
		std::size_t operator()(const MESH& k) const
		{
			using std::size_t;
			using std::hash;

			// Compute individual hash values for first,
			// second and third and combine them using XOR
			// and bit shifting:

			return ((hash<DirectX::XMFLOAT3>()(k.vPosition)
				^ (hash<DirectX::XMFLOAT3>()(k.vNormal) << 1)) >> 1)
				^ (hash<DirectX::XMFLOAT2>()(k.vUV) << 1);
		}
	};
}