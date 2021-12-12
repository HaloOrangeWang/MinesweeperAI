#include "ai.h"
#include <algorithm>
#include <functional>

AI_V6::AI_V6() :cur_g(false)
{
	for (int x = 0; x <= MAP_X - 1; x++) {
		for (int y = 0; y <= MAP_Y - 1; y++) {
			cur_g.skip_grids[x][y] = 0;
		}
	}
}

bool AI_V6::choice_single(bool img, MinesGame2& g2)
{
	for (int x = 0; x <= MAP_X - 1; x++) {
		for (int y = 0; y <= MAP_Y - 1; y++) {
			if (g2.g.observed_map[x][y] >= 1 && g2.g.observed_map[x][y] <= 8 && g2.skip_grids[x][y] == 0) {
				int num_flags_around = 0;
				int num_observe_around = 0;
				int num_unknown_around = 0;
				// 1.�����Χ�������ӡ����򿪡�δ���򿪵ĸ�������
				for (int tx = max(0, x - 1); tx <= min(MAP_X - 1, x + 1); tx++) {
					for (int ty = max(0, y - 1); ty <= min(MAP_Y - 1, y + 1); ty++) {
						if (tx != x || ty != y) {
							if (g2.g.observed_map[tx][ty] >= 0 && g2.g.observed_map[tx][ty] <= 8)
								num_observe_around += 1;
							else if (g2.g.observed_map[tx][ty] == 9)
								num_flags_around += 1;
							else
								num_unknown_around += 1;
						}
					}
				}
				if (g2.g.observed_map[x][y] != -1 && num_unknown_around == 0) { //������Ӻ���Χ�ĸ��Ӷ��Ѿ�̽�����ˣ�û��Ҫ����̽������
					g2.skip_grids[x][y] = 1;
					continue;
				}
				// 2.������ڡ����ӱ��� = ��Χ�����������ĸ��ӣ���ֱ��˫����
				if (num_flags_around == g2.g.observed_map[x][y]) {
					g2.g.double_click(x, y);
					return true;
				}
				// 3.������ڡ����ӱ���=��Χ��������+��Χδ̽�������������ĸ��ӣ���ֱ�Ӱ����������Χ�����и��Ӷ���������
				if (num_flags_around + num_unknown_around == g2.g.observed_map[x][y]) {
					g2.g.right_click_around(x, y);
					return true;
				}
			}
		}
	}
	return false;
}

Equations AI_V6::create_equations(bool img, MinesGame2& g2)
{
	Equations equas;
	equas.num_grid = 0;
	for (int x = 0; x <= MAP_X - 1; x++) {
		for (int y = 0; y <= MAP_Y - 1; y++) {
			//1.���ж���������Ƿ�Ϊ��Ե��
			if (g2.g.observed_map[x][y] <= 0 || g2.g.observed_map[x][y] == 9 || g2.skip_grids[x][y] == 1) {
				continue;
			}
			//2.��ȡ���������Χ��δ�򿪵ĸ��ӣ�����Щ�������뵽��������
			vector<pair<int, int>> grids_around;
			int num_mines_left_around = g2.g.observed_map[x][y]; //���������Χ���˲������ӵĸ������⣬��Ӧ���ж��ٸ�mine
			for (int tx = max(0, x - 1); tx <= min(MAP_X - 1, x + 1); tx++) {
				for (int ty = max(0, y - 1); ty <= min(MAP_Y - 1, y + 1); ty++) {
					if (tx != x || ty != y) {
						if (g2.g.observed_map[tx][ty] == -1)
							grids_around.push_back(make_pair(tx, ty));
						else if (g2.g.observed_map[tx][ty] == 9)
							num_mines_left_around -= 1;
					}
				}
			}
			if (grids_around.size() >= 1) {
				equas.results.push_back(num_mines_left_around);
				vector<int> grid_index_1equation;
				for (int grid_dx = 0; grid_dx <= grids_around.size() - 1; grid_dx++) {
					int grid_index;
					if (equas.grid_indexes.find(grids_around[grid_dx]) == equas.grid_indexes.end()) {
						grid_index = equas.num_grid;
						equas.grid_indexes.insert(make_pair(grids_around[grid_dx], grid_index));
						equas.grid_list.push_back(grids_around[grid_dx]);
						equas.num_grid++;
					}
					else {
						grid_index = equas.grid_indexes[grids_around[grid_dx]];
					}
					grid_index_1equation.push_back(grid_index);
				}
				equas.params.push_back(grid_index_1equation);
			}
		}
	}
	// 3.���ʣ��mines������������9�����������mines��������һ������
	// �ȸ���ʣ�����������ʣ��mines�����������Ƿ�Ҫ�洢ʣ��Ŀ������
	unknown_grid_list.clear();
	for (int x = 0; x <= MAP_X - 1; x++) {
		for (int y = 0; y <= MAP_Y - 1; y++) {
			if (cur_g.g.observed_map[x][y] == -1) {
				unknown_grid_list.push_back(make_pair(x, y));
			}
		}
	}
	if (g2.mines_left <= GlCalcMinesLeft || unknown_grid_list.size() <= 30) {
		equas.results.push_back(g2.mines_left);
		vector<int> grid_index_1equation;
		for (int x = 0; x <= MAP_X - 1; x++) {
			for (int y = 0; y <= MAP_Y - 1; y++) {
				if (g2.g.observed_map[x][y] == -1) {
					pair<int, int> grid = make_pair(x, y);
					int grid_index;
					if (equas.grid_indexes.find(grid) == equas.grid_indexes.end()) {
						grid_index = equas.num_grid;
						equas.grid_indexes.insert(make_pair(grid, grid_index));
						equas.grid_list.push_back(grid);
						equas.num_grid++;
					}
					else {
						grid_index = equas.grid_indexes[grid];
					}
					grid_index_1equation.push_back(grid_index);
				}
			}
		}
		equas.params.push_back(grid_index_1equation);
	}
	return equas;
}

vector<set<int>> AI_V6::divide_equations(int num_grid, int num_equation, unique_ptr<int[]>& all_params)
{
	vector<set<int>> divide_res;
	for (int grid_it = 0; grid_it <= num_grid - 1; grid_it++) {
		//1.��ȡ����������ӵķ����б�
		set<int> equas_dx_list;
		for (int t0 = 0; t0 <= num_equation - 1; t0++) {
			if (all_params[t0 * num_grid + grid_it] != 0)
				equas_dx_list.insert(t0);
		}
		//2.����������ӵ������ķ�����Ϣ�������ӵ���ֺ�ķ����б���
		if (divide_res.size() == 0)
			divide_res.push_back(equas_dx_list);
		else {
			vector<int> partition_dx_list; //��¼�ĸ����̷����а����������
			for (int t0 = 0; t0 <= divide_res.size() - 1; t0++) {
				for (set<int>::iterator t1 = equas_dx_list.begin(); t1 != equas_dx_list.end(); t1++) {
					if (find(divide_res[t0].begin(), divide_res[t0].end(), *t1) != divide_res[t0].end()) {
						if (find(partition_dx_list.begin(), partition_dx_list.end(), t0) == partition_dx_list.end())
							partition_dx_list.push_back(t0);
					}
				}
			}
			if (partition_dx_list.size() == 0)
				divide_res.push_back(equas_dx_list); //����������ӵ��⼸������Ŀǰ�������κ�һ���������򴴽�һ���µķ���
			else if (partition_dx_list.size() == 1) { //����������ӵļ�������ֻ����һ������������Щ���̷������������
				int partition_dx = partition_dx_list[0];
				divide_res[partition_dx].insert(equas_dx_list.begin(), equas_dx_list.end());
			}
			else {
				// ����������ӵļ�������ͬʱ���ڶ�����������Ƚ���Щ���̷����һ�������ڣ��ٽ��⼸�������ϲ���һ�������
				int first_partition_dx = partition_dx_list[0];
				divide_res[first_partition_dx].insert(equas_dx_list.begin(), equas_dx_list.end());
				for (int t0 = 1; t0 <= partition_dx_list.size() - 1; t0++) {
					divide_res[first_partition_dx].insert(divide_res[partition_dx_list[t0]].begin(), divide_res[partition_dx_list[t0]].end());
					divide_res[partition_dx_list[t0]].clear();
				}
			}
		}
	}
	//ȥ����û���κη��̵ķ���
	vector<set<int>> divide_res2;
	for (int t = 0; t < divide_res.size(); t++) {
		if (!divide_res[t].empty())
			divide_res2.push_back(divide_res[t]);
	}
	return divide_res2;
}

vector<unique_ptr<int[]>> AI_V6::gen_combinations(int num_grid, vector<int>& new_grid_dx_list, unique_ptr<int[]>& base_params, int num_new_mines)
{
	vector<unique_ptr<int[]>> res;
	if (num_new_mines == 0) {
		unique_ptr<int[]> b0(new int[num_grid]);
		for (int t = 0; t <= num_grid - 1; t++)
			b0[t] = base_params[t];
		for (int t = 0; t <= new_grid_dx_list.size() - 1; t++) {
			b0[new_grid_dx_list[t]] = 0;
		}
		res.push_back(move(b0));
		return res;
	}
	vector<int> curr_pos_list;
	unique_ptr<int[]> grid_dx_in_cur_pos_list(new int[new_grid_dx_list.size()]);
	for (int t = 0; t <= num_new_mines - 1; t++) {
		curr_pos_list.push_back(t);
		grid_dx_in_cur_pos_list[t] = 1;
	}
	for (int t = num_new_mines; t <= new_grid_dx_list.size() - 1; t++)
		grid_dx_in_cur_pos_list[t] = 0;
	while (true) {
		unique_ptr<int[]> b0(new int[num_grid]);
		for (int t = 0; t <= num_grid - 1; t++)
			b0[t] = base_params[t];
		for (int t = 0; t <= new_grid_dx_list.size() - 1; t++) {
			b0[new_grid_dx_list[t]] = grid_dx_in_cur_pos_list[t];
		}
		res.push_back(move(b0));
		int latter_grid_dx = new_grid_dx_list.size();
		for (int t = curr_pos_list.size() - 1; t >= 0; t--) {
			if (curr_pos_list[t] != latter_grid_dx - 1) {
				grid_dx_in_cur_pos_list[curr_pos_list[t]] = 0;
				curr_pos_list[t]++;
				grid_dx_in_cur_pos_list[curr_pos_list[t]] = 1;
				for (int t0 = t + 1; t0 <= curr_pos_list.size() - 1; t0++) {
					grid_dx_in_cur_pos_list[curr_pos_list[t0]] = 0;
					curr_pos_list[t0] = curr_pos_list[t] + t0 - t;
					grid_dx_in_cur_pos_list[curr_pos_list[t0]] = 1;
				}
				break;
			}
			else if (t == 0) {
				return res;
			}
			else {
				latter_grid_dx = curr_pos_list[t];
			}
		}
	}
}

unique_ptr<double[]> AI_V6::get_mine_rate_by_grid(bool img, MinesGame2& g2, vector<pair<int, int>>& grid_list, int num_grid, unique_ptr<int[]>& all_params, vector<int>& all_results, vector<int>& usable_equas_dx_list)
{
	vector<unique_ptr<int[]>> possible_mines_combination_list;  //�洢���п��е�mines��ϡ��Ǹ���ά���顣��ֵ����Ϊ0��ʾ���������������Ӳ�Ϊmine��1��ʾΪmine��-1��ʾ��ȷ��
	vector<bool> legal_combination_dx_list; //��һ�������ò�����֮�󣬽��������Ķ�Ӧλ�ø�д��0�������erase
	unique_ptr<int[]> checked_grid_dx_list(new int[num_grid]); //����ÿ��grid�Ƿ��Ѽ�����1��ʾ�Ѽ�����0��ʾ��δ������
	int equa_dx;
	for (int t = 0; t <= num_grid - 1; t++)
		checked_grid_dx_list[t] = 0;
	//�ȸ��ݵ�һ�����̣���ʼ�����е�mines��ϡ�
	equa_dx = usable_equas_dx_list[0];
	vector<int> new_grid_dx_list;
	for (int grid_it = 0; grid_it <= num_grid - 1; grid_it++) {
		if (all_params[equa_dx * num_grid + grid_it]) {
			new_grid_dx_list.push_back(grid_it);
			checked_grid_dx_list[grid_it] = 1;
		}
	}
	unique_ptr<int[]> base_params(new int[num_grid]);
	for (int t = 0; t <= num_grid - 1; t++) {
		base_params[t] = -1;
	}
	possible_mines_combination_list = gen_combinations(num_grid, new_grid_dx_list, base_params, all_results[equa_dx]);
	for (int t = 0; t <= possible_mines_combination_list.size() - 1; t++) {
		legal_combination_dx_list.push_back(true);
	}
	//�ٸ����������̽��е���
	for (int equa_dx_it = 1; equa_dx_it <= usable_equas_dx_list.size() - 1; equa_dx_it++) {
		equa_dx = usable_equas_dx_list[equa_dx_it];
		//��һ�������������ǰ�����У����е�mines��ϳ�����1000000����ǿ�Ʒ��ز��ɽ�
		if (possible_mines_combination_list.size() >= 1000000) {
			printf("���еĽ����������������1000000��ǿ�Ʒ��ز��ɽ�\n");
			break;
			//abort();
		}
		//1.�ȼ����Ϊ����������µķ��̣�����Ҫ�¼����Щgrid
		vector<int> new_grid_dx_list;
		for (int grid_it = 0; grid_it <= num_grid - 1; grid_it++) {
			if (all_params[equa_dx * num_grid + grid_it] && checked_grid_dx_list[grid_it] == 0) {
				new_grid_dx_list.push_back(grid_it);
				checked_grid_dx_list[grid_it] = true;
			}
		}
		//2.�ֱ��ÿһ�ֿ���������ж�����mines����������grid����֮��Ĺ�ϵ
		int num_mines_combinations_before = possible_mines_combination_list.size();
		for (int t = 0; t <= num_mines_combinations_before - 1; t++) {
			if (legal_combination_dx_list[t] == true) {
				int num_new_mines = all_results[equa_dx];
				for (int grid_it = 0; grid_it <= num_grid - 1; grid_it++) {
					if (all_params[equa_dx * num_grid + grid_it] && possible_mines_combination_list[t][grid_it] == 1)
						num_new_mines--;
				}
				if (num_new_mines < 0 || num_new_mines > new_grid_dx_list.size()) {
					// ��������С��0���򳬹�������grid�������������һ���ǲ������
					legal_combination_dx_list[t] = false;
				}
				else if (num_new_mines == 0 && new_grid_dx_list.size() == 0) {
					// ������mines������grid������Ϊ0��˵��������̲�δ�����κ����õ���Ϣ��ֱ�Ӻ��Ե�
					continue;
				}
				else if (num_new_mines == 0 && new_grid_dx_list.size() >= 1) {
					// ������grid��û������mines����ô������gridһ������Ϊmine
					for (int grid_dx_it = 0; grid_dx_it <= new_grid_dx_list.size() - 1; grid_dx_it++) {
						int grid_dx = new_grid_dx_list[grid_dx_it];
						possible_mines_combination_list[t][grid_dx] = 0;
					}
				}
				else if (num_new_mines == new_grid_dx_list.size() && new_grid_dx_list.size() >= 1) {
					// ������grid������������mines��������ô������gridһ��Ϊmine
					for (int grid_dx_it = 0; grid_dx_it <= new_grid_dx_list.size() - 1; grid_dx_it++) {
						int grid_dx = new_grid_dx_list[grid_dx_it];
						possible_mines_combination_list[t][grid_dx] = 1;
					}
				}
				else {
					// 0<����mines����<����grid���������������Щ�������ö�١���mines�����������ϼ��뵽����б���
					vector<unique_ptr<int[]>> new_mines_combination_list = gen_combinations(num_grid, new_grid_dx_list, possible_mines_combination_list[t], num_new_mines);
					for (int t0 = 0; t0 <= new_mines_combination_list.size() - 1; t0++) {
						possible_mines_combination_list.push_back(move(new_mines_combination_list[t0]));
						legal_combination_dx_list.push_back(true);
					}
					//ö�ٳ��µĿ���mines���֮�󣬶�Ӧ�ľɵ�mines��Ͼ��Զ�ʧЧ��
					legal_combination_dx_list[t] = false;
				}
			}
		}
	}
	//���������п��еļ��裬����������ÿ��������mine�ĸ��ʡ�-1��ʾ�������û�б�����
	unique_ptr<double[]> mine_rate_by_grid(new double[num_grid]);
	int num_mines_combinations = 0;
	for (int t = 0; t <= possible_mines_combination_list.size() - 1; t++) {
		if (legal_combination_dx_list[t] == true) {
			int num_hyp_mines = 0;
			for (int grid_it = 0; grid_it <= num_grid - 1; grid_it++) {
				if (checked_grid_dx_list[grid_it] == true) {
					if (possible_mines_combination_list[t][grid_it] == 1)
						num_hyp_mines++;
				}
			}
			if (num_hyp_mines > g2.mines_left)
				legal_combination_dx_list[t] = false;
			else
				num_mines_combinations++;
		}
	}
	for (int grid_it = 0; grid_it <= num_grid - 1; grid_it++) {
		if (checked_grid_dx_list[grid_it] == true) {
			int num_hyp_mines = 0;
			for (int t = 0; t <= possible_mines_combination_list.size() - 1; t++) {
				if (legal_combination_dx_list[t] == true) {
					if (possible_mines_combination_list[t][grid_it] == 1)
						num_hyp_mines++;
				}
			}
			mine_rate_by_grid[grid_it] = num_hyp_mines * 1.0 / num_mines_combinations;
		}
		else {
			mine_rate_by_grid[grid_it] = -1;
		}
	}
	//���ʣ��ĸ����������٣�ʣ��mines����Ҳ���٣�������ʽ����ģʽ�������к����mine��϶���¼����
	if (!img)
		store_legal_combinations(num_grid, grid_list, possible_mines_combination_list, legal_combination_dx_list);
	return mine_rate_by_grid;
}

void AI_V6::store_legal_combinations(int num_grid, vector<pair<int, int>>& grid_list, vector<unique_ptr<int[]>>& possible_mines_combination_list, vector<bool>& legal_combination_dx_list)
{
	if (unknown_grid_list.size() > PredictGridNum || cur_g.mines_left > GlCalcMinesLeft)
		return;
	// Ȼ����Щ��ϱ�������
	all_legal_combinations.clear();
	for (int t = 0; t <= possible_mines_combination_list.size() - 1; t++) {
		if (legal_combination_dx_list[t] == true) {
			vector<pair<int, int>> mine_list;
			mine_list.reserve(cur_g.mines_left);
			for (int grid_it = 0; grid_it <= num_grid - 1; grid_it++) {
				if (possible_mines_combination_list[t][grid_it] == 1) {
					mine_list.push_back(grid_list[grid_it]);
				}
			}
			all_legal_combinations.push_back(mine_list);
		}
	}
}

bool AI_V6::choice_by_equation(bool img, MinesGame2& g2, map<pair<int, int>, double>& all_grids_mine_rate)
{
	bool have_choice = false;
	//1.�ȹ���������
	Equations equas = create_equations(img, g2);
	if (equas.num_grid == 0)
		return false;
	unique_ptr<int[]> param_list(new int[equas.num_grid * equas.results.size()]); //ת��Ϊ������ʽ�ĺ�������
	for (int t = 0; t <= equas.num_grid * equas.results.size() - 1; t++)
		param_list[t] = 0;
	for (int t = 0; t <= equas.results.size() - 1; t++) {
		for (int t0 = 0; t0 <= equas.params[t].size() - 1; t0++) {
			param_list[t * equas.num_grid + equas.params[t][t0]] = 1;
		}
	}
	//2.����Щ������ָ�����ɻ�����صķ�����
	vector<set<int>> equa_partitions = divide_equations(equas.num_grid, equas.results.size(), param_list);
	//3.���ÿһ��������������ж��ܷ���ݷ��������������еĸ����Ƿ�Ϊmine
	for (int t = 0; t <= equa_partitions.size() - 1; t++) {
		// �Ȼ�ȡ����������ķ������б�
		vector<int> usable_equas_dx_list;
		for (set<int>::iterator equa_it = equa_partitions[t].begin(); equa_it != equa_partitions[t].end(); equa_it++)
			usable_equas_dx_list.push_back(*equa_it);
		// �ӷ������л�ȡ����Щ������mine�ĸ���
		unique_ptr<double[]> mine_rate = get_mine_rate_by_grid(img, g2, equas.grid_list, equas.num_grid, param_list, equas.results, usable_equas_dx_list);
		for (int grid_it = 0; grid_it <= equas.num_grid - 1; grid_it++) {
			if (mine_rate[grid_it] != -1)
				all_grids_mine_rate.insert(make_pair(equas.grid_list[grid_it], mine_rate[grid_it]));
		}
		// �����һ��������mine�ĸ���Ϊ0��1����ֱ��������Ҽ�����������
		for (map<pair<int, int>, double>::iterator _it = all_grids_mine_rate.begin(); _it != all_grids_mine_rate.end(); _it++) {
			if (_it->second == 1) {
				// ��Ȼ��mine�ĵ�
				int grid_x = _it->first.first;
				int grid_y = _it->first.second;
				if (g2.g.status == 0 && g2.g.observed_map[grid_x][grid_y] == -1)
					g2.g.right_click(grid_x, grid_y);
				have_choice = true;
			}
			if (_it->second == 0) {
				// ��Ȼ����mine�ĵ�
				int grid_x = _it->first.first;
				int grid_y = _it->first.second;
				if (g2.g.status == 0 && g2.g.observed_map[grid_x][grid_y] == -1)
					g2.g.left_click(grid_x, grid_y);
				have_choice = true;
			}
		}
	}
	return have_choice;
}

void AI_V6::guess_1grid(map<pair<int, int>, double>& all_grids_mine_rate)
{
	if (all_legal_combinations.size() >= 1) {
		//ʣ��δ֪������������ʱ���²�ر���ߵĸ���
		map<pair<int, int>, double> reward_by_grid = calc_next_step_by_grid();
		double max_reward = -0.1;
		pair<int, int> grid_with_max_reward;
		for (map<pair<int, int>, double>::iterator _it = reward_by_grid.begin(); _it != reward_by_grid.end(); _it++) {
			if (_it->second > max_reward) {
				max_reward = _it->second;
				grid_with_max_reward = _it->first;
			}
		}
		//�²����mine����͵ĸ���
		cur_g.g.left_click(grid_with_max_reward.first, grid_with_max_reward.second);
	}
	else {
		//ʣ��δ֪���������϶�ʱ���²�mine����͵ĸ���
		//���ҵ�mine����͵ĸ���
		pair<int, int> grid_with_min_mine_rate = guess_1grid_by_mine_rate(all_grids_mine_rate);
		//�²����mine����͵ĸ���
		cur_g.g.left_click(grid_with_min_mine_rate.first, grid_with_min_mine_rate.second);
	}
}

pair<int, int> AI_V6::guess_1grid_by_mine_rate(map<pair<int, int>, double>& all_grids_mine_rate)
{
	// 1.�ȼ���ÿ�����ӵ�mine�ʡ���������Ѿ����ˣ���mine��Ϊ-1.
	int num_grid_open = 0; //�Ѿ����˶��ٸ�����
	int num_grid_have_ratio = 0; //�ж��ٸ���������all_grids_mine_rate�е�
	double sum_mine_rate = 0; //all_grids_mine_rate�����и��ӵ�mine��֮��
	double mine_ratio_by_grid[MAP_X][MAP_Y];
	for (int x = 0; x <= MAP_X - 1; x++) {
		for (int y = 0; y <= MAP_Y - 1; y++) {
			pair<int, int> point = make_pair(x, y);
			if (cur_g.g.observed_map[x][y] != -1) { //�����Ѿ�����
				num_grid_open += 1;
				mine_ratio_by_grid[x][y] = -1;
			}
			else if (all_grids_mine_rate.find(point) != all_grids_mine_rate.end()) { //���ӵ�mine���Ѿ��������
				num_grid_have_ratio += 1;
				mine_ratio_by_grid[x][y] = all_grids_mine_rate[point];
				sum_mine_rate += all_grids_mine_rate[point];
			}
			else {  //���ӵĸ���û�м����
				mine_ratio_by_grid[x][y] = -2; //��д��-2�������ٸ�
			}
		}
	}
	double default_mine_rate = (cur_g.mines_left - sum_mine_rate) * 1.0 / (MAP_X * MAP_Y - num_grid_open - num_grid_have_ratio);
	for (int x = 0; x <= MAP_X - 1; x++) {
		for (int y = 0; y <= MAP_Y - 1; y++) {
			if (mine_ratio_by_grid[x][y] == -2)
				mine_ratio_by_grid[x][y] = default_mine_rate;
		}
	}
	//2.�ҵ�mine����͵ĸ���
	double min_mine_rate = 1;
	pair<int, int> grid_with_min_mine_rate;
	for (int x = 0; x <= MAP_X - 1; x++) {
		for (int y = 0; y <= MAP_Y - 1; y++) {
			if (mine_ratio_by_grid[x][y] != -1) {
				if (mine_ratio_by_grid[x][y] < min_mine_rate) {
					min_mine_rate = mine_ratio_by_grid[x][y];
					grid_with_min_mine_rate = make_pair(x, y);
				}
			}
		}
	}
	return grid_with_min_mine_rate;
}

map<pair<int, int>, double> AI_V6::calc_next_step_by_grid()
{
	//cur_g.g.show();
	map<pair<int, int>, double> reward_by_grid; //��¼�ڴ˽׶��£����ÿ�����ӵ�����
	//����ÿһ��δ֪�ĸ��ӣ����ݵ�ǰ״̬��Ԥ�������һ���²��ܹ��򿪶��ٸ�����
	for (int grid_it = 0; grid_it <= unknown_grid_list.size() - 1; grid_it++) {
		vector<double> reward_list;  //��¼����ÿһ����������ܲ����������б����ֱ�ӻ�ʤ�ˣ�������Ϊ1��ֱ��ʧ�ܵ�����Ϊ0���������Ϊ (�򿪵ĸ�������)/(�򿪵ĸ�������+1)
		reward_list.reserve(all_legal_combinations.size());
		for (int t = 0; t <= all_legal_combinations.size() - 1; t++) {
			if (find(all_legal_combinations[t].begin(), all_legal_combinations[t].end(), unknown_grid_list[grid_it]) != all_legal_combinations[t].end())
				reward_list.push_back(0);  //��һ����mine���������ô�²�������ӿ϶��ᵼ��ʧ��
			else {
				// ����һ������ĵ�ͼ���ڵ�ǰ��ͼ�Ļ����ϣ������������
				MinesGame2 g2(true);
				memcpy(g2.skip_grids, cur_g.skip_grids, sizeof(int8_t) * MAP_X * MAP_Y);
				g2.mines_left = cur_g.mines_left - 1;
				memset(g2.g.g_map, 0, sizeof(int8_t) * MAP_X * MAP_Y);
				memcpy(g2.g.observed_map, cur_g.g.observed_map, sizeof(int8_t) * MAP_X * MAP_Y);
				for (int x = 0; x <= MAP_X - 1; x++) {
					for (int y = 0; y <= MAP_Y - 1; y++) {
						if (cur_g.g.observed_map[x][y] == 9)
							g2.g.g_map[x][y] = 1;
					}
				}
				for (int t0 = 0; t0 <= all_legal_combinations[t].size() - 1; t0++)
					g2.g.g_map[all_legal_combinations[t][t0].first][all_legal_combinations[t][t0].second] = 1;
				g2.g.status = 0;
				// ����������ĵ�ͼ�ϣ��������
				g2.g.left_click(unknown_grid_list[grid_it].first, unknown_grid_list[grid_it].second);
				if (g2.g.status == 2) {
					reward_list.push_back(1);
					continue;
				}
				// ���ʣ������������ݣ�ֱ����ʤ����Ҫ�����²�λ��
				while (true) {
					g2.mines_left = MINES;
					for (int x = 0; x <= MAP_X - 1; x++) {
						for (int y = 0; y <= MAP_Y - 1; y++) {
							if (g2.g.observed_map[x][y] == 9) {
								g2.mines_left -= 1;
							}
						}
					}
					bool have_operate = choice_single(true, g2);
					if (!have_operate) {
						map<pair<int, int>, double> all_grids_mine_rate;
						have_operate = choice_by_equation(true, g2, all_grids_mine_rate);
					}
					if (g2.g.status == 2) {
						reward_list.push_back(1);
						break;
					}
					else if (g2.g.status == 1) {
						printf("�������ǲ����������.\n");
						abort();
					}
					else if (have_operate == false) {
						// ������ȥ�ˣ���ʱ���������ͼ��δ�򿪵ĸ�����������������
						int num_unknown_grid_after = 0;
						for (int x = 0; x <= MAP_X - 1; x++) {
							for (int y = 0; y <= MAP_Y - 1; y++) {
								if (g2.g.observed_map[x][y] == -1) {
									num_unknown_grid_after += 1;
								}
							}
						}
						int num_grid_open_add = unknown_grid_list.size() - num_unknown_grid_after;
						double reward = num_grid_open_add * 1.0 / (num_grid_open_add + 2);
						//double reward = 1 + (num_grid_open_add - 1) / 2.f;
						reward_list.push_back(reward);
						break;
					}
				}
			}
		}
		// ����ƽ������
		double avr_reward = 0;
		for (int t = 0; t <= reward_list.size() - 1; t++)
			avr_reward += reward_list[t];
		avr_reward /= reward_list.size();
		reward_by_grid.insert(make_pair(unknown_grid_list[grid_it], avr_reward));
	}
	return reward_by_grid;
}

void AI_V6::run_1step()
{
	// 0.�ȼ�鵱ǰʣ��mines����
	cur_g.mines_left = MINES;
	for (int x = 0; x <= MAP_X - 1; x++) {
		for (int y = 0; y <= MAP_Y - 1; y++) {
			if (cur_g.g.observed_map[x][y] == 9) {
				cur_g.mines_left -= 1;
			}
		}
	}
	// 1.�ȳ����ü򵥷����ж�
	bool have_operate = choice_single(false, cur_g);
	if (have_operate)
		return;
	// 2.���Խ����߼�����
	map<pair<int, int>, double> all_grids_mine_rate;
	have_operate = choice_by_equation(false, cur_g, all_grids_mine_rate);
	if (have_operate)
		return;
	// 3.�Ҳ���ȷ���ĸ��ӣ������²�mine������͵ĸ���
	guess_1grid(all_grids_mine_rate);
}

bool AI_V6::run()
{
	cur_g.g.set_initial_map(int(MAP_X / 2), int(MAP_Y / 2));
	//cur_g.g.show();
	while (cur_g.g.status == 0) {
		run_1step();
		//if (cur_g.g.status == 0)
		//	cur_g.g.show();
		//_sleep(100);
		if (MODE == 1)
			_sleep(REAL_MAP_SLEEP);
	}
	if (cur_g.g.status == 1)
		return false;
	else
		return true;
}
