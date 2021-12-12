#pragma once

#include "total.h"
#include <stdint.h>

class MinesGame
{
public:
	MinesGame(bool img2);
	void set_initial_map(int x, int y); //�㿪һ�����ӣ�Ȼ�����ɳ�ʼ�ĵ�ͼ
	void right_click(int x, int y); //�Ҽ���һ������
	void left_click(int x, int y); //�����һ������
	void double_click(int x, int y); //˫��һ������
	void right_click_around(int x, int y); //��һ��������Χ����δ̽�����ĸ��Ӷ������Ҽ����
	void show(); //��ʾ��Ϸ����
	int8_t observed_map[MAP_X][MAP_Y]; //�۲⵽�ĵ�ͼ��-1��ʾδ̽����0-8��ʾ������9��ʾ����
	int8_t status; //��Ϸ״̬��0Ϊ�����У�1Ϊʧ�ܣ�2Ϊ��ʤ
	int8_t g_map[MAP_X][MAP_Y]; //������Щ�������ס�1Ϊ�ף�0�����ס�
public:
	bool check_win(); //�ж��Ƿ��Ѿ���ʤ��
	void observe_1grid(int x, int y); //��һ���㡣��Ҫȷ����Ϸ�������У�������㲻��Mine��Ҳû�б��򿪹���
	bool img; //��ʵ��Ϸ��ͼ����������Ϸ��ͼ
};

//ʹ����ʵɨ��exe������Ϸʱ��ʹ�õ��ĺ���
extern bool InitHWND();
extern void GetObservedMap(int8_t observed_map[MAP_X][MAP_Y]);
extern bool CheckWin();
extern void RightClick(int x, int y);
extern int LeftClick(int x, int y);
extern int DoubleClick(int x, int y);
extern void RightClickAround(int x, int y, int8_t observed_map[MAP_X][MAP_Y]);
