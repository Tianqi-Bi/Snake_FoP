# 程序设计基础:大作业- SnakeFoP

## 基本信息

### 小组成员信息：

* btq
* qyn

### 项目运行环境：

* 系统环境：Windows 11
* 操作系统版本：22621.2861
* 使用编译器：gcc.exe (x86_64-posix-seh-rev1, Built by MinGW-Builds project) 13.1.0
* C++版本：C++17

### 文件结构：

- main.cpp
- Snake.hpp
- validation.hpp
- editors.hpp
- .last
- config
  - default.config
- maps
  - default.map
- record

***

## 功能实现逻辑：

代码主体主要分为四部分：
  * main.cpp
  * Snake.hpp
  * validation.hpp
  * editors.hpp
   
　　其中，main.cpp主要负责接受玩家的输入，并调用不同的函数来进行处理。Snake.hpp是游戏玩法的主体部分，负责游戏的运行。validation.hpp是对玩家输入合法性的检验部分，并会对玩家的非法输入做出反馈。editors.hpp主要处理玩家对于地图和配置的自定义编辑。下面我们从玩家的游玩的顺序来依次介绍。

 ### 1. 打开程序：

玩家通过main.cpp开始游戏后，代码将字符集设定为UTF-8，并打开.last文件开始记录玩家输入，便于记录回放功能。
```
  system("chcp 65001"); // 字符集设为utf-8
  system("cls");

  ifstream finLast;
  finLast.open(".last", ios::in);
  if (finLast)
  {
      finLast >> dcfg >> dmap;
      finLast.close();
  }
```
随后对配置和地图文件的加载进行检测，若打开失败则给予相应的反馈。
```
  if (!game.loadCfg(dcfg) || !game.loadMap(dmap))
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
```
若配置与地图文件加载均无问题，则调用Snake.hpp中的ptr()函数进行第一次输出屏幕。

### 2. prt()函数：

```
void prt(bool partly = false)
```
ptr()的输出分为两种：若游戏没有在运行，partly为false（不输入默认为false）；若游戏正在运行，partly为true，，则对蛇的身体进行操作，使蛇向指定方向移动一格。
```
if (partly) // 部分刷新
{
    moveCursor(body.front().x, body.front().y * 2); // 身体最前一节
    std::cout << snakeColor + "*\033[0m";
    moveCursor(body.back().x, body.back().y * 2); // 身体应该删除的一节
    std::cout << COLOR[map[body.back().x][body.back().y].second] << map[body.back().x][body.back().y].first << "\033[0m";
}
```
ptr()随后根据加载的配置和地图信息输出游戏的主要界面信息和墙体、障碍物，并根据随机种子开始刷新食物

### 3. 配置、地图加载：

配置、地图的加载分别依靠Snake.hpp中的loadCfg()函数和loadMap()函数完成，并会调用validation.hpp中的对应函数来检验文件中输入信息的合法性。

* 配置 配置信息包括难度、随机种子、场上同时存在食物数量、不同分值的食物的生成概率。调用loadCfg()函数后，程序会打开对应的文件夹，并读取其中的对应信息。
* 地图 地图信息包括四面墙的虚实和障碍物数量、障碍物位置。调用loadMap()函数后，程序会打开对应的文件夹，并读取其中的对应信息，同时，函数会将地图对应位置的形体与颜色储存在三维数组map[][]中，便于ptr()和其他函数的使用。

在程序刚开始运行时会默认加载上一次打开的配置与地图文件，若文件损毁则打开默认配置，此时若默认配置也损毁则提示并退出游戏。在玩家给出加载配置和地图的指令后，程序会打开玩家所制定的配置或地图文件，若加载失败则反馈。

### 4. 玩家的输入：

在进行初步的加载、初始化之后，玩家可以开始输入，程序会根据玩家的不同输入给予不同的反应：
* g　开始游戏
* q　退出游戏
* u　载入配置
* m　载入地图
* i　自定义配置
* n　自定义地图
* r　回放功能
* h　用户输入帮助指南
* 其他非法输入　反馈并提示输入"h"获取帮助指南

同时每次进行输入后会清空缓冲区以免对下一次输入造成影响。

### 5. 开始游戏：

检测到玩家输入为"g"后，游戏将正式开始。
```
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
        if (direction != KEY_TO_DIRECTION[int(key)][2])
            direction = KEY_TO_DIRECTION[int(key)][1];
        break;
        default:
            break;
     }
    run();     // 游戏运行一步
    prt(true); // 采用局部刷新
}
```
随后根据所设定的难度，在对应的时间内不断读取键盘点击信息，并取时间内最后一次输入作为最终输入。然后将输入的w、a、s、d转化为对应的蛇的方向变量direction。随后调用Snake.cpp中run()函数来让游戏进行一步：蛇向制定方向前进一格，若蛇吃到了食物则加上对应的得分并再次随机刷新食物。于此同时，玩家的每次有效输入都将记录进动态数组records中，以便于游戏结束后进行记录。

### 6.游戏结束：

在游戏进行每一步时，都会通过Snake.cpp中的isOver()函数来判定一次游戏是否结束。
结束条件：蛇头撞上了实墙边界、障碍物或者蛇的身体。
```
bool isOver()
{
    snakeColor = "\033[31m"; // 红色
    if (map[head.x][head.y].first != ' ' && map[head.x][head.y].second == 1)
    return true;
    for (std::vector<POINT>::iterator i = body.begin(); i != body.end() - 1; i++) // 与身体相撞
    {
        if (i->x == head.x && i->y == head.y)
        return true;
    }
    snakeColor = "\033[32m"; // 绿色
    return false;
}
```
结束后蛇的身体将变为红色，并询问是否保存回放，若玩家输入"b"，则为保存。此时会让玩家输入录像文件的名称，并将玩家本局游戏中的操作和所使用的配置和地图文件名储存在录像文件中。
```
void start()
{
    ......
    prt();
    cout << "Game Over!" << endl
         << "是否保存本局记录？ 按b保存,其他键退出:";
    std::string input;
    cin >> input;
    if (input == "b")
    {
        input = getName("record"); // 获得合法的回访文件名
        if (input != "q")
        saveRecord(input);
    }
}
```

### 7. 自定义配置、地图：

玩家在输入时输入"i"和"n"，会分别进入自定义配置和地图的程序中。

* 自定义配置　进入自定义配置程序后，程序将调用editors.hpp中的configEditor()函数。首先要求玩家输入新建配置的名称并调用getName()函数检验该名称是否已存在，已存在则询问是否覆盖。随后分别输入难度、随机种子、同一时间食物数量、不同分值食物概率。全部输入完成后会调用validation.hpp中的cfgContentCheck()函数检验输入的合法性。若输入不合法则对输入中不合法的部分给予反馈并要求重新输入。若输入合法则统一保存到对应配置文件中。
* 自定义地图　进入自定义地图后，程序将调用editors.hpp中的mapEditor()函数。首先要求玩家输入新建配置的名称并调用getName()函数检验该名称是否已存在，已存在则询问是否覆盖。随后要求输入地图的大小width、height，输入后以第一次输出默认地图。然后用户可以使用指令更改地图文件：
  * o + 参数x、y 在坐标x、y处新建障碍物
  * p + 参数x、y 删除坐标x、y处的障碍物
  * w、a、s、d + 参数0或1 设置对应墙的虚实，0为虚墙，1为实墙
  * f 结束地图输入并保存地图文件
  * q 退出地图输入并删除地图文件
   
  在用户每输入一次后均会刷新一次屏幕并对输入有对应的图像反馈。若输入不合法也将有对应的反馈。

### 8. 游戏记录回放
玩家在输入时输入"r"，则会进入回放程序。首先要求玩家输入所需放映的回放所存储的文件名称，并对录像文件名是否存在进行检测,若文件存在则开始读取配置和地图信息并对其存在性和输入的合法性进行检测。
```
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
    srand(randSeed);            // 重置种子
    loadMap(currentMap, false); // 已经提前检查过合法性
    prt();
    ......
    finRecord.close();
    if (!isOver()) // 若游戏还未结束，则文件出错
        return false;
    prt();
    return true;
}
```
通过后即开启回放进程。回放过程中，每隔难度对应的时间就会读取一次录像文件的操作，得益于相同随机种子生成的随机数列相同的特性，只需要记录玩家操作，通过网络游戏中常用的“操作复现”来进行回放，即对玩家的操作进行复刻而得到完全相同的结果。在回放过程中，玩家可随时输入q退出回放。
```
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
```
***

## 核心函数功能

### main.cpp

  处理指令，控制整体流程。

### Snake.hpp

含有Snake类，其中包含6个可外部调用的方法，4个不可外部调用的方法。
以下是不可调用的方法。

* 生成食物
  ```
  void generateFood()
  {
    ......
  }   
  ```
  generateFood()函数通过随机数生成三个数值，分别反映生成食物的分值和所在的坐标。
  ```
  float foodtype = rand() % 10000 / 10000.0;
  for (int i = 0; i < 3; i++) // 依次判断生成1-3分的食物
  {
      if (foodtype <= foodChance[i]) // 若随机数小于生成i+1分食物的概率，则可以生成
      {
        ......
      }
      foodtype -= foodChance[i]; // 随机数大于生成食物的概率，减去该数再生成更大的食物
  }
  ```
  foodtype是数值介于0~1之间的浮点数。将0到1之间的区间根据概率依次分为一分、二分、三分三部分，判断foodtype属于的区间范围从而判定生成食物的分值。
  ```
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
  ```
  生成食物坐标后首先对坐标合法性进行检验，检验合法后记录进地图信息map[][]中。


* 游戏运行
  ```
  void run()
  {
    ......
  }
  ```
  run()函数负责处理游戏运行中蛇身体所在位置的移动和其他变化。通过OFFSET[direction]来控制蛇头下一步的运行方向。处理方法为：将蛇向指定方向延申一格，如果撞到墙则移动到另一侧的墙边（具体撞到实墙还是虚墙、蛇是否死亡交由其他函数判定），如果没吃到食物则删除蛇身体的最后一节，从而实现蛇的移动。
  ```
  //检测是否吃到食物
  bool popBack = true;  
  ......
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
  ```
  ```
  records.push_back(direction);                      // 记录操作
  body.insert(body.begin(), {head.x, head.y});       // 移动身体
  head.x += OFFSET[direction].x, head.y += OFFSET[direction].y; // 移动头
  ```

* 保存回放
  ```// 保存回放
  void saveRecord(std::string name)
  {
    ......
  }
  ```
* 移动鼠标坐标

以下是可以外部调用的方法。

* 游戏开始、屏幕输出、载入配置、载入地图与记录回放
  ```
  //游戏开始
  void start()
  {
    ......
  }
  //输出到屏幕
  void prt(bool partly = false)
  {
    ......
  }
  //加载配置
  bool loadCfg(std::string name, bool check = true)
  {
    ......
  }
  //加载地图
  bool loadMap(std::string name, bool check = true)
  {
    ......
  }
  // 游戏状态
  bool isOver()
  {
    ......
  }
  //回放
  bool replay(std::string name)
  {
    ......
  }
  ```
  其中loadCfg()与loadMap()函数的返回值为bool型变量，代表加载配置或者地图是否成功。这些函数上文中已经有详细阐述，在此不再赘述。
  

### validation.hpp 
validation.hpp 中的函数为对各种输入的合法性检测。根据检测内容类型，可以分为一下五类：文件名、输入数据类型、输入配置信息、输入地图信息、文件存在性。
```
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
```

* 返回合法文件名
  ```
  std::string getName(std::string route)
  {
      const std::map<std::string, std::string> SUFFIX = {{"config", ".config"}, {"maps", ".map"}, {"record", ".rec"}};//路径与后缀对应
      std::ifstream fin;
      std::string name;
      while (true)
      {
          std::cout << "输入文件名，或按 q 退出：";
          std::cin >> name;
          fin.open(route + "//" + name + SUFFIX.at(route),std::ios::in);
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
  ```
  其中，函数的参数为所需文件的类型，返回值为最后得到的合法的文件名称（不含路径和后缀）。在输入名称后，若已存在相同类型的同名文件，则会询问是否覆盖，若回答是（y）则返回文件名，否则要求再次输入。

* 数据类型检查
  ```
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
  ```
  该函数负责检查数据类型，并在数据类型出错时清空缓冲区，以免影响下一次输入

* 检查配置文件、检查地图文件
  ```
  bool cfgCheck(std::string name)
  {
    ......
  }
  bool mapCheck(std::string name)
  {
    ......
  }
  ```
  这两个函数的参数均为对应的文件名称。作用为首先检测文件是否存在，若文件存在则将文件中的信息读取出来，并调用cfgContentCheck()、mapContentCheck()和mapObstacleCheck()函数对读取的信息进行合法性检验。

* 检查配置文件信息、检查地图文件信息
  ```
  bool cfgContentCheck(int difficulty, int randseed, int foodNum, float foodChance[])
  bool mapContentCheck(int width, int height, int wall[], int obstacleNum)
  bool mapObstacleCheck(char map[][25], int x, int y, int width, int height, bool addOrDelete)
  ```
  这三个函数负责对配置和文件信息的合法性进行检验，包括自定义配置或地图时的输入和载入配置或地图时的输入。主要检验输入数据的范围。
* 录像文件检验
  ```
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
      if(randseed!=-1&&randseed<0){
          std::cerr << "种子无效" << std::endl;
          return false;
      }
      return true;
  }
  ```
  该函数首先会检验录像文件的存在性。若存在，则读取文件中的配置和地图名称，再分别调用配置和地图检验函数检查其中信息的合法性。

### editors.hpp
editors.hpp中函数负责应对玩家对于配置和地图的自定义编辑功能
```
// 编辑配置文件
void configEditor();
// 编辑地图
void mapEditor();
// 用于在编辑地图时输出地图
void mapEditorPrt(char[][25], int, int, int[], const std::string[]);
```
其中配置编辑函数会调用validation.hpp的返回合法文件名函数和输入信息检验函数。

地图编辑函数则是让玩家通过指令编辑地图信息，根据玩家的指令来修改相应的地图信息，面对玩家的非法输入也会给予相应的反馈。同时，它会在玩家每次输入指令结束后调用mapEditorPrt()函数，从而做出相应的视觉反馈。而在输入是，由于不同的指令长短不一，通过利用缓冲区与适时清空缓冲区可以实现不同长度的指令在一行输入并不会对下次输入造成影响

* 编辑地图时输出地图
  ```
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
  ```
***
