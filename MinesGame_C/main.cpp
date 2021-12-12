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
		//	printf("��%d��\n", t);
		AI_V6 ai;
		bool res = ai.run();
		if (res) {
			win_cnt += 1;
			printf("�� %d �֣���ʤ\n", t);
		}
		else {
			loss_cnt += 1;
			printf("�� %d �֣�ʧ��\n", t);
		}
	}
	time_t ed = clock();
	printf("win_cnt = %d, loss_cnt = %d, ed - st = %d\n", win_cnt, loss_cnt, ed - st);
}

void run_use_real_exe()
{
	//ʹ����ʵexe������Ϸ
	// 1.�ҵ�ɨ�׵Ĵ���
	if (!InitHWND()) {
		system("pause");
		return;
	}
	// 2.������Ϸ
	Sleep(1000);
	for (int t = 0; t <= 2499; t++) {
		AI_V6 ai;
		bool res = ai.run();
		if (res) {
			printf("�� %d �֣���ʤ\n", t);
		}
		else {
			printf("�� %d �֣�ʧ��\n", t);
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