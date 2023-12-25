#ifndef SNAKE_HPP
#define SNAKE_HPP 1

#include "validation.hpp"
#include <iostream>
#include <conio.h>
#include <vector>
#include <windows.h>
#include <cstring>
#include <fstream>
#include <ctime>
#include <map>

class Snake
{
private:
    const std::string COLOR[5] = {"\033[0;32m", "\033[0;31m", "\033[34m", "\033[35m", "\033[33m"};            // 颜色索引，食物颜色=分值+1
    const std::map<int, int> KEY_TO_DIRECTION = {{int('w'), 0}, {int('s'), 1}, {int('a'), 2}, {int('d'), 3}}; // 按键与方向对应
    const POINT OFFSET[4] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};                                               // 方向对应的坐标移动
    const int CONFLICT[4] = {1, 0, 3, 2};                                                                     // 判断方向冲突

    int height, width;                   // 游戏区域大小
    std::pair<char, int> map[25][25];    // 储存地图与对应的颜色(不包含蛇)
    int wall[4];                         // 四面墙的虚实
    POINT head;                          // 头位置
    std::vector<POINT> body;             // 身体位置，其中最后一个为上次删除的位置
    POINT newFood = {0, 0};              // 最新食物的位置
    std::string snakeColor = "\033[32m"; // 蛇颜色，默认为绿色
    int direction = 3;                   // 方向 0上1下2左3右
    int points = 0;                      // 得分
    int difficulty;                      // 难度
    int foodNum;                         // 同屏食物数量
    float foodChance[3];                 // 食物生成概率
    std::vector<short> records;          // 记录操作
    int randSeed;                        // 随机种子
    std::string currentCfg, currentMap;  // 当前配置与地图

    // 移动光标到x行y列
    void moveCursor(short x, short y)
    {
        COORD pos = {y, x};
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
    }
    // 生成食物
    void generateFood()
    {
        float foodtype = rand() % 10000 / 10000.0;
        for (int i = 0; i < 3; i++) // 依次判断生成1-3分的食物
        {
            if (foodtype <= foodChance[i]) // 若随机数小于生成i+1分食物的概率，则可以生成
            {
                int x, y;
                bool regenerate = true;
                while (regenerate) // 合法性检查
                {
                    regenerate = false;
                    x = rand() % height, y = rand() % width;
                    if (map[x][y].first != ' ' || head.x == x && head.y == y) // 与食物、地图或头重合
                    {
                        regenerate = true;
                        continue;
                    }
                    for (std::vector<POINT>::iterator i = body.begin(); i != body.end(); i++)
                    {
                        if (i->x == x && i->y == y) // 与身体重合
                        {
                            regenerate = true;
                            break;
                        }
                    }
                }
                map[x][y].first = '@';    // 在地图上标记食物符号
                map[x][y].second = i + 2; // 颜色(分值+1)
                newFood = {x, y};         // 记录最新食物的位置，输出新食物时使用
                break;
            }
            foodtype -= foodChance[i]; // 随机数大于生成食物的概率，减去该数再生成更大的食物
        }
        return;
    }
    // 游戏运行
    void run()
    {
        bool popBack = true;                                          // 是否删除最后一节身体（若吃到食物则为否）
        records.push_back(direction);                                 // 记录操作
        body.insert(body.begin(), {head.x, head.y});                  // 移动身体
        head.x += OFFSET[direction].x, head.y += OFFSET[direction].y; // 移动头
        if (head.x < 0)                                               // 防止出界
            head.x = width - 1;
        if (head.x == width)
            head.x = 0;
        if (head.y < 0)
            head.y = height - 1;
        if (head.y == height)
            head.y = 0;
        if (map[head.x][head.y].first == '@') // 吃到食物
        {
            points += (map[head.x][head.y].second - 1);
            map[head.x][head.y].first = ' ';
            map[head.x][head.y].second = 0;
            popBack = false;
            generateFood();
        }
        if (popBack)
            body.pop_back();
        return;
    }
    // 保存回放
    void saveRecord(std::string name)
    {
        std::ofstream foutRecord;
        foutRecord.open("record\\" + name + ".rec", std::ios::out);
        foutRecord << currentCfg << ' ' << currentMap << ' ' << randSeed << std::endl;  // 记录配置、地图与种子
        for (std::vector<short>::iterator i = records.begin(); i != records.end(); i++) // 记录操作
        {
            foutRecord << *i << ' ';
        }
        foutRecord.close();
        return;
    }

public:
    // 开始游戏
    void start()
    {
        for (int i = 0; i < foodNum; i++) // 食物初始化
            generateFood();
        prt();
        char key;
        while (!isOver())
        {
            clock_t start = clock();
            while (float(clock() - start) / CLOCKS_PER_SEC < (1.0 / difficulty)) // 控制间隔时间
            {
                if (_kbhit())
                    key = _getch(); // 获得（最后一次）输入
            }
            switch (key)
            {
            case 'w':
            case 's':
            case 'a':
            case 'd':
                if (direction != CONFLICT[KEY_TO_DIRECTION.at(int(key))])
                    direction = KEY_TO_DIRECTION.at(int(key));
                break;
            default:
                break;
            }
            run();     // 游戏运行一步
            prt(true); // 采用局部刷新
        }
        prt();
        std::cout << "Game Over!" << std::endl
                  << "是否保存本局记录？ 按b保存,其他键退出:";
        std::string input;
        std::cin >> input;
        if (input == "b")
        {
            input = getName("record"); // 获得合法的回访文件名
            if (input != "q")
                saveRecord(input);
        }
        return;
    }
    // 输出到屏幕
    void prt(bool partly = false)
    {
        if (partly) // 部分刷新
        {
            moveCursor(body.front().x, body.front().y * 2); // 身体最前一节
            std::cout << snakeColor + "*\033[0m";
            moveCursor(body.back().x, body.back().y * 2); // 身体应该删除的一节
            std::cout << COLOR[map[body.back().x][body.back().y].second] << map[body.back().x][body.back().y].first << "\033[0m";
        }
        else // 全屏刷新
        {
            system("cls");
            moveCursor(0, 0);
            for (int i = 0; i < height; i++) // 地图与食物
            {
                for (int j = 0; j < width; j++)
                {
                    std::cout << COLOR[map[i][j].second] << map[i][j].first << "\033[0m ";
                }
                std::cout << std::endl;
            }
            for (std::vector<POINT>::iterator i = body.begin(); i != body.end() - 1; i++) // 输出完整的蛇
            {
                moveCursor(i->x, i->y * 2);
                std::cout << snakeColor + "*\033[0m";
            }
            moveCursor(2, width * 2 + 3); // 游戏信息
            std::cout << "config: " << currentCfg << ".config";
            moveCursor(3, width * 2 + 3);
            std::cout << "map: " << currentMap << ".map";
            moveCursor(4, width * 2 + 3);
            std::cout << "seed: " << randSeed;
        }
        if (newFood.x != 0 || newFood.y != 0) // 若食物刷新，输出新食物
        {
            moveCursor(newFood.x, newFood.y * 2);
            std::cout << COLOR[map[newFood.x][newFood.y].second] << map[newFood.x][newFood.y].first << "\033[0m";
            newFood = {0, 0};
        }
        moveCursor(head.x, head.y * 2); // 头
        std::cout << snakeColor + "#\033[0m";
        moveCursor(1, width * 2 + 3); // 更新分数
        std::cout << "Current score: " << points;
        moveCursor(height, 0);
        return;
    }
    // 游戏状态
    bool isOver()
    {
        snakeColor = "\033[31m";                                                 // 红色
        if (map[head.x][head.y].first != ' ' && map[head.x][head.y].second == 1) // 与红色的障碍相撞
            return true;
        for (std::vector<POINT>::iterator i = body.begin(); i != body.end() - 1; i++) // 与身体相撞
        {
            if (i->x == head.x && i->y == head.y)
                return true;
        }
        snakeColor = "\033[32m"; // 绿色
        return false;
    }
    // 加载配置
    bool loadCfg(std::string name, bool check = true)
    {
        if (check && !cfgCheck(name)) // 需要检查且检查结果不合法
            return false;
        std::ifstream fCfg;
        fCfg.open("config\\" + name + ".config", std::ios::in);
        currentCfg = name; // 更新当前配置
        fCfg >> difficulty >> randSeed >> foodNum >> foodChance[0] >> foodChance[1] >> foodChance[2];
        if (randSeed == -1)
            randSeed = time(NULL);
        srand(randSeed);
        fCfg.close();
        return true;
    }
    // 加载地图
    bool loadMap(std::string name, bool check = true)
    {
        if (check && !mapCheck(name)) // 需要检查且检查结果不合法
            return false;
        std::ifstream fMap;
        fMap.open("maps\\" + name + ".map", std::ios::in);
        currentMap = name; // 更新当前地图
        fMap >> height >> width >> wall[0] >> wall[1] >> wall[2] >> wall[3];
        for (int i = 0; i < height; i++) // 地图初始化
        {
            for (int j = 0; j < width; j++)
            {
                map[i][j].first = ' ';
                map[i][j].second = 0;
            }
        }
        for (int i = 0; i < width; i++) // 标记边界的符号与颜色
        {
            map[0][i].first = '-';
            map[0][i].second = wall[0];
            map[height - 1][i].first = '-';
            map[height - 1][i].second = wall[2];
        }
        for (int i = 0; i < height; i++)
        {
            map[i][0].first = '|';
            map[i][0].second = wall[1];
            map[i][width - 1].first = '|';
            map[i][width - 1].second = wall[3];
        }
        int n, x, y; // 暂存障碍的个数与位置
        fMap >> n;
        for (int i = 0; i < n; i++) // 存入障碍物
        {
            fMap >> x >> y;
            map[x][y].first = '%';
            map[x][y].second = 1;
        }
        fMap.close();
        head = {width / 2, height / 2}; // 蛇头初始化
        body.clear();                   // 身体初始化
        for (int i = 1; i < 5; i++)
            body.push_back({head.x, head.y - i});
        return true;
    }
    // 回放
    bool replay(std::string name)
    {
        if (!recCheck(name)) // 内容检查
            return false;
        std::ifstream finRecord;
        finRecord.open("record\\" + name + ".rec", std::ios::in);
        finRecord >> currentCfg >> currentMap;
        loadCfg(currentCfg, false); // 已经提前检查过合法性
        finRecord >> randSeed;
        srand(randSeed);                  // 重置种子
        loadMap(currentMap, false);       // 已经提前检查过合法性
        for (int i = 0; i < foodNum; i++) // 食物初始化
            generateFood();
        prt();
        char input; // 键盘输入
        while (finRecord >> direction)
        {
            clock_t start = clock();
            while (float(clock() - start) / CLOCKS_PER_SEC < (1.0 / difficulty)) // 间隔时间
            {
                if (_kbhit())
                    input = _getch();
            }
            if (input == 'q') // 退出回放
                return true;
            run();                           // 运行
            prt(true);                       // 局部刷新
            if (finRecord.eof() && isOver()) // 若游戏中途结束，则文件出错
            {
                finRecord.close();
                return false;
            }
        }
        finRecord.close();
        if (!isOver()) // 若游戏还未结束，则文件出错
            return false;
        prt();
        return true;
    }
};

#endif