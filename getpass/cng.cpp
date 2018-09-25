#include "stdafx.h"
#include "cng.h"

#pragma comment(lib, "Bcrypt.lib")

BOOL DesDecrypt(IN LPBYTE lpEncryptBuf, IN DWORD cbEncryptPassword,
	IN LPBYTE lpKey, IN DWORD cbKey, IN LPBYTE lpIV,
	IN DWORD cbIV, OUT LPBYTE lpPlainOutput, OUT DWORD cbPlainOutput)
{
	BCRYPT_ALG_HANDLE hBcryptALG = NULL;
	BCRYPT_KEY_HANDLE hDesKey = NULL;
	DWORD cbData = 0;
	NTSTATUS status;
	/* ÿ��BCryptDecrypt��ı�iv��������ֵ����ε��ú���ܻ���֡���֡������⣬����Ҫ���ƽ�szLocalIV����*/
	BYTE  szLocalIV[16] = { 0 };
	RtlCopyMemory(szLocalIV, lpIV, cbIV);

	if (NT_ERROR(status = BCryptOpenAlgorithmProvider(&hBcryptALG, BCRYPT_3DES_ALGORITHM, NULL, 0)))
	{
		fwprintf(stderr, L"BCryptOpenAlgorithmProvider: [%x]\r\n", status);
		return FALSE;
	}

	//����CBC���ģʽ
	if (NT_ERROR(status = BCryptSetProperty(hBcryptALG, BCRYPT_CHAINING_MODE,
		(LPBYTE)BCRYPT_CHAIN_MODE_CBC, sizeof(BCRYPT_CHAIN_MODE_CBC), 0)))
	{
		fwprintf(stderr, L"BCryptSetProperty: BCRYPT_CHAINING_MODE [%x]\r\n", status);
		return FALSE;
	}

	if (NT_ERROR(status = BCryptGenerateSymmetricKey(hBcryptALG, &hDesKey, NULL,
		0, lpKey, cbKey, 0)))
	{
		fwprintf(stderr, L"BCryptGenerateSymmetricKey [%x]\r\n", status);
		return FALSE;
	}

	//����
	if (NT_ERROR(status = BCryptDecrypt(hDesKey, lpEncryptBuf, cbEncryptPassword,
		NULL, szLocalIV, cbIV, lpPlainOutput,
		cbPlainOutput, &cbData, 0))) //���һ����������Ϊ0��Ĭ��ʹ��Zero���
	{
		fwprintf(stderr, L"BCryptDecrypt [%x]\r\n", status);
		return FALSE;
	}
	//�ͷž��
	BCryptCloseAlgorithmProvider(hBcryptALG, 0);
	BCryptDestroyKey(hDesKey);

	lpEncryptBuf[cbData] = 0;
	lpEncryptBuf[cbData + 1] = 0;

	return TRUE;
}

BOOL AesDecrypt(IN LPBYTE lpEncryptBuf, IN DWORD cbEncryptPassword,
	IN LPBYTE lpKey, IN DWORD cbKey, IN LPBYTE lpIV,
	IN DWORD cbIV, OUT LPBYTE lpPlainOutput, OUT DWORD cbPlainOutput)
{
	BCRYPT_ALG_HANDLE hBcryptALG = NULL;
	BCRYPT_KEY_HANDLE hAesKey = NULL;
	DWORD cbData = 0;
	NTSTATUS status;

	if (NT_ERROR(status = BCryptOpenAlgorithmProvider(&hBcryptALG, BCRYPT_AES_ALGORITHM, NULL, 0)))
	{
		fwprintf(stderr, L"BCryptOpenAlgorithmProvider: [%x]\r\n", status);
		return FALSE;
	}

	//����CFB���ģʽ
	if (NT_ERROR(status = BCryptSetProperty(hBcryptALG, BCRYPT_CHAINING_MODE,
		(LPBYTE)BCRYPT_CHAIN_MODE_CBC, sizeof(BCRYPT_CHAIN_MODE_CFB), 0)))
	{
		fwprintf(stderr, L"BCryptSetProperty: BCRYPT_CHAINING_MODE [%x]\r\n", status);
		return FALSE;
	}

	if (NT_ERROR(status = BCryptGenerateSymmetricKey(hBcryptALG, &hAesKey, NULL,
		0, lpKey, cbKey, 0)))
	{
		fwprintf(stderr, L"BCryptGenerateSymmetricKey [%x]\r\n", status);
		return FALSE;
	}

	//����
	if (NT_ERROR(status = BCryptDecrypt(hAesKey, lpEncryptBuf, cbEncryptPassword,
		NULL, lpIV, cbIV, lpPlainOutput,
		cbPlainOutput, &cbData, 0))) //���һ����������Ϊ0��Ĭ��ʹ��Zero���
	{
		fwprintf(stderr, L"BCryptDecrypt [%x]\r\n", status);
		return FALSE;
	}
	lpEncryptBuf[cbData] = 0;
	lpEncryptBuf[cbData + 1] = 0;
	return TRUE;
}