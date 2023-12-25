#ifndef EDITORS_HPP
#define EDITORS_HPP 1

#include "validation.hpp"
#include <cstring>
#include <fstream>
#include <windows.h>
#include <iostream>
#include <limits>
#include <map>

// 编辑配置文件
void configEditor();

// 编辑地图
void mapEditor();

// 用于在编辑地图时输出地图
void mapEditorPrt(char[][25], int, int, int[], const std::string[]);

void configEditor()
{
    std::string name;
    name = getName("config"); // 获得合法配置名
    if (name == "q")
        return;
    std::ofstream fout("config//" + name + ".config");
    int difficulty, randseed, foodNum;
    float foodChance[3];
    do
    {
        std::cin >> difficulty >> randseed >> foodNum >> foodChance[0] >> foodChance[1] >> foodChance[2];
        if (typeCheck()) // 是否出现输入类型错误
            continue;
    } while (!cfgContentCheck(difficulty, randseed, foodNum, foodChance)); // 若内容不正确，重新输入
    fout << difficulty << std::endl
         << randseed << std::endl
         << foodNum << std::endl
         << foodChance[0] << ' ' << foodChance[1] << ' ' << foodChance[2];
    fout.close();
    return;
}

void mapEditor()
{
    const std::string WALL_COLOR[2] = {"\033[0;32m", "\033[0;31m"};                                      // 墙颜色：绿、红
    const std::map<int, int> KEY_TO_WALL = {{int('w'), 0}, {int('a'), 1}, {int('s'), 2}, {int('d'), 3}}; // 按键与墙序号转换

    char map[25][25];
    memset(map, ' ', sizeof(map)); // 地图初始化
    std::string name;
    name = getName("maps"); // 获得合法地图名
    if (name == "q")
        return;
    std::ofstream fout("maps//" + name + ".map");
    int width, height;          // 地图宽、高
    int wall[4] = {1, 1, 1, 1}; // 墙的虚实
    int obstacleNum = 0, x, y;  // 障碍物数量与坐标
    do
    {
        std::cout << "输入地图大小：";
        std::cin >> width >> height;
        if (!typeCheck())
            continue;
    } while (!mapContentCheck(width, height, wall, 0)); // 检查宽、高
    for (int i = 0; i < width; i++)                     // 墙载入
    {
        map[0][i] = '-';
        map[height - 1][i] = '-';
    }
    for (int i = 0; i < height; i++)
    {
        map[i][0] = '|';
        map[i][width - 1] = '|';
    }
    map[height / 2][width / 2] = '#'; // 蛇载入
    map[height / 2][width / 2 - 1] = '*';
    map[height / 2][width / 2 - 2] = '*';
    map[height / 2][width / 2 - 3] = '*';
    char key; // 键盘输入
    bool n;   // 暂存输入的墙虚实
    mapEditorPrt(map, width, height, wall, WALL_COLOR);
    do
    {
        std::cin >> key;
        switch (key)
        {
        case 'q': // 退出
            fout.close();
            name = "maps//" + name + ".map";
            remove(name.c_str()); // 删除新建的文件
            return;
        case 'f': // 保存
            break;
        case 'o': // 添加障碍物
            std::cin >> x >> y;
            if (!typeCheck())
                continue;
            if (mapObstacleCheck(map, x, y, width, height, true)) // 添加障碍物是否合法
            {
                obstacleNum++;
                map[x][y] = '%';
                mapEditorPrt(map, width, height, wall, WALL_COLOR);
            }
            break;
        case 'p': // 删除障碍物
            std::cin >> x >> y;
            if (!typeCheck())
                continue;
            if (mapObstacleCheck(map, x, y, width, height, false)) // 删除障碍物是否合法
            {
                obstacleNum--;
                map[x][y] = ' ';
                mapEditorPrt(map, width, height, wall, WALL_COLOR);
            }
            break;
        case 'w':
        case 'a':
        case 's':
        case 'd': // 四面墙虚实
            std::cin >> n;
            if (!typeCheck())
                continue;
            wall[KEY_TO_WALL.at(int(key))] = n; // 标记虚实
            mapEditorPrt(map, width, height, wall, WALL_COLOR);
            break;
        default:
            std::cout << "未知指令" << std::endl;
            break;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    } while (key != 'q' && key != 'f');
    fout << width << ' ' << height << std::endl // 数据存入文件
         << wall[0] << ' ' << wall[1] << ' ' << wall[2] << ' ' << wall[3] << std::endl
         << obstacleNum << std::endl;
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (map[i][j] == '%')
                fout << i << ' ' << j << std::endl;
        }
    }
    fout.close();
    return;
}

void mapEditorPrt(char map[][25], int width, int height, int wall[], const std::string WALL_COLOR[])
{
    system("cls");                  // 刷新输出
    for (int j = 0; j < width; j++) // 地图第一行
        std::cout << WALL_COLOR[wall[0]] << map[0][j] << "\033[0m ";
    std::cout << std::endl;
    for (int i = 1; i < height - 1; i++) // 中间行
    {
        std::cout << WALL_COLOR[wall[1]] << map[i][0] << "\033[0m "; // 左侧墙
        for (int j = 1; j < width - 1; j++)
        {
            std::cout << map[i][j] << ' ';
        }
        std::cout << WALL_COLOR[wall[3]] << map[i][width - 1] << "\033[0m " << std::endl; // 右侧墙
    }
    for (int j = 0; j < width; j++) // 地图最后一行
        std::cout << WALL_COLOR[wall[2]] << map[height - 1][j] << "\033[0m ";
    std::cout << std::endl;
    return;
}

#endif