#pragma once
class CFinder_Tag
{
public:
	CFinder_Tag(const wchar_t* pTag) : m_pTag(pTag) {}
	~CFinder_Tag() = default;
public:
	template <typename T>
	bool operator ()  (T& Pair)
	{
		return !lstrcmp(Pair.first, m_pTag);
	}
private:
	const wchar_t* m_pTag;
};
