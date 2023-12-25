#include "Snake.hpp"
#include "editors.hpp"
#include <conio.h>
#include <cstring>
#include <fstream>
#include <windows.h>
#include <iostream>
#include <limits>
using namespace std;

int main()
{
    system("chcp 65001"); // 字符集设为utf-8
    system("cls");
    char key;          // 输入的字符
    string input;      // 输入的字符串
    string dcfg, dmap; // 默认配置与地图
    ifstream finLast;  // 读取上次加载的配置与地图名称
    finLast.open(".last", ios::in);
    if (finLast)
    {
        finLast >> dcfg >> dmap;
        finLast.close();
    }
    do // 每个循环为一局游戏
    {
        Snake game; // 新建一局游戏
        system("cls");
        if (!game.loadCfg(dcfg) || !game.loadMap(dmap)) // 加载上次的配置与地图
        {
            cerr << "上次配置或地图加载失败，载入默认设置" << endl;
            if (!game.loadCfg("default") || !game.loadMap("default"))
            {
                cerr << "默认配置已损坏，无法运行" << endl;
                system("pause");
                return 0;
            }
            dcfg = "default", dmap = "default";
        }
        game.prt();
        do // 每个循环处理一次游戏外输入
        {
            cout << "指令：";
            cin >> key;
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 清空缓冲区
            switch (key)
            {
            case 'g': // 开始游戏
                game.start();
                break;
            case 'q': // 退出
                break;
            case 'u': // 载入配置
                cout << "请输入要载入的配置文件名，或按 q 取消:";
                do
                {
                    cin >> input;
                    if (input == "q")
                        break;
                } while (!game.loadCfg(input)); // 如果加载失败，重复尝试
                dcfg = input;
                break;
            case 'm': // 载入地图
                cout << "请输入要载入的地图文件名, 或按 q 取消:";
                do
                {
                    cin >> input;
                    if (input == "q")
                        break;
                } while (!game.loadMap(input)); // 如果加载失败，重复尝试
                dmap = input;
                break;
            case 'i': // 自定义配置
                configEditor();
                break;
            case 'n': // 自定义地图
                mapEditor();
                break;
            case 'r': // 回放
                cout << "输入回放文件名：";
                cin >> input;
                if (!game.replay(input))
                    std::cerr << "回放文件损坏" << std::endl;
                cout << "按任意键返回或按 q 退出游戏" << endl;
                key = _getch();
                continue;
            case 'h': // 帮助
                cout << "g:开始游戏  q:退出游戏  p:重置游戏  u:载入配置  m:载入地图" << endl
                     << "i:新建配置  n:新建地图  r:回放" << endl;
                continue;
            default: // 其他输入
                cout << "未知命令，请重新输入。按 h 显示帮助。" << endl;
                continue;
            }
            game.prt();
        } while (!game.isOver() && key != 'q'); // 退出循环条件：结束或输入为q
    } while (key != 'q');                       // 退出游戏：输入为q
    ofstream foutLast;                          // 保存最后一次配置和地图
    foutLast.open(".last", ios::out);
    foutLast << dcfg << ' ' << dmap;
    foutLast.close();
    return 0;
}