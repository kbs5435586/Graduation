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

	// PathRelativePathTo: 절대경로 -> 상대경로로 변환.
	// szCurDirectory기준으로 strFullPath으로 찾아가는 상대경로를 szRelativePath에 저장.
	PathRelativePathTo(szRelativePath, szCurDirectory, FILE_ATTRIBUTE_DIRECTORY,
		strFullPath.GetString(), FILE_ATTRIBUTE_DIRECTORY);

	return CString(szRelativePath);
}

void CFileInfo::DirInfoExtraction(const wstring& wstrPath, list<IMGPATHINFO*>& rImgInfoLst)
{
	// CFileFind: MFC에서 제공하는 파일 및 경로 제어 관련 클래스.
	CFileFind find;

	// wstrFilePath = D:\김태윤\4개월차\88기\Framework88\Texture\Stage\EFFECT\*.*
	wstring wstrFilePath = wstrPath + L"\\*.*";

	// CFileFind::FindFile함수: 해당 경로로부터 탐색을 시작 설정.
	find.FindFile(wstrFilePath.c_str());

	int iContinue = 1;

	while (iContinue)
	{
		// FindNextFile: 다음 파일을 찾는다. 더이상 찾을게 없다면 0을 반환.
		iContinue = find.FindNextFile();

		if (find.IsDots())	// 찾은게 .이나 ..같은 마커라면 건너뜀
			continue;
		else if (find.IsDirectory()) // 찾은게 경로라면 다음 파일을 찾을 때까지 재귀
		{
			// find.GetFilePath() = D:\김태윤\4개월차\88기\Framework88\Texture\Stage\EFFECT\BossMultiAttack
			DirInfoExtraction(wstring(find.GetFilePath()), rImgInfoLst);
		}
		else // 파일을 찾았을 때
		{
			if (find.IsSystem())	// 만일 숨어있는 시스템 파일이라면 건너뜀.
				continue;

			// find.GetFilePath() = D:\김태윤\4개월차\88기\Framework88\Texture\Stage\
			//							EFFECT\BossMultiAttack\BossMultiAttack0.png
			IMGPATHINFO* pImgPathInfo = new IMGPATHINFO;

			TCHAR szPathInfo[MAX_STR] = L"";
			lstrcpy(szPathInfo, find.GetFilePath().GetString());

			// szPathInfo = D:\김태윤\4개월차\88기\Framework88\Texture\Stage\EFFECT\BossMultiAttack
			PathRemoveFileSpec(szPathInfo);

			pImgPathInfo->iCount = CFileInfo::DirFileCount(szPathInfo);

			///////////////////////////////////////////////////////////////////////////////

			// wstrFileName = L"BossMultiAttack"
			wstring wstrFileName = find.GetFileTitle();

			// wstring::substr(시작, count): 시작부터 count만큼 해당하는 문자열을 얻어옴.
			// wstrFileName = L"BossMultiAttack%d.png"
			wstrFileName = wstrFileName.substr(0, wstrFileName.length() - 1) + L"%d.png";

			TCHAR szRelativeFullPath[MAX_STR] = L"";			
			
			// PathCombine(dst, src1, src2): src1과 src2를 결합한 경로를 만들어 dst에 저장.
			// 이 때 src1과 src2 사이에 \가 자동으로 삽입된다.
			// szRelativeFullPath = D:\김태윤\4개월차\88기\Framework88\Texture\Stage\EFFECT\BossMultiAttack\BossMultiAttack%d.png
			PathCombine(szRelativeFullPath, szPathInfo, wstrFileName.c_str());

			pImgPathInfo->wstrPath = CFileInfo::ConvertRelativePath(szRelativeFullPath);

			///////////////////////////////////////////////////////////////////////////////

			// szPathInfo = D:\김태윤\4개월차\88기\Framework88\Texture\Stage\EFFECT\BossMultiAttack

			// PathFindFileName: 파일명을 찾거나 말단 폴더명을 찾아냄.
			pImgPathInfo->wstrStateKey = PathFindFileName(szPathInfo);


			///////////////////////////////////////////////////////////////////////////////

			// szPathInfo = D:\김태윤\4개월차\88기\Framework88\Texture\Stage\EFFECT
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
