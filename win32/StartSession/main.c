/*
 * Author:��������
 * Date:	2015.6.19
 * Project:	һ�����ٴ򿪶������ĳ�������Ϊ�������������͵һ����
 */

#include <Windows.h>
#include <stdlib.h>

TCHAR *CharToTchar(char *_char);
char* getNextCommand(int);

#define BUFSIZE 2000
PVOID buf;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow){
	char* szCommandline;
	DWORD n;
	HANDLE handle;
	TCHAR *sztemp;
	TCHAR szbuf[200];
	DWORD error;

	if (MessageBox(NULL, TEXT("�Ƿ������Ự��"), TEXT("Session"), MB_YESNO) != IDYES){
		return 0;
	}

	buf = malloc(BUFSIZE);
	handle = CreateFile(TEXT("config.txt"), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (handle == INVALID_HANDLE_VALUE){
		MessageBox(NULL, TEXT("���ܴ�config.txt������Ҫ��StartSession.exe��ͬһĿ¼��"), 0, 0);
		return -1;
	}
	if (ReadFile(handle, buf, BUFSIZE, &n, NULL) == FALSE){
		HLOCAL hlocal = NULL;
		DWORD systemLocale = MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL);
		error = GetLastError();
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, error, systemLocale, (PTSTR)&hlocal, 0, NULL);
		wsprintf(szbuf, TEXT("���ܶ�ȡconfig.txt, ԭ��Ϊ��%s"), (PCTSTR)LocalLock(hlocal));
		MessageBox(NULL, szbuf, 0, 0);
		LocalFree(hlocal);
		return -1;
	}
	if ((szCommandline = getNextCommand(0)) == NULL){
		MessageBox(NULL, TEXT("config.txt����Ϊ�ա��������һ������session.txt��·�������Իس�������"), 0, 0);
		return -1;
	}
	sztemp = CharToTchar(szCommandline);
	handle = CreateFile(sztemp, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	free(sztemp);
	if (handle == INVALID_HANDLE_VALUE){
		MessageBox(NULL, TEXT("���ܴ�session.txt������config.txt�ڵ�·��"), 0, 0);
		return -1;
	}
	if (ReadFile(handle, buf, BUFSIZE, &n, NULL)==FALSE){
		HLOCAL hlocal = NULL;
		DWORD systemLocale = MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL);
		error = GetLastError();
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, error, systemLocale, (PTSTR)&hlocal, 0, NULL);
		wsprintf(szbuf, TEXT("���ܶ�ȡsession.txt, ԭ��Ϊ��%s"), (PCTSTR)LocalLock(hlocal));
		MessageBox(NULL, szbuf, 0, 0);
		LocalFree(hlocal);
		return -1;
	}
	else if (n == BUFSIZE){
		MessageBox(NULL, TEXT("session.txt�������࣬���޸�����������"), TEXT("Notice"), MB_OK);
		return -1;
	}

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	int num = 0;
	while ((szCommandline=getNextCommand(1)) != NULL){
		if (szCommandline[0] == '#'){		// ���ע��
			continue;
		}
		num++;
		BOOL ret = 0;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));

		sztemp = CharToTchar(szCommandline);
		ret = CreateProcess(NULL, sztemp, NULL, NULL, 0, 0, NULL, NULL, &si, &pi);
		error = GetLastError();
		if (ret != TRUE){
			HLOCAL hlocal = NULL;	// Buffer that gets the error message string
			// Use the default system locale since we look for Windows messages
			// Note: this MAKELANGID combination has a value of 0
			DWORD systemLocale = MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL);
			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, error, systemLocale, (PTSTR)&hlocal, 0, NULL);
			wsprintf(szbuf, TEXT("����ִ�е�%d������, ԭ��Ϊ��%s"), num, (PCTSTR)LocalLock(hlocal));
			MessageBox(NULL, szbuf, 0, 0);
			LocalFree(hlocal);
		}
		free(sztemp);
	}
	free(buf);
	CloseHandle(handle);
	return 0;
}

// ASCII�ַ�ת���ַ�
// ע�⣺�������ͷŻ�����
TCHAR* CharToTchar(char *_char){
	DWORD tlen;
	TCHAR *tchar;
	// ȡ�û���������
	tlen = MultiByteToWideChar(CP_ACP, 0, _char, -1, NULL, 0);
	// ���仺����
	tchar = malloc(tlen*sizeof(TCHAR));
	// д�뻺����
	MultiByteToWideChar(CP_ACP, 0, _char, strlen(_char) + 1, tchar, tlen*sizeof(TCHAR));
	return tchar;
}

// ����&_&��id��һ�����޵����
char* getNextCommand(int id){
	static char *lastpos=NULL;
	static int kd= -1;
	char *start, *end;
	if (id != kd){
		lastpos = NULL;
		kd = id;
	}

	if (lastpos){
		start = lastpos;
	}
	else{
		start = buf;
	}

	end = strchr(start, '\r');
	if (end == NULL){
		return NULL;
	}
	*(end++) = 0;		// ��\rΪ0
	*(end++) = 0;		// ��\nΪ0
	lastpos = end;
	return start;
}