#include "stdafx.h"
#include "FileInfo.h"


CFileInfo::CFileInfo()
{
}


CFileInfo::~CFileInfo()
{
}

CString CFileInfo::ConvertRelativePath(CString strFullPath)
{
	TCHAR szRelativePath[MAX_STR] = L"";
	TCHAR szCurDirectory[MAX_STR] = L"";

	GetCurrentDirectory(MAX_STR, szCurDirectory);

	// PathRelativePathTo: ������ -> ����η� ��ȯ.
	// szCurDirectory�������� strFullPath���� ã�ư��� ����θ� szRelativePath�� ����.
	PathRelativePathTo(szRelativePath, szCurDirectory, FILE_ATTRIBUTE_DIRECTORY,
		strFullPath.GetString(), FILE_ATTRIBUTE_DIRECTORY);

	return CString(szRelativePath);
}

void CFileInfo::DirInfoExtraction(const wstring& wstrPath, list<IMGPATHINFO*>& rImgInfoLst)
{
	// CFileFind: MFC���� �����ϴ� ���� �� ��� ���� ���� Ŭ����.
	CFileFind find;

	// wstrFilePath = D:\������\4������\88��\Framework88\Texture\Stage\EFFECT\*.*
	wstring wstrFilePath = wstrPath + L"\\*.*";

	// CFileFind::FindFile�Լ�: �ش� ��ηκ��� Ž���� ���� ����.
	find.FindFile(wstrFilePath.c_str());

	int iContinue = 1;

	while (iContinue)
	{
		// FindNextFile: ���� ������ ã�´�. ���̻� ã���� ���ٸ� 0�� ��ȯ.
		iContinue = find.FindNextFile();

		if (find.IsDots())	// ã���� .�̳� ..���� ��Ŀ��� �ǳʶ�
			continue;
		else if (find.IsDirectory()) // ã���� ��ζ�� ���� ������ ã�� ������ ���
		{
			// find.GetFilePath() = D:\������\4������\88��\Framework88\Texture\Stage\EFFECT\BossMultiAttack
			DirInfoExtraction(wstring(find.GetFilePath()), rImgInfoLst);
		}
		else // ������ ã���� ��
		{
			if (find.IsSystem())	// ���� �����ִ� �ý��� �����̶�� �ǳʶ�.
				continue;

			// find.GetFilePath() = D:\������\4������\88��\Framework88\Texture\Stage\
			//							EFFECT\BossMultiAttack\BossMultiAttack0.png
			IMGPATHINFO* pImgPathInfo = new IMGPATHINFO;

			TCHAR szPathInfo[MAX_STR] = L"";
			lstrcpy(szPathInfo, find.GetFilePath().GetString());

			// szPathInfo = D:\������\4������\88��\Framework88\Texture\Stage\EFFECT\BossMultiAttack
			PathRemoveFileSpec(szPathInfo);

			pImgPathInfo->iCount = CFileInfo::DirFileCount(szPathInfo);

			///////////////////////////////////////////////////////////////////////////////

			// wstrFileName = L"BossMultiAttack"
			wstring wstrFileName = find.GetFileTitle();

			// wstring::substr(����, count): ���ۺ��� count��ŭ �ش��ϴ� ���ڿ��� ����.
			// wstrFileName = L"BossMultiAttack%d.png"
			wstrFileName = wstrFileName.substr(0, wstrFileName.length() - 1) + L"%d.png";

			TCHAR szRelativeFullPath[MAX_STR] = L"";			
			
			// PathCombine(dst, src1, src2): src1�� src2�� ������ ��θ� ����� dst�� ����.
			// �� �� src1�� src2 ���̿� \�� �ڵ����� ���Եȴ�.
			// szRelativeFullPath = D:\������\4������\88��\Framework88\Texture\Stage\EFFECT\BossMultiAttack\BossMultiAttack%d.png
			PathCombine(szRelativeFullPath, szPathInfo, wstrFileName.c_str());

			pImgPathInfo->wstrPath = CFileInfo::ConvertRelativePath(szRelativeFullPath);

			///////////////////////////////////////////////////////////////////////////////

			// szPathInfo = D:\������\4������\88��\Framework88\Texture\Stage\EFFECT\BossMultiAttack

			// PathFindFileName: ���ϸ��� ã�ų� ���� �������� ã�Ƴ�.
			pImgPathInfo->wstrStateKey = PathFindFileName(szPathInfo);


			///////////////////////////////////////////////////////////////////////////////

			// szPathInfo = D:\������\4������\88��\Framework88\Texture\Stage\EFFECT
			PathRemoveFileSpec(szPathInfo);

			pImgPathInfo->wstrObjKey = PathFindFileName(szPathInfo);

			rImgInfoLst.push_back(pImgPathInfo);
			break;
		}			
	}

}

int CFileInfo::DirFileCount(const wstring & wstrPath)
{
	CFileFind find;
	wstring wstrCurPath = wstrPath + L"\\*.*";

	find.FindFile(wstrCurPath.c_str());

	int iCount = 0;
	int iContinue = 1;

	while (iContinue)
	{
		iContinue = find.FindNextFile();

		if (find.IsDots())
			continue;
		else if (find.IsDirectory())
			continue;
		else
		{
			if (find.IsSystem())
				continue;

			++iCount;
		}
	}

	return iCount;
}
