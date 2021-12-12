#pragma once

#include "total.h"
#include <stdint.h>

class MinesGame
{
public:
	MinesGame(bool img2);
	void set_initial_map(int x, int y); //点开一个格子，然后生成初始的地图
	void right_click(int x, int y); //右键点一个格子
	void left_click(int x, int y); //左键点一个格子
	void double_click(int x, int y); //双击一个格子
	void right_click_around(int x, int y); //对一个格子周围所有未探索过的格子都进行右键点击
	void show(); //显示游戏画面
	int8_t observed_map[MAP_X][MAP_Y]; //观测到的地图。-1表示未探索，0-8表示数量，9表示旗子
	int8_t status; //游戏状态。0为进行中，1为失败，2为获胜
	int8_t g_map[MAP_X][MAP_Y]; //标明哪些格子是雷。1为雷，0不是雷。
public:
	bool check_win(); //判断是否已经获胜了
	void observe_1grid(int x, int y); //打开一个点。需要确保游戏正在运行，且这个点不是Mine，也没有被打开过。
	bool img; //真实游戏地图还是虚拟游戏地图
};

//使用真实扫雷exe进行游戏时，使用到的函数
extern bool InitHWND();
extern void GetObservedMap(int8_t observed_map[MAP_X][MAP_Y]);
extern bool CheckWin();
extern void RightClick(int x, int y);
extern int LeftClick(int x, int y);
extern int DoubleClick(int x, int y);
extern void RightClickAround(int x, int y, int8_t observed_map[MAP_X][MAP_Y]);
