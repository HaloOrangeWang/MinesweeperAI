#include <algorithm>
#include <vector>
#include <ctime>
#include <random>
#include "game.h"
#include "ai.h"
#include <fstream>
#include <Windows.h>

using namespace std;

void calc_win_rate()
{
	int loss_cnt = 0;
	int win_cnt = 0;
	time_t st = clock();
	for (int t = 0; t <= 999; t++) {
		//if (t % 100 == 0)
		//	printf("第%d局\n", t);
		AI_V6 ai;
		bool res = ai.run();
		if (res) {
			win_cnt += 1;
			printf("第 %d 局：获胜\n", t);
		}
		else {
			loss_cnt += 1;
			printf("第 %d 局：失败\n", t);
		}
	}
	time_t ed = clock();
	printf("win_cnt = %d, loss_cnt = %d, ed - st = %d\n", win_cnt, loss_cnt, ed - st);
}

void run_use_real_exe()
{
	//使用真实exe进行游戏
	// 1.找到扫雷的窗口
	if (!InitHWND()) {
		system("pause");
		return;
	}
	// 2.进行游戏
	Sleep(1000);
	for (int t = 0; t <= 2499; t++) {
		AI_V6 ai;
		bool res = ai.run();
		if (res) {
			printf("第 %d 局：获胜\n", t);
		}
		else {
			printf("第 %d 局：失败\n", t);
		}
		Sleep(3000);
	}
}

void test(){}

int main()
{
	srand(time(NULL));
	if (MODE == 0)
		test();
	else if (MODE == 1)
		run_use_real_exe();
	else if (MODE == 2)
		calc_win_rate();
	system("pause");
	return 0;
}