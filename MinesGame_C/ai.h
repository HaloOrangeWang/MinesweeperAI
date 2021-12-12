#include "total.h"
#include "game.h"
#include <random>
#include <vector>
#include <set>
#include <map>
#include <memory>
#include <fstream>

using namespace std;

class AI_V1
{
public:
	AI_V1();
	bool run();
private:
	void run_1step();
	int8_t skip_grids[MAP_X][MAP_Y];
	MinesGame g;
	random_device rd;
};

//�Ե�ͼ���з��δ���Ľ��
struct Equations
{
	int num_grid; //һ���ж��ٸ�����
	map<pair<int, int>, int> grid_indexes; //ÿ�����ӵ�x��y�������ڽṹ������ֵ֮��Ķ�Ӧ��ϵ
	vector<pair<int, int>> grid_list; //����ֵ����ӵ�x��y�ķ����Ӧ��ϵ
	vector<vector<int>> params; //������Ĳ���
	vector<int> results; //������Ľ��
};

struct SolveResult
{
	bool have_content;
	vector<int> right_click_dx_list; //�������ĸ����б�
	vector<int> left_click_dx_list; //����㿪�ĸ����б�
};

struct MinesGame2
{
	int8_t skip_grids[MAP_X][MAP_Y];
	int mines_left; //ʣ��mines������������ȷ���Ƿ����ʣ��mines�����µķ�����
	MinesGame g;
	MinesGame2(bool img) :g(img) {}
};

class AI_V6
{
public:
	AI_V6();
	bool run();
public:
	void run_1step();
	bool choice_single(bool img, MinesGame2& g2);  //V1�汾�ļ��߼�
	Equations create_equations(bool img, MinesGame2& g2); //������ǰ��ͼ��Ӧ�ķ�����
	vector<set<int>> divide_equations(int num_grid, int num_equation, unique_ptr<int[]>& all_params); //����Щ������ָ�����ɸ�������ص�С������
	vector<unique_ptr<int[]>> gen_combinations(int num_grid, vector<int>& new_grid_dx_list, unique_ptr<int[]>& base_params, int curr_pos);
	unique_ptr<double[]> get_mine_rate_by_grid(bool img, MinesGame2& g2, vector<pair<int, int>>& grid_list, int num_grid, unique_ptr<int[]>& all_params, vector<int>& all_results, vector<int>& usable_equas_dx_list); //����ÿ��������mine�ĸ���
	bool choice_by_equation(bool img, MinesGame2& g2, map<pair<int, int>, double>& all_grids_mine_rate); //ͨ���ⷽ��������ȡ���Ӿ�ȷ���ж�
	void guess_1grid(map<pair<int, int>, double>& all_grids_mine_rate); //���ⷽ��������������ʱ���²�һ������
	MinesGame2 cur_g;
	std::random_device rd;
public:
	void store_legal_combinations(int num_grid, vector<pair<int, int>>& grid_list, vector<unique_ptr<int[]>>& possible_mines_combination_list, vector<bool>& legal_combination_dx_list); //��¼��Щ���Ǻ�������
	map<pair<int, int>, double> calc_next_step_by_grid(); //����㿪ÿ�����Ӻ���һ״̬������
	const int PredictGridNum = 16; //��ʣ���������С���������ʱ���ͽ���״̬Ԥ�⡣���ȡֵ18
	const int GlCalcMinesLeft = 9; //��ʣ��mines����С���������ʱ��������mines����������㡣���ȡֵ9
	vector<vector<pair<int, int>>> all_legal_combinations; //��¼��ǰ״̬����Щ����Ϊmine��ȫ����ϡ�����ʣ����ӽ���ʱ��������������ݲ�����ȷ�ġ�
	vector<pair<int, int>> unknown_grid_list; //ʣ��ȫ��δ�򿪵ĸ����б�����ʣ����ӽ���ʱ��������������ݲ�����ȷ�ġ�
public:
	pair<int, int> guess_1grid_by_mine_rate(map<pair<int, int>, double>& all_grids_mine_rate); //�ڴ򿪵ĸ��Ӳ�����ʱ���²�mine����͵ĸ���
};