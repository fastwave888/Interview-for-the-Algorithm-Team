#include <iostream>
#include <vector>
#include <string>
#include <cstring> 
#include <queue>
#include <utility>

/*
 	现在你有一个类MAP_BASE，请编写他的子类，继承于MAP_BASE
 	可以看到map_in是一张地图，定义.是空地，#是障碍物，现在有一个的机器人想从左上角到达右下角，请在子类中写一个函数完成这件事情
 	要求：1.将机器人中心走过的路用C表示出来并输出整个地图
		  2.机器人是四向移动的，即只能向上下左右移动
      	  3.机器人走过的路线为所有可行的路线中最短的
	
	注意：答案不唯一，只要输出一种答案就可
	加分条件：现实中机器人是有大小的，不可能完全让中心靠墙移动，现在假设机器人需要3x3的空地才能安全移动，请给出能让机器人安全移动的路径

	示例：

	#############################	#############################			#############################
	#..............###..........#	#.C............###..........#			#.C............###..........#
	#........###................#	#.C......###................#			#.C......###................#
	#.........................###	#.C.......................###			#.C.......................###	
	#.........................###	#.CCCCCCCCCCCCCCCCCC......###			#.C.......................###
	#....................########	#..................C.########			#.C..................########
	#.....#####.................#	#.....#####........C........#			#.C...#####.................#
	#...........................#	#..................CCCCCCCC.#			#.CCCCCCCCCCCCCCCCCCCCCCCCC.#
	#...........................#	#...........................#			#...........................#
	#############################中	#############################是安全的而	 #############################是不安全的因为通过了2x2的空地
	同时
	#############################	           #############################
	#.C............###..........#	           #.C............###..........#
	#.C......###................#	           #.C......###................#
	#.C.......................###	           #.C.......................###	
	#.CCCCCCCCCCCCCC..........###	           #.CCCCCCCCCCCC.CCCCC......###
	#..............C.....########	           #............C.C...C.########
	#.....#####....C............#	           #.....#####..C.C...C........#
	#..............CCCCCCCCCCCC.#	           #............CCC...CCCCCCCC.#
	#...........................#			   #...........................#
	#############################也是可行的而	#############################是不可行的因为这不是最短路

*/
class MAP_BASE
{
	public:
		MAP_BASE()
		{
			map_in = {
	        "#######################################################################",
	        "#.............................................######...........##.....#",
	        "#...........#############........................######.......#########",
	        "#...........#####...........................###...####.......##########",
	        "##.............##.............#######.................................#",
	        "###...................................................................#",
	        "#.........####................######.......................####.......#",
	        "#..........##.........#.##...................######...................#",
	        "#.....................................................................#",
	        "#.............................##..../............................#######",
	        "#.....................................................................#",
	        "##########.............#######..########......#########...............#",
	        "#...............................###############.......................#",
	        "#######################################################################"
			};
			std::memset(visit, false, sizeof(visit));
		}
		
		void print(std::vector<std::string> map_in)
		{
			for (const auto& line : map_in)
		        std::cout << line << '\n';
		}
	protected:
		std::vector<std::string> map_in;
		bool visit[100][100];
};

/*
*在map_in中将'.'替换成'C'表示机器人的路径。将最终的结果像示例里那样输出出来
*/

// 方向数组：上下左右
const int dx[] = {-1, 1, 0, 0};
const int dy[] = {0, 0, -1, 1};

class MAP_SOLVER : public MAP_BASE
{
public:
	// 基础版：BFS求最短路径，将路径标记为'C'
	void solve()
	{
		int rows = map_in.size();
		int cols = map_in[0].size();

		// 起点(1,1)，终点为右下角最后一个'.'
		int start_r = 1, start_c = 1;
		int end_r = rows - 2, end_c = cols - 2;

		// 找到终点：从右下角往回找第一个'.'
		bool found_end = false;
		for (int r = rows - 1; r >= 0 && !found_end; --r)
			for (int c = cols - 1; c >= 0 && !found_end; --c)
				if (map_in[r][c] == '.')
				{
					end_r = r;
					end_c = c;
					found_end = true;
				}

		// BFS
		std::queue<std::pair<int, int>> q;
		int parent[100][100]; // 用一维编码记录路径来源，-1表示未访问
		std::memset(parent, -1, sizeof(parent));

		q.push({start_r, start_c});
		parent[start_r][start_c] = start_r * 100 + start_c; // 起点标记为已访问

		bool reached = false;
		while (!q.empty() && !reached)
		{
			auto [r, c] = q.front();
			q.pop();

			for (int d = 0; d < 4; ++d)
			{
				int nr = r + dx[d];
				int nc = c + dy[d];

				if (nr >= 0 && nr < rows && nc >= 0 && nc < cols
					&& map_in[nr][nc] == '.' && parent[nr][nc] == -1)
				{
					parent[nr][nc] = r * 100 + c;
					q.push({nr, nc});

					if (nr == end_r && nc == end_c)
					{
						reached = true;
						break;
					}
				}
			}
		}

		if (!reached)
		{
			std::cout << "No path found!" << std::endl;
			return;
		}

		// 回溯路径，标记为'C'
		int cur_r = end_r, cur_c = end_c;
		while (cur_r != start_r || cur_c != start_c)
		{
			map_in[cur_r][cur_c] = 'C';
			int p = parent[cur_r][cur_c];
			cur_r = p / 100;
			cur_c = p % 100;
		}
		map_in[start_r][start_c] = 'C';

		print(map_in);
	}

	// 加分版：机器人需要3x3空地才能安全移动
	// 核心思路：只有当机器人中心所在位置的周围3x3区域全部为'.'时，该位置才是安全的
	void solve_safe()
	{
		int rows = map_in.size();
		int cols = map_in[0].size();

		// 预处理：标记每个位置作为机器人中心是否安全（3x3区域全为'.'）
		bool safe[100][100];
		std::memset(safe, false, sizeof(safe));

		for (int r = 1; r < rows - 1; ++r)
			for (int c = 1; c < cols - 1; ++c)
			{
				bool ok = true;
				for (int dr = -1; dr <= 1 && ok; ++dr)
					for (int dc = -1; dc <= 1 && ok; ++dc)
						if (map_in[r + dr][c + dc] != '.')
							ok = false;
				safe[r][c] = ok;
			}

		// 找到安全的起点和终点
		int start_r = -1, start_c = -1, end_r = -1, end_c = -1;
		for (int r = 1; r < rows && start_r == -1; ++r)
			for (int c = 1; c < cols && start_r == -1; ++c)
				if (safe[r][c]) { start_r = r; start_c = c; }

		for (int r = rows - 2; r >= 0 && end_r == -1; --r)
			for (int c = cols - 2; c >= 0 && end_r == -1; --c)
				if (safe[r][c]) { end_r = r; end_c = c; }

		if (start_r == -1 || end_r == -1)
		{
			std::cout << "No safe path found!" << std::endl;
			return;
		}

		// BFS on safe positions
		std::queue<std::pair<int, int>> q;
		int parent[100][100];
		std::memset(parent, -1, sizeof(parent));

		q.push({start_r, start_c});
		parent[start_r][start_c] = start_r * 100 + start_c;

		bool reached = false;
		while (!q.empty() && !reached)
		{
			auto [r, c] = q.front();
			q.pop();

			for (int d = 0; d < 4; ++d)
			{
				int nr = r + dx[d];
				int nc = c + dy[d];

				if (nr >= 0 && nr < rows && nc >= 0 && nc < cols
					&& safe[nr][nc] && parent[nr][nc] == -1)
				{
					parent[nr][nc] = r * 100 + c;
					q.push({nr, nc});

					if (nr == end_r && nc == end_c)
					{
						reached = true;
						break;
					}
				}
			}
		}

		if (!reached)
		{
			std::cout << "No safe path found!" << std::endl;
			return;
		}

		// 回溯路径，标记为'C'
		int cur_r = end_r, cur_c = end_c;
		while (cur_r != start_r || cur_c != start_c)
		{
			map_in[cur_r][cur_c] = 'C';
			int p = parent[cur_r][cur_c];
			cur_r = p / 100;
			cur_c = p % 100;
		}
		map_in[start_r][start_c] = 'C';

		print(map_in);
	}
};

int main()
{
	std::cout << "=== 基础版（最短路径）===" << std::endl;
	MAP_SOLVER solver;
	solver.solve();

	std::cout << std::endl;
	std::cout << "=== 加分版（3x3安全路径）===" << std::endl;
	MAP_SOLVER solver2;
	solver2.solve_safe();

	return 0;
}