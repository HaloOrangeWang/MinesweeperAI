#include "total.h"
#include "game.h"
#include <algorithm>
#include <vector>
#include <Windows.h>

using namespace std;

MinesGame::MinesGame(bool img2)
{
	img = img2;
	status = 0;
	for (int x = 0; x <= MAP_X - 1; x++) {
		for (int y = 0; y <= MAP_Y - 1; y++) {
			g_map[x][y] = 0;
			observed_map[x][y] = -1;
		}
	}
}

void MinesGame::set_initial_map(int x, int y)
{
	//获取哪些格子可以是mine的列表
	vector<pair<int, int>> grids_can_be_mine{};
	for (int tx = 0; tx <= MAP_X - 1; tx++) {
		for (int ty = 0; ty <= MAP_Y - 1; ty++) {
			if ((tx < x - 1 || tx > x + 1) || (ty < y - 1 || ty > y + 1)) {
			//if ((tx != x) || (ty != y)) {
				grids_can_be_mine.push_back(make_pair(tx, ty));
			}
		}
	}
	//获取mine列表
	random_shuffle(grids_can_be_mine.begin(), grids_can_be_mine.end());
	for (int t = 0; t <= MINES - 1; t++) {
		g_map[grids_can_be_mine[t].first][grids_can_be_mine[t].second] = 1;
	}
	//开地图
	if (MODE == 1 && img == false) {
		//使用真实扫雷进程
		LeftClick(x, y);
		GetObservedMap(observed_map);
		if (check_win())
			status = 2;
	}
	else {
		//使用程序内部维护的游戏局
		observe_1grid(x, y);
		if (check_win())
			status = 2;
	}
}

bool MinesGame::check_win(){
	if (MODE == 1 && img == false) {
		//使用真实扫雷进程
		return CheckWin();
	}
	else {
		//如果已观测到的非mine个数与实际非mine个数相等，则可以判断为获胜
		int observed_not_mine = 0;
		for (int x = 0; x <= MAP_X - 1; x++) {
			for (int y = 0; y <= MAP_Y - 1; y++) {
				if (observed_map[x][y] >= 0 && observed_map[x][y] <= 8) {
					observed_not_mine += 1;
				}
			}
		}
		if (observed_not_mine == MAP_X * MAP_Y - MINES)
			return true;
		return false;
	}
}

void MinesGame::observe_1grid(int x, int y)
{
	int num_mines_around = 0;
	for (int tx = max(0, x - 1); tx <= min(MAP_X - 1, x + 1); tx++) {
		for (int ty = max(0, y - 1); ty <= min(MAP_Y - 1, y + 1); ty++) {
			if (g_map[tx][ty] == 1) {
				num_mines_around += 1;
			}
		}
	}
	observed_map[x][y] = num_mines_around;
	if (num_mines_around == 0) {
		for (int tx = max(0, x - 1); tx <= min(MAP_X - 1, x + 1); tx++) {
			for (int ty = max(0, y - 1); ty <= min(MAP_Y - 1, y + 1); ty++) {
				if ((tx != x || ty != y) && observed_map[tx][ty] == -1) {
					observe_1grid(tx, ty);
				}
			}
		}
	}
}

void MinesGame::right_click(int x, int y)
{
	if (status != 0) {
		printf("不能在Game Over的状态下点击\n");
	}
	else {
		if (MODE == 1 && img == false) {
			//使用真实扫雷进程
			RightClick(x, y);
			Sleep(REAL_MAP_SLEEP);
			GetObservedMap(observed_map);
		}
		else {
			if (observed_map[x][y] == -1)
				observed_map[x][y] = 9;
			else if (observed_map[x][y] == 9)
				observed_map[x][y] = -1;
		}
	}
}

void MinesGame::left_click(int x, int y)
{
	if (status != 0) {
		printf("不能在Game Over的状态下点击\n");
	}
	else {
		if (MODE == 1 && img == false) {
			//使用真实扫雷进程
			status = LeftClick(x, y);
			GetObservedMap(observed_map);
		}
		else {
			if (g_map[x][y] == 1) { //如果点到mine了就直接结束
				status = 1;
				return;
			}
			else if (observed_map[x][y] != -1) {
				printf("不能重复打开已打开过的地方\n");
				abort();
			}
			else {
				observe_1grid(x, y);
				if (check_win())
					status = 2;
			}
		}
	}
}

void MinesGame::double_click(int x, int y)
{
	if (status != 0) {
		printf("不能在Game Over的状态下点击\n");
	}
	else if (observed_map[x][y] <= 0 || observed_map[x][y] == 9) printf("不能双击未探索、标位0或旗子的方格\n");
	else {
		if (MODE == 1 && img == false) {
			//使用真实扫雷进程
			status = DoubleClick(x, y);
			GetObservedMap(observed_map);
		}
		else {
			// 检查格子周围的旗子个数与格子标数是否一致，如果不一致则不执行
			int num_flags_around = 0;
			for (int tx = max(0, x - 1); tx <= min(MAP_X - 1, x + 1); tx++) {
				for (int ty = max(0, y - 1); ty <= min(MAP_Y - 1, y + 1); ty++) {
					if (observed_map[tx][ty] == 9) {
						num_flags_around += 1;
					}
				}
			}
			if (num_flags_around != observed_map[x][y]) {
				printf("双击之前需要保证周围的旗子数量与方格标数一致\n");
				return;
			}
			//探索这个格子周围所有未定义的方格。如果其中有一个是mine则游戏结束
			for (int tx = max(0, x - 1); tx <= min(MAP_X - 1, x + 1); tx++) {
				for (int ty = max(0, y - 1); ty <= min(MAP_Y - 1, y + 1); ty++) {
					if ((tx != x || ty != y) && observed_map[tx][ty] == -1) {
						if (g_map[tx][ty] == 1) {
							status = 1;
							return;
						}
						else
							observe_1grid(tx, ty);
					}
				}
			}
			//判断是否获胜了
			if (check_win())
				status = 2;
		}
	}
}

void MinesGame::right_click_around(int x, int y)
{
	if (status != 0) {
		printf("不能在Game Over的状态下点击\n");
	}
	else {
		if (MODE == 1 && img == false) {
			//使用真实扫雷进程
			RightClickAround(x, y, observed_map);
			Sleep(REAL_MAP_SLEEP);
			GetObservedMap(observed_map);
		}
		else {
			for (int tx = max(0, x - 1); tx <= min(MAP_X - 1, x + 1); tx++) {
				for (int ty = max(0, y - 1); ty <= min(MAP_Y - 1, y + 1); ty++) {
					if ((tx != x || ty != y) && observed_map[tx][ty] == -1) {
						observed_map[tx][ty] = 9;
					}
				}
			}
		}
	}
}

void MinesGame::show()
{
	system("cls");
	for (int x = 0; x <= MAP_X - 1; x++) {
		for (int y = 0; y <= MAP_Y - 1; y++) {
			if (observed_map[x][y] == -1)
				printf("##");
			else if (observed_map[x][y] == 0)
				printf("  ");
			else if (observed_map[x][y] <= 8)
				printf("%d ", observed_map[x][y]);
			else
				printf("△");
		}
		printf("\n");
	}
}