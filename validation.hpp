#ifndef VALIDATION_HPP
#define VALIDATION_HPP 1

#include <fstream>
#include <cstring>
#include <iostream>
#include <map>
#include <limits>

// 返回合法文件名
std::string getName(std::string);

// 检查是否出现输入类型错误
bool typeCheck();

// 检查配置文件
bool cfgCheck(std::string);

// 检查配置文件内容
bool cfgContentCheck(int, int, int, float[]);

// 检查地图文件
bool mapCheck(std::string);

// 检查地图内容
bool mapContentCheck(int, int, int[], int);

// 检查地图障碍物
bool mapObstacleCheck(char[][25], int, int, int, int, bool);

// 检查回放文件
bool recCheck(std::string);

std::string getName(std::string route)
{
    const std::map<std::string, std::string> SUFFIX = {{"config", ".config"}, {"maps", ".map"}, {"record", ".rec"}}; // 路径与后缀对应
    std::ifstream fin;
    std::string name;
    while (true)
    {
        std::cout << "输入文件名，或按 q 退出：";
        std::cin >> name;
        fin.open(route + "//" + name + SUFFIX.at(route), std::ios::in);
        if (fin)
        {
            fin.close();
            std::string input;
            std::cout << "已存在重名文件，是否覆盖？按 y 确认,任意键取消：";
            std::cin >> input;
            if (input != "y")
                continue;
            else
                return name;
        }
        else
            return name;
    }
}

bool typeCheck()
{
    if (std::cin.fail())
    {
        std::cout << "输入的数据类型错误，请重新输入" << std::endl;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return false;
    }
    return true;
}

bool cfgCheck(std::string name)
{
    std::ifstream fin;
    fin.open("config//" + name + ".config", std::ios::in);
    if (!fin)
    {
        std::cerr << "该配置文件不存在，请重试" << std::endl;
        return false;
    }
    int difficulty, randseed, foodNum;
    float foodChance[3];
    fin >> difficulty >> randseed >> foodNum >> foodChance[0] >> foodChance[1] >> foodChance[2];
    fin.close();
    return cfgContentCheck(difficulty, randseed, foodNum, foodChance);
}

bool cfgContentCheck(int difficulty, int randseed, int foodNum, float foodChance[])
{
    bool isRight = true;
    if (difficulty < 1 || difficulty > 10)
    {
        isRight = false;
        std::cerr << "难度不为1~10的整数" << std::endl;
    }
    if (randseed < 0 && randseed != -1)
    {
        isRight = false;
        std::cerr << "随机种子不为无符号整数或-1" << std::endl;
    }
    if (foodNum < 1 || foodNum > 5)
    {
        isRight = false;
        std::cerr << "食物数量不为1~5的整数" << std::endl;
    }
    if ((foodChance[0] + foodChance[1] + foodChance[2]) > 1.01 || (foodChance[0] + foodChance[1] + foodChance[2]) < 0.99 || foodChance[0] < 0 || foodChance[1] < 0 || foodChance[2] < 0)
    {
        isRight = false;
        std::cerr << "食物概率小于0或总和大于1" << std::endl;
    }
    return isRight;
}

bool mapCheck(std::string name)
{
    std::ifstream fin;
    fin.open("maps//" + name + ".map", std::ios::in);
    if (!fin)
    {
        std::cerr << "该地图文件不存在，请重试" << std::endl;
        return false;
    }
    int width, height, wall[4], obstacleNum;
    fin >> width >> height >> wall[0] >> wall[1] >> wall[2] >> wall[3] >> obstacleNum;
    if (!mapContentCheck(width, height, wall, obstacleNum))
    {
        fin.close();
        return false;
    }
    char map[25][25];
    int n, x, y;
    memset(map, ' ', sizeof(map));
    map[height / 2][width / 2] = '#'; // 蛇载入
    map[height / 2][width / 2 - 1] = '*';
    map[height / 2][width / 2 - 2] = '*';
    map[height / 2][width / 2 - 3] = '*';
    for (int i = 0; i < obstacleNum; i++)
    {
        if (!(fin >> x >> y))
        {
            std::cerr << "缺少障碍物信息" << std::endl;
            fin.close();
            return false;
        }
        if (!mapObstacleCheck(map, x, y, width, height, true))
            return false;
    }
    return true;
}

bool mapContentCheck(int width, int height, int wall[], int obstacleNum)
{
    if (width < 8 || width > 20 || height < 8 || height > 20)
    {
        std::cerr << "地图大小非8-20整数, 重新输入" << std::endl;
        return false;
    }
    for (int i = 0; i < 4; i++)
    {
        if (wall[i] != 0 && wall[i] != 1)
        {
            std::cerr << "墙虚实非0或1, 重新输入" << std::endl;
            return false;
        }
    }
    if (obstacleNum < 0 || obstacleNum > ((width - 2) * (height - 2) - 4))
    {
        std::cerr << "障碍数量无效, 重新输入" << std::endl;
        return false;
    }
    return true;
}

bool mapObstacleCheck(char map[][25], int x, int y, int width, int height, bool addOrDelete)
{
    if (x <= 0 || x >= width || y <= 0 || y >= height)
    {
        std::cerr << "超出地图范围" << std::endl;
        return false;
    }
    if (addOrDelete) // TRUE 添加 FALSE 删除
    {
        if (map[x][y] != ' ')
        {
            std::cerr << "目标地点已有物体" << std::endl;
            return false;
        }
    }
    else
    {
        if (map[x][y] != '%')
        {
            std::cerr << "目标地点没有障碍物" << std::endl;
            return false;
        }
    }
    return true;
}

bool recCheck(std::string name)
{
    std::string cfg, map;
    int randseed;
    std::ifstream frecord;
    frecord.open("record\\" + name + ".rec", std::ios::in);
    if (!frecord)
    {
        std::cerr << "录像不存在" << std::endl;
        return false;
    }
    frecord >> cfg >> map >> randseed;
    frecord.close();
    if (!cfgCheck(cfg) || !mapCheck(map))
        return false;
    if (randseed != -1 && randseed < 0)
    {
        std::cerr << "种子无效" << std::endl;
        return false;
    }
    return true;
}

#endif