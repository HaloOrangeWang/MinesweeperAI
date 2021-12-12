#include <stdio.h>
#include <Windows.h>
#include <stdint.h>
#include "total.h"

void* MineThreadID;
HWND MineWindow;

bool InitHWND()
{
	//��ȡ6��ɨ�״���
	//1.�Ȼ�ȡ��һ��ɨ�״���
	MineWindow = FindWindow("ɨ��", "ɨ��");
	if (MineWindow == nullptr) {
		printf("failed to find minesweeper program #1.\n");
		return false;
	}
	RECT mine_window_loc;
	GetWindowRect(MineWindow, &mine_window_loc);

	DWORD pid;
	GetWindowThreadProcessId((HWND__*)(MineWindow), &pid);
	MineThreadID = OpenProcess(PROCESS_ALL_ACCESS, false, pid);

	return true;
}

void GetObservedMap(int8_t observed_map[MAP_X][MAP_Y])
{
	//��ȡɨ�׳�����ڴ棬��ȡ��ǰ�ĵ�ͼ����
	byte lpBuffer1[512];
	BOOL b;
	b = ReadProcessMemory(
		MineThreadID,   // Ŀ����̾��
		(void*)0x01005360,//Ҫ��ȡ���ڴ��ַ
		lpBuffer1,  // ������ݵĻ�������ַ
		512,      // Ҫ��ȡ���ֽ���
		NULL  // �Ѷ��˶��ٸ��ֽڴ浽һ����ַ�����ΪNULL����Դ˲�����
	);
	for (int x = 0; x <= 15; x++) {
		for (int y = 0; y <= 29; y++) {
			char tmp = lpBuffer1[x * 32 + y + 1] & 0xf;
			if (tmp == 0xf) //�ո�
				observed_map[x][y] = -1;
			else if (tmp == 0xe) //����
				observed_map[x][y] = 9;
			else if (tmp >= 0x0 && tmp <= 0x8) //����
				observed_map[x][y] = tmp;
		}
	}
}

bool CheckWin()
{
	// �ж��Ƿ��ʤ��
	byte lpBuffer1[8];
	BOOL b;
	b = ReadProcessMemory(
		MineThreadID,   // Ŀ����̾��
		(void*)0x010057A0,//Ҫ��ȡ���ڴ��ַ
		lpBuffer1,  // ������ݵĻ�������ַ
		8,      // Ҫ��ȡ���ֽ���
		NULL  // �Ѷ��˶��ٸ��ֽڴ浽һ����ַ�����ΪNULL����Դ˲�����
	);
	int grid_need_open = lpBuffer1[1] * 256 + lpBuffer1[0]; //��Ӧ�򿪶��ٸ�����
	int grid_opened = lpBuffer1[5] * 256 + lpBuffer1[4]; //ʵ�ʴ��˶��ٸ�����
	if (grid_need_open == grid_opened) {
		return true;
	}
	else {
		return false;
	}
}

void RightClick(int x, int y)
{
	SendMessage(MineWindow, WM_RBUTTONDOWN, 0, MAKELPARAM(20 + y * 16, 60 + x * 16));
	Sleep(REAL_MAP_SLEEP);
	SendMessage(MineWindow, WM_RBUTTONUP, 0, MAKELPARAM(20 + y * 16, 60 + x * 16));
}

int LeftClick(int x, int y)
{
	SendMessage(MineWindow, WM_LBUTTONDOWN, 0, MAKELPARAM(20 + y * 16, 60 + x * 16));
	Sleep(REAL_MAP_SLEEP);
	SendMessage(MineWindow, WM_LBUTTONUP, 0, MAKELPARAM(20 + y * 16, 60 + x * 16));
	Sleep(REAL_MAP_SLEEP);
	// �����һ���������֮���Ƿ�ʧ����
	byte lpBuffer1[512];
	BOOL b;
	b = ReadProcessMemory(
		MineThreadID,   // Ŀ����̾��
		(void*)0x01005360,//Ҫ��ȡ���ڴ��ַ
		lpBuffer1,  // ������ݵĻ�������ַ
		512,      // Ҫ��ȡ���ֽ���
		NULL  // �Ѷ��˶��ٸ��ֽڴ浽һ����ַ�����ΪNULL����Դ˲�����
	);
	for (int x = 0; x <= 15; x++) {
		for (int y = 0; y <= 29; y++) {
			char tmp = lpBuffer1[x * 32 + y + 1] & 0xf;
			if (tmp == 10 || tmp == 12)
				return 1;
		}
	}
	// �����һ���������֮���Ƿ��ʤ��
	if (CheckWin())
		return 2;
	return 0;
}

int DoubleClick(int x, int y)
{
	SendMessage(MineWindow, WM_LBUTTONDOWN, 0, MAKELPARAM(20 + y * 16, 60 + x * 16));
	SendMessage(MineWindow, WM_RBUTTONDOWN, 0, MAKELPARAM(20 + y * 16, 60 + x * 16));
	Sleep(REAL_MAP_SLEEP);
	SendMessage(MineWindow, WM_LBUTTONUP, 0, MAKELPARAM(20 + y * 16, 60 + x * 16));
	SendMessage(MineWindow, WM_RBUTTONUP, 0, MAKELPARAM(20 + y * 16, 60 + x * 16));
	Sleep(REAL_MAP_SLEEP);
	// �����һ���������֮���Ƿ�ʧ����
	byte lpBuffer1[512];
	BOOL b;
	b = ReadProcessMemory(
		MineThreadID,   // Ŀ����̾��
		(void*)0x01005360,//Ҫ��ȡ���ڴ��ַ
		lpBuffer1,  // ������ݵĻ�������ַ
		512,      // Ҫ��ȡ���ֽ���
		NULL  // �Ѷ��˶��ٸ��ֽڴ浽һ����ַ�����ΪNULL����Դ˲�����
	);
	for (int x = 0; x <= 15; x++) {
		for (int y = 0; y <= 29; y++) {
			char tmp = lpBuffer1[x * 32 + y + 1] & 0xf;
			if (tmp == 10 || tmp == 12)
				return 1;
		}
	}
	// �����һ���������֮���Ƿ��ʤ��
	if (CheckWin())
		return 2;
	return 0;
}

void RightClickAround(int x, int y, int8_t observed_map[MAP_X][MAP_Y])
{
	//�����������Χobserved_mapΪ-1�ĸ���ȫ����������
	for (int tx = max(0, x - 1); tx <= min(MAP_X - 1, x + 1); tx++) {
		for (int ty = max(0, y - 1); ty <= min(MAP_Y - 1, y + 1); ty++) {
			if ((tx != x || ty != y) && observed_map[tx][ty] == -1) {
				SendMessage(MineWindow, WM_RBUTTONDOWN, 0, MAKELPARAM(20 + ty * 16, 60 + tx * 16));
				Sleep(REAL_MAP_SLEEP);
				SendMessage(MineWindow, WM_RBUTTONUP, 0, MAKELPARAM(20 + ty * 16, 60 + tx * 16));
			}
		}
	}
}