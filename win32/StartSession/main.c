/*
 * Author:　王宗磊
 * Date:	2015.6.19
 * Project:	一个快速打开多个程序的程序。设置为开机启动后可以偷一下懒
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

	if (MessageBox(NULL, TEXT("是否启动会话？"), TEXT("Session"), MB_YESNO) != IDYES){
		return 0;
	}

	buf = malloc(BUFSIZE);
	handle = CreateFile(TEXT("config.txt"), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (handle == INVALID_HANDLE_VALUE){
		MessageBox(NULL, TEXT("不能打开config.txt，它需要和StartSession.exe在同一目录下"), 0, 0);
		return -1;
	}
	if (ReadFile(handle, buf, BUFSIZE, &n, NULL) == FALSE){
		HLOCAL hlocal = NULL;
		DWORD systemLocale = MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL);
		error = GetLastError();
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, error, systemLocale, (PTSTR)&hlocal, 0, NULL);
		wsprintf(szbuf, TEXT("不能读取config.txt, 原因为：%s"), (PCTSTR)LocalLock(hlocal));
		MessageBox(NULL, szbuf, 0, 0);
		LocalFree(hlocal);
		return -1;
	}
	if ((szCommandline = getNextCommand(0)) == NULL){
		MessageBox(NULL, TEXT("config.txt不能为空。请在其第一行输入session.txt的路径，并以回车键结束"), 0, 0);
		return -1;
	}
	sztemp = CharToTchar(szCommandline);
	handle = CreateFile(sztemp, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	free(sztemp);
	if (handle == INVALID_HANDLE_VALUE){
		MessageBox(NULL, TEXT("不能打开session.txt，请检查config.txt内的路径"), 0, 0);
		return -1;
	}
	if (ReadFile(handle, buf, BUFSIZE, &n, NULL)==FALSE){
		HLOCAL hlocal = NULL;
		DWORD systemLocale = MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL);
		error = GetLastError();
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, error, systemLocale, (PTSTR)&hlocal, 0, NULL);
		wsprintf(szbuf, TEXT("不能读取session.txt, 原因为：%s"), (PCTSTR)LocalLock(hlocal));
		MessageBox(NULL, szbuf, 0, 0);
		LocalFree(hlocal);
		return -1;
	}
	else if (n == BUFSIZE){
		MessageBox(NULL, TEXT("session.txt字数过多，请修改字数后重试"), TEXT("Notice"), MB_OK);
		return -1;
	}

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	int num = 0;
	while ((szCommandline=getNextCommand(1)) != NULL){
		if (szCommandline[0] == '#'){		// 添加注释
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
			wsprintf(szbuf, TEXT("不能执行第%d条命令, 原因为：%s"), num, (PCTSTR)LocalLock(hlocal));
			MessageBox(NULL, szbuf, 0, 0);
			LocalFree(hlocal);
		}
		free(sztemp);
	}
	free(buf);
	CloseHandle(handle);
	return 0;
}

// ASCII字符转宽字符
// 注意：不负责释放缓冲区
TCHAR* CharToTchar(char *_char){
	DWORD tlen;
	TCHAR *tchar;
	// 取得缓冲区长度
	tlen = MultiByteToWideChar(CP_ACP, 0, _char, -1, NULL, 0);
	// 分配缓冲区
	tchar = malloc(tlen*sizeof(TCHAR));
	// 写入缓冲区
	MultiByteToWideChar(CP_ACP, 0, _char, strlen(_char) + 1, tchar, tlen*sizeof(TCHAR));
	return tchar;
}

// 作者&_&：id是一个很赞的设计
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
	*(end++) = 0;		// 置\r为0
	*(end++) = 0;		// 置\n为0
	lastpos = end;
	return start;
}