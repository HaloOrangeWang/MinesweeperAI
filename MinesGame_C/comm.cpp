#include <stdio.h>
#include <Windows.h>
#include <stdint.h>
#include "total.h"

void* MineThreadID;
HWND MineWindow;

bool InitHWND()
{
	//获取6个扫雷窗口
	//1.先获取第一个扫雷窗口
	MineWindow = FindWindow("扫雷", "扫雷");
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
	//读取扫雷程序的内存，获取当前的地图内容
	byte lpBuffer1[512];
	BOOL b;
	b = ReadProcessMemory(
		MineThreadID,   // 目标进程句柄
		(void*)0x01005360,//要读取的内存地址
		lpBuffer1,  // 存放数据的缓存区地址
		512,      // 要读取的字节数
		NULL  // 把读了多少个字节存到一个地址，如果为NULL则忽略此参数。
	);
	for (int x = 0; x <= 15; x++) {
		for (int y = 0; y <= 29; y++) {
			char tmp = lpBuffer1[x * 32 + y + 1] & 0xf;
			if (tmp == 0xf) //空格
				observed_map[x][y] = -1;
			else if (tmp == 0xe) //旗子
				observed_map[x][y] = 9;
			else if (tmp >= 0x0 && tmp <= 0x8) //数字
				observed_map[x][y] = tmp;
		}
	}
}

bool CheckWin()
{
	// 判断是否获胜了
	byte lpBuffer1[8];
	BOOL b;
	b = ReadProcessMemory(
		MineThreadID,   // 目标进程句柄
		(void*)0x010057A0,//要读取的内存地址
		lpBuffer1,  // 存放数据的缓存区地址
		8,      // 要读取的字节数
		NULL  // 把读了多少个字节存到一个地址，如果为NULL则忽略此参数。
	);
	int grid_need_open = lpBuffer1[1] * 256 + lpBuffer1[0]; //本应打开多少个格子
	int grid_opened = lpBuffer1[5] * 256 + lpBuffer1[4]; //实际打开了多少个格子
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
	// 检查这一步操作完成之后是否失败了
	byte lpBuffer1[512];
	BOOL b;
	b = ReadProcessMemory(
		MineThreadID,   // 目标进程句柄
		(void*)0x01005360,//要读取的内存地址
		lpBuffer1,  // 存放数据的缓存区地址
		512,      // 要读取的字节数
		NULL  // 把读了多少个字节存到一个地址，如果为NULL则忽略此参数。
	);
	for (int x = 0; x <= 15; x++) {
		for (int y = 0; y <= 29; y++) {
			char tmp = lpBuffer1[x * 32 + y + 1] & 0xf;
			if (tmp == 10 || tmp == 12)
				return 1;
		}
	}
	// 检查这一步操作完成之后是否获胜了
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
	// 检查这一步操作完成之后是否失败了
	byte lpBuffer1[512];
	BOOL b;
	b = ReadProcessMemory(
		MineThreadID,   // 目标进程句柄
		(void*)0x01005360,//要读取的内存地址
		lpBuffer1,  // 存放数据的缓存区地址
		512,      // 要读取的字节数
		NULL  // 把读了多少个字节存到一个地址，如果为NULL则忽略此参数。
	);
	for (int x = 0; x <= 15; x++) {
		for (int y = 0; y <= 29; y++) {
			char tmp = lpBuffer1[x * 32 + y + 1] & 0xf;
			if (tmp == 10 || tmp == 12)
				return 1;
		}
	}
	// 检查这一步操作完成之后是否获胜了
	if (CheckWin())
		return 2;
	return 0;
}

void RightClickAround(int x, int y, int8_t observed_map[MAP_X][MAP_Y])
{
	//将这个格子周围observed_map为-1的格子全部插上旗子
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