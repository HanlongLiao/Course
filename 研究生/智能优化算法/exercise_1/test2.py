from itertools import combinations
import random as r
import numpy as np

# 计算目标函数:距离
def get_tour_length(sol, distance):
    tour = sol.copy()
    n = len(tour)
    length = 0
    tour.append(tour[0])
    for k in range(n):
        i = tour[k]
        j = tour[k + 1]
        length += distance[i, j]
    return length

# 计算距离矩阵
def getDistance(x, y):
    n = len(x)
    distance = np.zeros((n, n))
    for i in range(n - 1):
        for j in range(i, n):
            distance[i, j] = np.sqrt(pow((x[i] - x[j]), 2) + pow((y[i] - y[j]), 2))
    distance += distance.T - np.diag(distance.diagonal())
    return distance

# 请在此处添加代码，实现目标函数功能
#********** Begin **********#

def getActionlist(n):  # 得到操作列表
    action_list = list()
    for i in range(n-1):
        for j in range(i+1, n):
            action_list.append([i, j])
    return action_list


def getBestImprovement(sol, action_list, distance):  # 得到改进解
    current_sol = sol.copy()
    current_len = get_tour_length(current_sol, distance)
    while True:

        # 搜索解领域
        current_best_len = float('inf')
        current_best_sol = list()
        for action in action_list:
            sol_new = current_sol.copy()
            i, j = action[0], action[1]
            sol_new[i], sol_new[j] = sol_new[j], sol_new[i]
            tour_len = get_tour_length(sol_new, distance)
            if tour_len < current_best_len:
                current_best_len = tour_len
                current_best_sol = sol_new.copy()
        
        if current_best_len <= current_len:
            current_sol = current_best_sol.copy()
            current_len = current_best_len
        else:
            break
    sol = current_sol
    return sol

#*********** End ***********#

x = [82, 91, 12, 92, 63, 9, 28, 55, 96, 97, 15, 98]  # 城市坐标
y = [14, 42, 92, 80, 96, 66, 3, 85, 94, 68, 76, 75]

sol = [11, 9, 10, 5, 2, 6, 0, 1, 7, 4, 8, 3]  # 当前解
distance = getDistance(x, y)

n = len(x) # 城市总数
# 测试函数
action_list = getActionlist(n)  # 格式为[(x1,x2),(x1,x3),...]
# print(action_list)
new_sol = getBestImprovement(sol, action_list, distance)

# 输出结果
print(new_sol)
