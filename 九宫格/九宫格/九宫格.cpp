#include<graphics.h>
#include<vector>
#include<string>
#include<algorithm>
#include<conio.h>
#include<sstream>
#include<fstream>
#include<Windows.h>
#include<ctime>

using namespace std;

//枚举游戏状态
enum GameState
{
	MAIN_MENU,
	PLAYING,
	PAUSED,
	REPLAY,
	SAVE_LOAD_MENU,
	GAME_OVER,
	INSTRUCTIONS
};

//枚举棋子类型
enum PieceType
{
	EMPTY,
	BLACKPIECE,
	WHITEPIECE
};

//小宫状态
class SmallBoard
{
public:
	PieceType cells[3][3] = { EMPTY };			//每一个小宫分出的3*3小格子
	bool blackWon;								//黑棋是否在这个小宫获胜
	bool whiteWon;								//白棋是否在这个小宫获胜
};

//游戏存档结构
class GameSave
{
public:
	vector<pair<int, int>> moves;				//所有走棋位置
	PieceType currentPlayer = BLACKPIECE;		//当前玩家
	SmallBoard smallBoards[3][3];				//所有小宫状态
	int blackWins = 0;							//黑棋赢得的小宫数
	int whiteWins = 0;							//白棋赢得的小宫数
	time_t saveTime;							//存档时间
};

//全局变量
GameState gameState = MAIN_MENU;
GameSave currentGame;
GameSave replayGame;
int replayIndex = 0;
bool isSaving = false;

const int MAX_SAVES = 5;				//最大存档数量
vector<GameSave> gameSaves(MAX_SAVES);	//存档列表
int currentSaveSlot = 0;				//当前选中的存档槽

int nextBoardX = -1, nextBoardY = -1;	//下一个必须下的小宫坐标（-1表示任意）

//初始化新游戏
void InitNewGame()
{
	currentGame.moves.clear();
	currentGame.currentPlayer = BLACKPIECE;
	currentGame.blackWins = 0;
	currentGame.whiteWins = 0;

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			currentGame.smallBoards[i][j].blackWon = false;
			currentGame.smallBoards[i][j].whiteWon = false;
			for (int x = 0; x < 3; x++)
			{
				for (int y = 0; y < 3; y++)
				{
					currentGame.smallBoards[i][j].cells[x][y] = EMPTY;	//清除小格子内的棋子
				}
			}
		}
	}

	nextBoardX = -1;
	nextBoardY = -1;
}

//检查小宫内是否有玩家获胜
void CheckSmallBoardWinner(SmallBoard& board)
{
	board.blackWon = false;
	board.whiteWon = false;

	//检查黑棋是否连成一线
	bool blackWin = false;
	//检查行
	for (int i = 0; i < 3; i++)
	{
		if (board.cells[i][0] == BLACKPIECE &&
			board.cells[i][1] == BLACKPIECE &&
			board.cells[i][2] == BLACKPIECE)
		{
			blackWin = true;
			break;
		}
	}
	//检查列
	if (!blackWin)
	{
		for (int j = 0; j < 3; j++)
		{
			if (board.cells[0][j] == BLACKPIECE &&
				board.cells[1][j] == BLACKPIECE &&
				board.cells[2][j] == BLACKPIECE)
			{
				blackWin = true;
				break;
			}
		}
	}
	//检查对角线
	if (!blackWin)
	{
		if (board.cells[0][0] == BLACKPIECE &&
			board.cells[1][1] == BLACKPIECE &&
			board.cells[2][2] == BLACKPIECE)
		{
			blackWin = true;
		}
		if (board.cells[0][2] == BLACKPIECE &&
			board.cells[1][1] == BLACKPIECE &&
			board.cells[2][0] == BLACKPIECE)
		{
			blackWin = true;
		}
	}

	//检查白棋是否连成一线
	bool whiteWin = false;
	//检查行
	for (int i = 0; i < 3; i++)
	{
		if (board.cells[i][0] == WHITEPIECE &&
			board.cells[i][1] == WHITEPIECE &&
			board.cells[i][2] == WHITEPIECE)
		{
			whiteWin = true;
			break;
		}
	}
	//检查列
	if (!whiteWin)
	{
		for (int j = 0; j < 3; j++)
		{
			if (board.cells[0][j] == WHITEPIECE &&
				board.cells[1][j] == WHITEPIECE &&
				board.cells[2][j] == WHITEPIECE)
			{
				whiteWin = true;
				break;
			}
		}
	}
	//检查对角线
	if (!whiteWin)
	{
		if (board.cells[0][0] == WHITEPIECE &&
			board.cells[1][1] == WHITEPIECE &&
			board.cells[2][2] == WHITEPIECE)
		{
			whiteWin = true;
		}
		if (board.cells[0][2] == WHITEPIECE &&
			board.cells[1][1] == WHITEPIECE &&
			board.cells[2][0] == WHITEPIECE)
		{
			whiteWin = true;
		}
	}

	board.blackWon = blackWin;
	board.whiteWon = whiteWin;
}

//检查大棋盘是否有玩家获胜
PieceType CheckBigBoardWinner()
{
	//分别检查黑棋和白棋在棋盘上是否获胜
	bool blackWins = false;
	bool whiteWins = false;

	//检查黑棋是否在大棋盘上连成一线

	//检查行
	for (int i = 0; i < 3; i++) {
		if (currentGame.smallBoards[i][0].blackWon &&
			currentGame.smallBoards[i][1].blackWon &&
			currentGame.smallBoards[i][2].blackWon)
		{
			blackWins = true;
		}
	}

	// 检查列
	for (int j = 0; j < 3; j++) {
		if (currentGame.smallBoards[0][j].blackWon &&
			currentGame.smallBoards[1][j].blackWon &&
			currentGame.smallBoards[2][j].blackWon)
		{
			blackWins = true;
		}
	}

	//检查对角线
	if (currentGame.smallBoards[0][0].blackWon &&
		currentGame.smallBoards[1][1].blackWon &&
		currentGame.smallBoards[2][2].blackWon)
	{
		blackWins = true;
	}
	if (currentGame.smallBoards[0][2].blackWon &&
		currentGame.smallBoards[1][1].blackWon &&
		currentGame.smallBoards[2][0].blackWon)
	{
		blackWins = true;
	}

	//检查白棋是否在大棋盘上连成一线

	//检查行
	for (int i = 0; i < 3; i++) {
		if (currentGame.smallBoards[i][0].whiteWon &&
			currentGame.smallBoards[i][1].whiteWon &&
			currentGame.smallBoards[i][2].whiteWon)
		{
			whiteWins = true;
		}
	}

	// 检查列
	for (int j = 0; j < 3; j++) {
		if (currentGame.smallBoards[0][j].whiteWon &&
			currentGame.smallBoards[1][j].whiteWon &&
			currentGame.smallBoards[2][j].whiteWon)
		{
			whiteWins = true;
		}
	}

	//检查对角线
	if (currentGame.smallBoards[0][0].whiteWon &&
		currentGame.smallBoards[1][1].whiteWon &&
		currentGame.smallBoards[2][2].whiteWon)
	{
		whiteWins = true;
	}
	if (currentGame.smallBoards[0][2].whiteWon &&
		currentGame.smallBoards[1][1].whiteWon &&
		currentGame.smallBoards[2][0].whiteWon)
	{
		whiteWins = true;
	}

	//判定是否获胜
	if (blackWins) return BLACKPIECE;
	if (whiteWins) return WHITEPIECE;

	// 检查棋盘是否填满
	if (currentGame.moves.size() == 81)
	{
		if (currentGame.blackWins > currentGame.whiteWins) return BLACKPIECE;
		if (currentGame.whiteWins > currentGame.blackWins) return WHITEPIECE;
		return EMPTY;				// 和棋
	}

	return EMPTY;
}

//绘制主菜单
void DrawMainMenu()
{
	cleardevice();

	//背景色
	setbkcolor(RGB(240, 240, 240));
	cleardevice();

	//标题
	settextstyle(80, 0, _T("宋体"));
	settextcolor(BLACK);
	outtextxy(getwidth() / 2 - textwidth(_T("九宫格")) / 2, 100, _T("九宫格"));

	//菜单选项
	settextstyle(40, 0, _T("宋体"));

	//开始游戏
	setfillcolor(RGB(200, 230, 255));
	fillrectangle(getwidth() / 2 - 150, 250, getwidth() / 2 + 150, 310);
	setbkmode(TRANSPARENT); // 设置文字背景透明
	outtextxy(getwidth() / 2 - textwidth(_T("开始游戏")) / 2, 260, _T("开始游戏"));

	//存档复盘
	setfillcolor(RGB(200, 230, 255));
	fillrectangle(getwidth() / 2 - 150, 350, getwidth() / 2 + 150, 410);
	setbkmode(TRANSPARENT); // 设置文字背景透明
	outtextxy(getwidth() / 2 - textwidth(_T("存档复盘")) / 2, 360, _T("存档复盘"));

	//游戏说明
	setfillcolor(RGB(200, 230, 255));
	fillrectangle(getwidth() / 2 - 150, 450, getwidth() / 2 + 150, 510);
	setbkmode(TRANSPARENT); // 设置文字背景透明
	outtextxy(getwidth() / 2 - textwidth(_T("游戏说明")) / 2, 460, _T("游戏说明"));

	//退出游戏
	setfillcolor(RGB(200, 230, 255));
	fillrectangle(getwidth() / 2 - 150, 550, getwidth() / 2 + 150, 610);
	setbkmode(TRANSPARENT); // 设置文字背景透明
	outtextxy(getwidth() / 2 - textwidth(_T("退出游戏")) / 2, 560, _T("退出游戏"));
}

//绘制游戏界面
void DrawGameBoard()
{
	cleardevice();

	//背景色
	setbkcolor(RGB(240, 240, 240));
	cleardevice();

	//绘制大棋盘
	int boardSize = min(getwidth(), getheight()) - 100;
	int startX = (getwidth() - boardSize) / 2;
	int startY = (getheight() - boardSize) / 2;

	setlinecolor(BLACK);

	//绘制大格子
	setlinestyle(PS_SOLID, 5);
	for (int i = 0; i <= 3; i++)
	{
		line(startX, startY + i * boardSize / 3, startX + boardSize, startY + i * boardSize / 3);
		line(startX + i * boardSize / 3, startY, startX + i * boardSize / 3, startY + boardSize);
	}

	//绘制小格子
	setlinestyle(PS_SOLID, 2);
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			int smallStartX = startX + j * boardSize / 3;
			int smallStartY = startY + i * boardSize / 3;
			int smallCellSize = boardSize / 9;

			//如果这个小宫有赢家，则填充颜色
			if (currentGame.smallBoards[i][j].blackWon || currentGame.smallBoards[i][j].whiteWon)
			{
				//如果双方同时占据了这个小宫，显示混合颜色
				if (currentGame.smallBoards[i][j].blackWon && currentGame.smallBoards[i][j].whiteWon)
				{
					setfillcolor(RGB(150, 150, 150));
				}
				else if (currentGame.smallBoards[i][j].blackWon)
				{
					//深色表示黑棋占据这个小宫
					setfillcolor(RGB(180, 180, 180));
				}
				else if (currentGame.smallBoards[i][j].whiteWon)
				{
					//浅色表示黑棋占据这个小宫
					setfillcolor(RGB(220, 220, 220));
				}
				fillrectangle(smallStartX, smallStartY, smallStartX + boardSize / 3, smallStartY + boardSize / 3);
			}

			//绘制小格子
			for (int x = 0; x <= 3; x++)
			{
				line(smallStartX, smallStartY + x * smallCellSize, smallStartX + boardSize / 3, smallStartY + x * smallCellSize);
				line(smallStartX + x * smallCellSize, smallStartY, smallStartX + x * smallCellSize, smallStartY + boardSize / 3);
			}

			//绘制棋子
			for (int x = 0; x < 3; x++)
			{
				for (int y = 0; y < 3; y++)
				{
					int centerX = smallStartX + y * smallCellSize + smallCellSize / 2;
					int centerY = smallStartY + x * smallCellSize + smallCellSize / 2;
					int radius = smallCellSize / 2 - 5;

					if (currentGame.smallBoards[i][j].cells[x][y] == BLACKPIECE)
					{
						setfillcolor(BLACK);
						fillcircle(centerX, centerY, radius);
					}
					else if (currentGame.smallBoards[i][j].cells[x][y] == WHITEPIECE)
					{
						setfillcolor(WHITE);
						setlinecolor(BLACK);
						fillcircle(centerX, centerY, radius);
						circle(centerX, centerY, radius);
					}
				}
			}

		}
	}

	//显示当前玩家
	settextstyle(30, 0, _T("宋体"));
	if (currentGame.currentPlayer == BLACKPIECE)
	{
		settextcolor(BLACK);
		outtextxy(50, 50, _T("当前：黑棋回合"));
	}
	else
	{
		settextcolor(RGB(100, 100, 100));
		outtextxy(50, 50, _T("当前：白棋回合"));
	}

	//高亮下一个必须下的小宫
	if (nextBoardX != -1 && nextBoardY != -1)
	{
		setlinecolor(RED);
		setlinestyle(PS_SOLID, 3);
		rectangle(startX + nextBoardY * boardSize / 3 + 2, startY + nextBoardX * boardSize / 3 + 2,
			startX + (nextBoardY + 1) * boardSize / 3 - 2, startY + (nextBoardX + 1) * boardSize / 3 - 2);

	}

	//如果nextBoardX==-1且nextBoardY==-1，则高亮整个大宫
	else if (nextBoardX == -1 && nextBoardY == -1)
	{
		setlinecolor(RED);
		setlinestyle(PS_SOLID, 7);
		rectangle(startX + 3, startY + 3, startX + boardSize - 3, startY + boardSize - 3);
	}

}

//绘制暂停菜单
void DrawPauseMenu()
{
	//半透明背景
	setfillcolor(RGB(0, 0, 0, 128));
	solidrectangle(0, 0, getwidth(), getheight());

	//暂停菜单框
	setfillcolor(RGB(240, 240, 240));
	fillrectangle(getwidth() / 2 - 150, getheight() / 2 - 150, getwidth() / 2 + 150, getheight() / 2 + 150);

	settextstyle(40, 0, _T("宋体"));
	settextcolor(BLACK);
	setbkmode(TRANSPARENT); // 设置文字背景透明
	outtextxy(getwidth() / 2 - textwidth(_T("游戏暂停")) / 2, getheight() / 2 - 120, _T("游戏暂停"));

	//继续游戏
	settextstyle(30, 0, _T("宋体"));
	setfillcolor(RGB(200, 230, 255));

	fillrectangle(getwidth() / 2 - 120, getheight() / 2 - 60, getwidth() / 2 + 120, getheight() / 2 - 10);
	setbkmode(TRANSPARENT); // 设置文字背景透明
	outtextxy(getwidth() / 2 - textwidth(_T("继续游戏")) / 2, getheight() / 2 - 55, _T("继续游戏"));

	//返回主菜单
	fillrectangle(getwidth() / 2 - 120, getheight() / 2 + 10, getwidth() / 2 + 120, getheight() / 2 + 60);
	setbkmode(TRANSPARENT); // 设置文字背景透明
	outtextxy(getwidth() / 2 - textwidth(_T("返回主菜单")) / 2, getheight() / 2 + 15, _T("返回主菜单"));

	//存档
	fillrectangle(getwidth() / 2 - 120, getheight() / 2 + 80, getwidth() / 2 + 120, getheight() / 2 + 130);
	setbkmode(TRANSPARENT); // 设置文字背景透明
	outtextxy(getwidth() / 2 - textwidth(_T("存档")) / 2, getheight() / 2 + 85, _T("存档"));

}

//绘制游戏说明
void DrawInstructions()
{
	cleardevice();

	//背景色
	setbkcolor(RGB(240, 240, 240));
	cleardevice();

	//标题
	settextcolor(BLACK);
	settextstyle(50, 0, _T("宋体"));
	setbkmode(TRANSPARENT); // 设置文字背景透明
	outtextxy(getwidth() / 2 - textwidth(_T("游戏说明")) / 2, 50, _T("游戏说明"));

	//游戏规则

	settextstyle(25, 0, _T("宋体"));

	const TCHAR* rules[] = {
		_T("游戏规则:"),
		_T("1. 棋盘为9x9格子，分为9个3x3小宫"),
		_T("2. 黑棋先手，第一手可在任意位置落子"),
		_T("3. 后续落子位置由上一手决定:"),
		_T("   上一手落子在小宫的(x,y)位置，"),
		_T("   则下一手必须在大宫的(x,y)小宫落子"),
		_T("4. 率先在一个小宫内连成一线者赢得该小宫"),
		_T("5. 同一小宫可被双方同时赢得")
		_T(",率先在大宫连成三个小宫一线或赢得五个小宫者胜"),
		_T("6. 若棋盘填满未分胜负，则小宫多者胜"),
		_T("7. 若小宫数相等则为和棋")
	};

	setbkmode(TRANSPARENT); // 设置文字背景透明

	for (int i = 0; i < sizeof(rules) / sizeof(rules[0]); i++)
	{
		outtextxy(100, 150 + i * 40, rules[i]);
	}

	// 返回按钮
	int buttonWidth = 200;
	int buttonHeight = 50;
	int buttonX = getwidth() / 2 - buttonWidth / 2;
	int buttonY = getheight() - 100;

	setfillcolor(RGB(200, 230, 255));
	fillrectangle(buttonX, buttonY, buttonX + buttonWidth, buttonY + buttonHeight);
	settextstyle(30, 0, _T("宋体"));
	setbkmode(TRANSPARENT);
	outtextxy(buttonX + (buttonWidth - textwidth(_T("返回"))) / 2,
		buttonY + (buttonHeight - textheight(_T("返回"))) / 2, _T("返回"));
}

//绘制游戏结束画面
void DrawGameover()
{
	//半透明背景
	setfillcolor(RGB(0, 0, 0, 128));
	solidrectangle(0, 0, getwidth(), getheight());

	//结束菜单框
	setfillcolor(RGB(240, 240, 240));
	fillrectangle(getwidth() / 2 - 200, getheight() / 2 - 120, getwidth() / 2 + 200, getheight() / 2 + 150);

	settextstyle(40, 0, _T("宋体"));
	settextcolor(BLACK);

	setbkmode(TRANSPARENT); // 设置文字背景透明

	//显示胜负
	if (CheckBigBoardWinner() == BLACKPIECE)
	{
		outtextxy(getwidth() / 2 - textwidth(_T("黑棋获胜!")) / 2, getheight() / 2 - 60, _T("黑棋获胜!"));
	}
	else if (CheckBigBoardWinner() == WHITEPIECE)
	{
		outtextxy(getwidth() / 2 - textwidth(_T("白棋获胜!")) / 2, getheight() / 2 - 60, _T("白棋获胜!"));
	}
	else
	{
		outtextxy(getwidth() / 2 - textwidth(_T("双方和棋!")) / 2, getheight() / 2 - 60, _T("双方和棋!"));
	}

	settextstyle(30, 0, _T("宋体"));

	//存档按钮
	setfillcolor(RGB(200, 230, 255));
	fillrectangle(getwidth() / 2 - 100, getheight() / 2 + 10, getwidth() / 2 + 100, getheight() / 2 + 50);
	setbkmode(TRANSPARENT); // 设置文字背景透明
	outtextxy(getwidth() / 2 - textwidth(_T("存档")) / 2, getheight() / 2 + 20, _T("存档"));

	//返回主菜单按钮
	setfillcolor(RGB(200, 230, 255));
	fillrectangle(getwidth() / 2 - 100, getheight() / 2 + 50, getwidth() / 2 + 100, getheight() / 2 + 90);
	setbkmode(TRANSPARENT); // 设置文字背景透明
	outtextxy(getwidth() / 2 - textwidth(_T("返回主菜单")) / 2, getheight() / 2 + 60, _T("返回主菜单"));
}

//绘制存档/读档界面
void DrawSaveLoadMenu(bool isSaving)
{
	cleardevice();

	//背景色
	setbkcolor(RGB(240, 240, 240));
	cleardevice();

	//标题
	settextcolor(BLACK);
	settextstyle(50, 0, _T("宋体"));
	setbkmode(TRANSPARENT); // 设置文字背景透明
	outtextxy(getwidth() / 2 - textwidth(isSaving ? _T("选择存档的位置") : _T("选择存档加载")) / 2,
		50, isSaving ? _T("选择存档的位置") : _T("选择存档加载"));

	//存档槽
	settextstyle(30, 0, _T("宋体"));
	for (int i = 0; i < MAX_SAVES; i++)
	{
		//存档槽背景
		setfillcolor(i == currentSaveSlot ? RGB(180, 220, 255) : RGB(200, 230, 255));
		fillrectangle(getwidth() / 2 - 300, 150 + i * 100, getwidth() / 2 + 300, 220 + i * 100);

		//存档信息
		if (!gameSaves[i].moves.empty())
		{
			//显示存档的时间和步数
			char timeStr[50];
			struct tm timeinfo;
			localtime_s(&timeinfo, &gameSaves[i].saveTime);
			strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", &timeinfo);

			wchar_t infoText[100];
			swprintf(infoText, 100, L"存档 %d: %hs (%d步)", i + 1, timeStr, (int)gameSaves[i].moves.size());
			outtextxy(getwidth() / 2 - textwidth(infoText) / 2, 160 + i * 100, infoText);
		}
		else
		{
			//空存档
			TCHAR slotText[20];
			swprintf_s(slotText, _T("存档 %d (空)"), i + 1);
			setbkmode(TRANSPARENT); // 设置文字背景透明
			outtextxy(getwidth() / 2 - 50, 160 + i * 100, slotText);
		}
	}

	//返回按钮
	setfillcolor(RGB(200, 230, 255));
	fillrectangle(getwidth() / 2 - 100, getheight() - 100, getwidth() / 2 + 100, getheight() - 50);
	setbkmode(TRANSPARENT); // 设置文字背景透明
	outtextxy(getwidth() / 2 - textwidth(_T("返回")) / 2, getheight() - 90, _T("返回"));

}

//绘制复盘界面
void DrawReplay()
{
	cleardevice();

	//背景色
	setbkcolor(RGB(240, 240, 240));
	cleardevice();

	//标题
	settextcolor(BLACK);
	settextstyle(40, 0, _T("宋体"));
	setbkmode(TRANSPARENT); // 设置文字背景透明
	outtextxy(getwidth() / 2 - textwidth(_T("存档复盘")) / 2, 30, _T("存档复盘"));

	//绘制棋盘（与游戏界面相同）
	int boardSize = min(getwidth(), getheight()) - 150;
	int startX = (getwidth() - boardSize) / 2;
	int startY = (getheight() - boardSize) / 2;

	setlinecolor(BLACK);

	//绘制大格子
	setlinestyle(PS_SOLID, 5);
	for (int i = 0; i <= 3; i++)
	{
		line(startX, startY + i * boardSize / 3, startX + boardSize, startY + i * boardSize / 3);
		line(startX + i * boardSize / 3, startY, startX + i * boardSize / 3, startY + boardSize);
	}

	//绘制小格子
	setlinestyle(PS_SOLID, 2);
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			int smallStartX = startX + j * boardSize / 3;
			int smallStartY = startY + i * boardSize / 3;
			int smallCellSize = boardSize / 9;

			// 如果这个小宫已经有赢家，填充颜色
			if (replayGame.smallBoards[i][j].blackWon || replayGame.smallBoards[i][j].whiteWon)
			{
				// 如果双方都赢了，显示混合颜色
				if (replayGame.smallBoards[i][j].blackWon && replayGame.smallBoards[i][j].whiteWon)
				{
					setfillcolor(RGB(150, 150, 150));  // 灰色表示双方都赢了
				}
				else if (replayGame.smallBoards[i][j].blackWon)
				{
					setfillcolor(RGB(180, 180, 180));  // 深色表示黑棋赢了
				}
				else
				{
					setfillcolor(RGB(220, 220, 220));  // 浅色表示白棋赢了
				}
				fillrectangle(smallStartX, smallStartY, smallStartX + boardSize / 3, smallStartY + boardSize / 3);
			}

			//绘制小格子
			for (int x = 0; x <= 3; x++)
			{
				line(smallStartX, smallStartY + x * smallCellSize, smallStartX + boardSize / 3, smallStartY + x * smallCellSize);
				line(smallStartX + x * smallCellSize, smallStartY, smallStartX + x * smallCellSize, smallStartY + boardSize / 3);
			}

			//绘制棋子
			GameSave tempGame;					//创建临时游戏状态tempGame，每次复盘绘制时重新初始化
			tempGame.moves.clear();
			tempGame.currentPlayer = BLACKPIECE;
			tempGame.blackWins = 0;
			tempGame.whiteWins = 0;
			for (int i = 0; i < 3; i++)
			{
				for (int j = 0; j < 3; j++)
				{
					tempGame.smallBoards[i][j].blackWon = false;
					tempGame.smallBoards[i][j].whiteWon = false;
					for (int x = 0; x < 3; x++)
						for (int y = 0; y < 3; y++)
							tempGame.smallBoards[i][j].cells[x][y] = EMPTY;
				}
			}

			for (int k = 0; k < replayIndex && k < replayGame.moves.size(); k++)
			{
				int bigX = replayGame.moves[k].first;
				int bigY = replayGame.moves[k].second;
				int boardX = bigX / 3;
				int boardY = bigY / 3;
				int cellX = bigX % 3;
				int cellY = bigY % 3;

				PieceType player = (k % 2 == 0) ? BLACKPIECE : WHITEPIECE;
				tempGame.smallBoards[boardX][boardY].cells[cellX][cellY] = player;
				CheckSmallBoardWinner(tempGame.smallBoards[boardX][boardY]);
			}

			for (int x = 0; x < 3; x++)
			{
				for (int y = 0; y < 3; y++)
				{
					if (tempGame.smallBoards[i][j].cells[x][y] != EMPTY)
					{
						int centerX = smallStartX + y * smallCellSize + smallCellSize / 2;
						int centerY = smallStartY + x * smallCellSize + smallCellSize / 2;
						int radius = smallCellSize / 2 - 5;

						if (tempGame.smallBoards[i][j].cells[x][y] == BLACKPIECE)
						{
							setfillcolor(BLACK);
							fillcircle(centerX, centerY, radius);
						}
						else if (tempGame.smallBoards[i][j].cells[x][y] == WHITEPIECE)
						{
							setfillcolor(WHITE);
							setlinecolor(BLACK);
							fillcircle(centerX, centerY, radius);
							circle(centerX, centerY, radius);
						}
					}
				}
			}

		}
	}

	//控制按钮
	settextstyle(25, 0, _T("宋体"));

	//上一手
	setfillcolor(replayIndex > 0 ? RGB(200, 230, 255) : RGB(150, 150, 150));
	fillrectangle(getwidth() / 2 - 250, startY + boardSize + 20, getwidth() / 2 - 150, startY + boardSize + 60);
	setbkmode(TRANSPARENT); // 设置文字背景透明
	outtextxy(getwidth() / 2 - 250 + (100 - textwidth(_T("上一手"))) / 2, startY + boardSize + 25, _T("上一手"));

	//下一手
	setfillcolor(replayIndex < replayGame.moves.size() ? RGB(200, 230, 255) : RGB(150, 150, 150));
	fillrectangle(getwidth() / 2 - 50, startY + boardSize + 20, getwidth() / 2 + 50, startY + boardSize + 60);
	setbkmode(TRANSPARENT); // 设置文字背景透明
	outtextxy(getwidth() / 2 - 50 + (100 - textwidth(_T("下一手"))) / 2, startY + boardSize + 25, _T("下一手"));

	//退出复盘
	setfillcolor(RGB(200, 230, 255));
	fillrectangle(getwidth() / 2 + 150, startY + boardSize + 20, getwidth() / 2 + 250, startY + boardSize + 60);
	setbkmode(TRANSPARENT); // 设置文字背景透明
	outtextxy(getwidth() / 2 + 150 + (100 - textwidth(_T("退出复盘"))) / 2, startY + boardSize + 25, _T("退出复盘"));
}

//处理鼠标点击主菜单
void HandleMainMenuClick(int x, int y)
{
	//检查点击了哪个按钮
	if (x >= getwidth() / 2 - 150 && x <= getwidth() / 2 + 150)
	{
		if (y >= 250 && y <= 310)			//开始游戏
		{
			InitNewGame();
			gameState = PLAYING;
			DrawGameBoard();
		}
		else if (y >= 350 && y <= 410)		//存档复盘
		{
			gameState = SAVE_LOAD_MENU;
			isSaving = false;
			currentSaveSlot = 0;
			DrawSaveLoadMenu(false);
		}
		else if (y >= 450 && y <= 510)		//游戏说明
		{
			gameState = INSTRUCTIONS;
			DrawInstructions();
		}
		else if (y >= 550 && y <= 610)		//退出游戏
		{
			closegraph();
			exit(0);
		}
	}
}

//处理游戏页面的点击
void HandleGameClick(int x, int y)
{
	int boardSize = min(getwidth(), getheight()) - 100;
	int startX = (getwidth() - boardSize) / 2;
	int startY = (getheight() - boardSize) / 2;

	//检查点击是否在棋盘内
	if (x < startX || x >= startX + boardSize ||
		y < startY || y >= startY + boardSize)
	{
		return;
	}

	//计算点击的大宫坐标
	int boardCol = (x - startX) / (boardSize / 3);
	int boardRow = (y - startY) / (boardSize / 3);

	//计算小宫内的坐标
	int cellX = (x - startX - boardCol * (boardSize / 3)) / (boardSize / 9);
	int cellY = (y - startY - boardRow * (boardSize / 3)) / (boardSize / 9);

	//检查这个位置是否已经有棋子
	if (currentGame.smallBoards[boardRow][boardCol].cells[cellY][cellX] != EMPTY)
	{
		return;
	}

	//检查是否下在特定小宫内
	if (nextBoardX != -1 && nextBoardY != -1 &&
		(boardRow != nextBoardX || boardCol != nextBoardY))
	{
		return;
	}

	//放置棋子
	currentGame.smallBoards[boardRow][boardCol].cells[cellY][cellX] = currentGame.currentPlayer;
	currentGame.moves.push_back(make_pair(boardRow * 3 + cellY, boardCol * 3 + cellX));

	//检查这个小宫是否有赢家
	CheckSmallBoardWinner(currentGame.smallBoards[boardRow][boardCol]);
	if (currentGame.smallBoards[boardRow][boardCol].blackWon)
	{
		currentGame.blackWins++;
	}
	if (currentGame.smallBoards[boardRow][boardCol].whiteWon)
	{
		currentGame.whiteWins++;
	}

	//检查游戏是否结束
	if (CheckBigBoardWinner() != EMPTY || currentGame.moves.size() == 81)
	{
		gameState = GAME_OVER;
		DrawGameBoard();
		DrawGameover();
		return;
	}

	//切换玩家
	currentGame.currentPlayer = (currentGame.currentPlayer == BLACKPIECE) ? WHITEPIECE : BLACKPIECE;

	//确定下一个必须下的小宫
	nextBoardX = cellY;
	nextBoardY = cellX;

	//如果下一个小宫已经下满，则可以在整个大宫位置随便落子
	int count = 0;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (currentGame.smallBoards[nextBoardX][nextBoardY].cells[i][j] != EMPTY)
			{
				count++;
			}
		}
	}
	if (count == 9)
	{
		nextBoardX = -1;
		nextBoardY = -1;
	}

	DrawGameBoard();

}

//处理暂停菜单的点击
void HandkePauseMenuClick(int x, int y)
{
	//检查点击了哪个按钮
	if (x >= getwidth() / 2 - 120 && x <= getwidth() / 2 + 120)
	{
		//继续游戏
		if (y >= getheight() / 2 - 60 && y <= getheight() / 2 - 10)
		{
			gameState = PLAYING;
			DrawGameBoard();
		}
		//返回主菜单
		else if (y >= getheight() / 2 + 10 && y <= getheight() / 2 + 60)
		{
			gameState = MAIN_MENU;
			DrawMainMenu();
		}
		//存档
		else if (y >= getheight() / 2 + 80 && y <= getheight() / 2 + 130)
		{
			gameState = SAVE_LOAD_MENU;
			isSaving = true;
			currentSaveSlot = 0;
			DrawSaveLoadMenu(true);
		}
	}
}

//处理存档/读档界面点击
void HandleSaveLoadMenuClick(int x, int y, bool isSaving)
{
	//检查存档槽点击
	for (int i = 0; i < MAX_SAVES; i++)
	{
		if (x >= getwidth() / 2 - 300 && x <= getwidth() / 2 + 300 &&
			y >= 150 + i * 100 && y <= 220 + i * 100)
		{
			currentSaveSlot = i;

			if (isSaving)
			{
				//保存游戏
				currentGame.saveTime = time(nullptr);
				gameSaves[currentSaveSlot] = currentGame;

				//保存到文件
				ofstream file("saves.dat", ios::binary);
				if (file)
				{
					for (int i = 0; i < MAX_SAVES; i++)
					{
						//写出步数
						int moveCount = (int)gameSaves[i].moves.size();
						file.write((char*)&moveCount, sizeof(moveCount));

						//写入每一步
						for (auto move : gameSaves[i].moves)
						{
							file.write((char*)&move.first, sizeof(move.first));
							file.write((char*)&move.second, sizeof(move.second));
						}

						//写入其他游戏状态
						file.write((char*)&gameSaves[i].currentPlayer, sizeof(gameSaves[i].currentPlayer));
						file.write((char*)&gameSaves[i].blackWins, sizeof(gameSaves[i].blackWins));
						file.write((char*)&gameSaves[i].whiteWins, sizeof(gameSaves[i].whiteWins));
						file.write((char*)&gameSaves[i].saveTime, sizeof(gameSaves[i].saveTime));

						//写入小宫状态
						for (int x = 0; x < 3; x++)
						{
							for (int y = 0; y < 3; y++)
							{
								file.write((char*)&gameSaves[i].smallBoards[x][y], sizeof(gameSaves[i].smallBoards[x][y]));
							}
						}
					}
				}

				MessageBox(GetHWnd(), _T("游戏已存档"), _T("提示"), MB_OK);
				gameState = PLAYING;
				DrawGameBoard();
			}

			else
			{
				//加载存档
				if (!gameSaves[i].moves.empty())
				{
					replayGame = gameSaves[i];
					replayIndex = 0;
					gameState = REPLAY;
					DrawReplay();
				}
			}

			return;
		}
	}

	//检查返回按钮
	if (x >= getwidth() / 2 - 100 && x <= getwidth() / 2 + 100 &&
		y >= getheight() - 100 && y <= getheight() - 50)
	{
		gameState = isSaving ? PAUSED : MAIN_MENU;
		if (isSaving)
		{
			DrawPauseMenu();
		}
		else
		{
			DrawMainMenu();
		}
	}
}

//处理游戏结束点击
void HandleGameOverClick(int x, int y)
{
	//检查是否点击了存档按钮
	if (x >= getwidth() / 2 - 100 && x <= getwidth() / 2 + 100)
	{
		//存档
		if (y >= getheight() / 2 + 10 && y <= getheight() / 2 + 50)
		{
			gameState = SAVE_LOAD_MENU;
			isSaving = true;
			currentSaveSlot = 0;
			DrawSaveLoadMenu(true);
			return;
		}
		//返回主菜单
		else if (y >= getheight() / 2 + 50 && y <= getheight() / 2 + 90)
		{
			gameState = MAIN_MENU;
			DrawMainMenu();
			return;
		}
	}
}

//处理游戏说明点击
void HandleInstructionsClick(int x, int y)
{
	// 使用与绘制时相同的坐标计算
	int buttonWidth = 200;
	int buttonHeight = 50;
	int buttonX = getwidth() / 2 - buttonWidth / 2;
	int buttonY = getheight() - 100;

	if (x >= buttonX && x <= buttonX + buttonWidth &&
		y >= buttonY && y <= buttonY + buttonHeight)
	{
		gameState = MAIN_MENU;
		DrawMainMenu();
	}
}

//处理复盘界面点击
void HandleReplayClick(int x, int y)
{
	int boardSize = min(getwidth(), getheight()) - 150;
	int startX = (getwidth() - boardSize) / 2;
	int startY = (getheight() - boardSize) / 2;

	//检查点击了哪个按钮
	if (y >= startY + boardSize + 20 && y <= startY + boardSize + 60)
	{
		//上一手
		if (x >= getwidth() / 2 - 250 && x <= getwidth() / 2 - 150)
		{
			if (replayIndex > 0)
			{
				//只撤销最后一步
				replayIndex--;

				//获取最后一步的位置
				auto move = replayGame.moves[replayIndex];
				int boardRow = move.first / 3;
				int boardCol = move.second / 3;
				int cellRow = move.first % 3;
				int cellCol = move.second % 3;

				//移除最后一步的棋子
				replayGame.smallBoards[boardRow][boardCol].cells[cellRow][cellCol] = EMPTY;

				//重置这个小宫的胜负状态
				bool wasBlackWon = replayGame.smallBoards[boardRow][boardCol].blackWon;
				bool wasWhiteWon = replayGame.smallBoards[boardRow][boardCol].whiteWon;

				//重置这个小宫的胜负状态
				replayGame.smallBoards[boardRow][boardCol].blackWon = false;
				replayGame.smallBoards[boardRow][boardCol].whiteWon = false;

				//重新检查这个小宫的胜负
				CheckSmallBoardWinner(replayGame.smallBoards[boardRow][boardCol]);

				//更新总胜利数
				if (wasBlackWon && !replayGame.smallBoards[boardRow][boardCol].blackWon)
				{
					replayGame.blackWins--;
				}
				if (wasWhiteWon && !replayGame.smallBoards[boardRow][boardCol].whiteWon)
				{
					replayGame.whiteWins--;
				}

				//切换玩家
				replayGame.currentPlayer = (replayGame.currentPlayer == BLACKPIECE) ? WHITEPIECE : BLACKPIECE;

				//重绘界面
				DrawReplay();
			}
		}

		//下一手
		else if (x >= getwidth() / 2 - 50 && x <= getwidth() / 2 + 50)
		{
			if (replayIndex < replayGame.moves.size())
			{
				auto move = replayGame.moves[replayIndex];
				int boardRow = move.first / 3;
				int boardCol = move.second / 3;
				int cellRow = move.first % 3;
				int cellCol = move.second % 3;

				//放置棋子
				replayGame.smallBoards[boardRow][boardCol].cells[cellRow][cellCol] = replayGame.currentPlayer;

				//检查这个小宫是否有赢家
				CheckSmallBoardWinner(replayGame.smallBoards[boardRow][boardCol]);
				if (replayGame.smallBoards[boardRow][boardCol].blackWon)
				{
					replayGame.blackWins++;
				}
				if (replayGame.smallBoards[boardRow][boardCol].whiteWon)
				{
					replayGame.whiteWins++;
				}

				//切换玩家
				replayGame.currentPlayer = (replayGame.currentPlayer == BLACKPIECE) ? WHITEPIECE : BLACKPIECE;

				replayIndex++;
				DrawReplay();
			}
		}

		//返回主菜单
		else if (x >= getwidth() / 2 + 150 && x <= getwidth() / 2 + 250)
		{
			gameState = MAIN_MENU;
			DrawMainMenu();
		}
	}
}

//加载存档
void LoadSaves()
{
	ifstream file("saves.dat", ios::binary);
	if (file)
	{
		for (int i = 0; i < MAX_SAVES; i++)
		{
			//读取步数
			int moveCount;
			file.read((char*)&moveCount, sizeof(moveCount));
			gameSaves[i].moves.resize(moveCount);

			//读取每一步
			for (int j = 0; j < moveCount; j++)
			{
				file.read((char*)&gameSaves[i].moves[j].first, sizeof(gameSaves[i].moves[j].first));
				file.read((char*)&gameSaves[i].moves[j].second, sizeof(gameSaves[i].moves[j].second));
			}

			//读取其他游戏状态
			file.read((char*)&gameSaves[i].currentPlayer, sizeof(gameSaves[i].currentPlayer));
			file.read((char*)&gameSaves[i].blackWins, sizeof(gameSaves[i].blackWins));
			file.read((char*)&gameSaves[i].whiteWins, sizeof(gameSaves[i].whiteWins));
			file.read((char*)&gameSaves[i].saveTime, sizeof(gameSaves[i].saveTime));

			//读取小宫状态
			for (int x = 0; x < 3; x++)
			{
				for (int y = 0; y < 3; y++)
				{
					file.read((char*)&gameSaves[i].smallBoards[x][y], sizeof(gameSaves[i].smallBoards[x][y]));
				}
			}
		}
	}

	//重置棋盘
	replayGame = gameSaves[currentSaveSlot];
	replayIndex = 0;

	replayGame.currentPlayer = BLACKPIECE;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			replayGame.smallBoards[i][j].blackWon = false;
			replayGame.smallBoards[i][j].whiteWon = false;
			for (int x = 0; x < 3; x++)
			{
				for (int y = 0; y < 3; y++)
				{
					replayGame.smallBoards[i][j].cells[x][y] = EMPTY;
				}
			}
		}
	}
}


int main()
{
	//初始化图形窗口
	initgraph(1000, 700, SHOWCONSOLE);
	setbkcolor(WHITE);
	cleardevice();

	//设置字体
	LOGFONT f;
	gettextstyle(&f);
	_tcscpy_s(f.lfFaceName, _T("宋体"));
	f.lfQuality = ANTIALIASED_QUALITY;
	settextstyle(&f);

	//加载存档
	LoadSaves();

	//绘制主菜单
	DrawMainMenu();

	//消息循环
	ExMessage msg;
	while (true)
	{
		if (peekmessage(&msg, EM_MOUSE | EM_KEY))
		{
			switch (msg.message)
			{
			case WM_LBUTTONDOWN:
				if (gameState == MAIN_MENU)
				{
					HandleMainMenuClick(msg.x, msg.y);
				}
				else if (gameState == PLAYING)
				{
					HandleGameClick(msg.x, msg.y);
				}
				else if (gameState == PAUSED)
				{
					HandkePauseMenuClick(msg.x, msg.y);
				}
				else if (gameState == REPLAY)
				{
					HandleReplayClick(msg.x, msg.y);
				}
				else if (gameState == SAVE_LOAD_MENU)
				{
					HandleSaveLoadMenuClick(msg.x, msg.y, isSaving);
				}
				else if (gameState == GAME_OVER)
				{
					HandleGameOverClick(msg.x, msg.y);
				}
				else if (gameState == INSTRUCTIONS)
				{
					HandleInstructionsClick(msg.x, msg.y);
				}
				break;

			case WM_KEYDOWN:
				if (msg.vkcode == VK_ESCAPE)
				{
					if (gameState == PLAYING)
					{
						gameState = PAUSED;
						DrawPauseMenu();
					}
					else if (gameState == PAUSED)
					{
						gameState = PLAYING;
						DrawGameBoard();
					}
				}
				break;
			}
		}

		//延时
		Sleep(10);
	}

	closegraph();
	return 0;
}