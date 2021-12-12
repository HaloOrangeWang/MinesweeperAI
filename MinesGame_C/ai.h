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

//对地图进行分治处理的结果
struct Equations
{
	int num_grid; //一共有多少个格子
	map<pair<int, int>, int> grid_indexes; //每个格子的x、y与它们在结构体索引值之间的对应关系
	vector<pair<int, int>> grid_list; //索引值与格子的x、y的反向对应关系
	vector<vector<int>> params; //方程组的参数
	vector<int> results; //方程组的结果
};

struct SolveResult
{
	bool have_content;
	vector<int> right_click_dx_list; //必须插旗的格子列表
	vector<int> left_click_dx_list; //必须点开的格子列表
};

struct MinesGame2
{
	int8_t skip_grids[MAP_X][MAP_Y];
	int mines_left; //剩余mines的数量。用于确定是否根据剩余mines启用新的方程组
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
	bool choice_single(bool img, MinesGame2& g2);  //V1版本的简单逻辑
	Equations create_equations(bool img, MinesGame2& g2); //构建当前地图对应的方程组
	vector<set<int>> divide_equations(int num_grid, int num_equation, unique_ptr<int[]>& all_params); //将这些方程组分割成若干个互不相关的小方程组
	vector<unique_ptr<int[]>> gen_combinations(int num_grid, vector<int>& new_grid_dx_list, unique_ptr<int[]>& base_params, int curr_pos);
	unique_ptr<double[]> get_mine_rate_by_grid(bool img, MinesGame2& g2, vector<pair<int, int>>& grid_list, int num_grid, unique_ptr<int[]>& all_params, vector<int>& all_results, vector<int>& usable_equas_dx_list); //计算每个格子是mine的概率
	bool choice_by_equation(bool img, MinesGame2& g2, map<pair<int, int>, double>& all_grids_mine_rate); //通过解方程组来获取更加精确的判断
	void guess_1grid(map<pair<int, int>, double>& all_grids_mine_rate); //当解方程组解决不了问题时，猜测一个格子
	MinesGame2 cur_g;
	std::random_device rd;
public:
	void store_legal_combinations(int num_grid, vector<pair<int, int>>& grid_list, vector<unique_ptr<int[]>>& possible_mines_combination_list, vector<bool>& legal_combination_dx_list); //记录哪些点是合理的组合
	map<pair<int, int>, double> calc_next_step_by_grid(); //计算点开每个格子后，下一状态的期望
	const int PredictGridNum = 16; //当剩余格子数量小于这个数字时，就进行状态预测。最大取值18
	const int GlCalcMinesLeft = 9; //当剩余mines数量小于这个数字时，根据总mines数量额外计算。最大取值9
	vector<vector<pair<int, int>>> all_legal_combinations; //记录当前状态下哪些格子为mine的全部组合。仅在剩余格子较少时，这个变量的内容才是正确的。
	vector<pair<int, int>> unknown_grid_list; //剩余全部未打开的格子列表。仅在剩余格子较少时，这个变量的内容才是正确的。
public:
	pair<int, int> guess_1grid_by_mine_rate(map<pair<int, int>, double>& all_grids_mine_rate); //在打开的格子不够多时，猜测mine率最低的格子
};