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

//ö����Ϸ״̬
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

//ö����������
enum PieceType
{
	EMPTY,
	BLACKPIECE,
	WHITEPIECE
};

//С��״̬
class SmallBoard
{
public:
	PieceType cells[3][3] = { EMPTY };			//ÿһ��С���ֳ���3*3С����
	bool blackWon;								//�����Ƿ������С����ʤ
	bool whiteWon;								//�����Ƿ������С����ʤ
};

//��Ϸ�浵�ṹ
class GameSave
{
public:
	vector<pair<int, int>> moves;				//��������λ��
	PieceType currentPlayer = BLACKPIECE;		//��ǰ���
	SmallBoard smallBoards[3][3];				//����С��״̬
	int blackWins = 0;							//����Ӯ�õ�С����
	int whiteWins = 0;							//����Ӯ�õ�С����
	time_t saveTime;							//�浵ʱ��
};

//ȫ�ֱ���
GameState gameState = MAIN_MENU;
GameSave currentGame;
GameSave replayGame;
int replayIndex = 0;
bool isSaving = false;

const int MAX_SAVES = 5;				//���浵����
vector<GameSave> gameSaves(MAX_SAVES);	//�浵�б�
int currentSaveSlot = 0;				//��ǰѡ�еĴ浵��

int nextBoardX = -1, nextBoardY = -1;	//��һ�������µ�С�����꣨-1��ʾ���⣩

//��ʼ������Ϸ
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
					currentGame.smallBoards[i][j].cells[x][y] = EMPTY;	//���С�����ڵ�����
				}
			}
		}
	}

	nextBoardX = -1;
	nextBoardY = -1;
}

//���С�����Ƿ�����һ�ʤ
void CheckSmallBoardWinner(SmallBoard& board)
{
	board.blackWon = false;
	board.whiteWon = false;

	//�������Ƿ�����һ��
	bool blackWin = false;
	//�����
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
	//�����
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
	//���Խ���
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

	//�������Ƿ�����һ��
	bool whiteWin = false;
	//�����
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
	//�����
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
	//���Խ���
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

//���������Ƿ�����һ�ʤ
PieceType CheckBigBoardWinner()
{
	//�ֱ������Ͱ������������Ƿ��ʤ
	bool blackWins = false;
	bool whiteWins = false;

	//�������Ƿ��ڴ�����������һ��

	//�����
	for (int i = 0; i < 3; i++) {
		if (currentGame.smallBoards[i][0].blackWon &&
			currentGame.smallBoards[i][1].blackWon &&
			currentGame.smallBoards[i][2].blackWon)
		{
			blackWins = true;
		}
	}

	// �����
	for (int j = 0; j < 3; j++) {
		if (currentGame.smallBoards[0][j].blackWon &&
			currentGame.smallBoards[1][j].blackWon &&
			currentGame.smallBoards[2][j].blackWon)
		{
			blackWins = true;
		}
	}

	//���Խ���
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

	//�������Ƿ��ڴ�����������һ��

	//�����
	for (int i = 0; i < 3; i++) {
		if (currentGame.smallBoards[i][0].whiteWon &&
			currentGame.smallBoards[i][1].whiteWon &&
			currentGame.smallBoards[i][2].whiteWon)
		{
			whiteWins = true;
		}
	}

	// �����
	for (int j = 0; j < 3; j++) {
		if (currentGame.smallBoards[0][j].whiteWon &&
			currentGame.smallBoards[1][j].whiteWon &&
			currentGame.smallBoards[2][j].whiteWon)
		{
			whiteWins = true;
		}
	}

	//���Խ���
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

	//�ж��Ƿ��ʤ
	if (blackWins) return BLACKPIECE;
	if (whiteWins) return WHITEPIECE;

	// ��������Ƿ�����
	if (currentGame.moves.size() == 81)
	{
		if (currentGame.blackWins > currentGame.whiteWins) return BLACKPIECE;
		if (currentGame.whiteWins > currentGame.blackWins) return WHITEPIECE;
		return EMPTY;				// ����
	}

	return EMPTY;
}

//�������˵�
void DrawMainMenu()
{
	cleardevice();

	//����ɫ
	setbkcolor(RGB(240, 240, 240));
	cleardevice();

	//����
	settextstyle(80, 0, _T("����"));
	settextcolor(BLACK);
	outtextxy(getwidth() / 2 - textwidth(_T("�Ź���")) / 2, 100, _T("�Ź���"));

	//�˵�ѡ��
	settextstyle(40, 0, _T("����"));

	//��ʼ��Ϸ
	setfillcolor(RGB(200, 230, 255));
	fillrectangle(getwidth() / 2 - 150, 250, getwidth() / 2 + 150, 310);
	setbkmode(TRANSPARENT); // �������ֱ���͸��
	outtextxy(getwidth() / 2 - textwidth(_T("��ʼ��Ϸ")) / 2, 260, _T("��ʼ��Ϸ"));

	//�浵����
	setfillcolor(RGB(200, 230, 255));
	fillrectangle(getwidth() / 2 - 150, 350, getwidth() / 2 + 150, 410);
	setbkmode(TRANSPARENT); // �������ֱ���͸��
	outtextxy(getwidth() / 2 - textwidth(_T("�浵����")) / 2, 360, _T("�浵����"));

	//��Ϸ˵��
	setfillcolor(RGB(200, 230, 255));
	fillrectangle(getwidth() / 2 - 150, 450, getwidth() / 2 + 150, 510);
	setbkmode(TRANSPARENT); // �������ֱ���͸��
	outtextxy(getwidth() / 2 - textwidth(_T("��Ϸ˵��")) / 2, 460, _T("��Ϸ˵��"));

	//�˳���Ϸ
	setfillcolor(RGB(200, 230, 255));
	fillrectangle(getwidth() / 2 - 150, 550, getwidth() / 2 + 150, 610);
	setbkmode(TRANSPARENT); // �������ֱ���͸��
	outtextxy(getwidth() / 2 - textwidth(_T("�˳���Ϸ")) / 2, 560, _T("�˳���Ϸ"));
}

//������Ϸ����
void DrawGameBoard()
{
	cleardevice();

	//����ɫ
	setbkcolor(RGB(240, 240, 240));
	cleardevice();

	//���ƴ�����
	int boardSize = min(getwidth(), getheight()) - 100;
	int startX = (getwidth() - boardSize) / 2;
	int startY = (getheight() - boardSize) / 2;

	setlinecolor(BLACK);

	//���ƴ����
	setlinestyle(PS_SOLID, 5);
	for (int i = 0; i <= 3; i++)
	{
		line(startX, startY + i * boardSize / 3, startX + boardSize, startY + i * boardSize / 3);
		line(startX + i * boardSize / 3, startY, startX + i * boardSize / 3, startY + boardSize);
	}

	//����С����
	setlinestyle(PS_SOLID, 2);
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			int smallStartX = startX + j * boardSize / 3;
			int smallStartY = startY + i * boardSize / 3;
			int smallCellSize = boardSize / 9;

			//������С����Ӯ�ң��������ɫ
			if (currentGame.smallBoards[i][j].blackWon || currentGame.smallBoards[i][j].whiteWon)
			{
				//���˫��ͬʱռ�������С������ʾ�����ɫ
				if (currentGame.smallBoards[i][j].blackWon && currentGame.smallBoards[i][j].whiteWon)
				{
					setfillcolor(RGB(150, 150, 150));
				}
				else if (currentGame.smallBoards[i][j].blackWon)
				{
					//��ɫ��ʾ����ռ�����С��
					setfillcolor(RGB(180, 180, 180));
				}
				else if (currentGame.smallBoards[i][j].whiteWon)
				{
					//ǳɫ��ʾ����ռ�����С��
					setfillcolor(RGB(220, 220, 220));
				}
				fillrectangle(smallStartX, smallStartY, smallStartX + boardSize / 3, smallStartY + boardSize / 3);
			}

			//����С����
			for (int x = 0; x <= 3; x++)
			{
				line(smallStartX, smallStartY + x * smallCellSize, smallStartX + boardSize / 3, smallStartY + x * smallCellSize);
				line(smallStartX + x * smallCellSize, smallStartY, smallStartX + x * smallCellSize, smallStartY + boardSize / 3);
			}

			//��������
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

	//��ʾ��ǰ���
	settextstyle(30, 0, _T("����"));
	if (currentGame.currentPlayer == BLACKPIECE)
	{
		settextcolor(BLACK);
		outtextxy(50, 50, _T("��ǰ������غ�"));
	}
	else
	{
		settextcolor(RGB(100, 100, 100));
		outtextxy(50, 50, _T("��ǰ������غ�"));
	}

	//������һ�������µ�С��
	if (nextBoardX != -1 && nextBoardY != -1)
	{
		setlinecolor(RED);
		setlinestyle(PS_SOLID, 3);
		rectangle(startX + nextBoardY * boardSize / 3 + 2, startY + nextBoardX * boardSize / 3 + 2,
			startX + (nextBoardY + 1) * boardSize / 3 - 2, startY + (nextBoardX + 1) * boardSize / 3 - 2);

	}

	//���nextBoardX==-1��nextBoardY==-1�������������
	else if (nextBoardX == -1 && nextBoardY == -1)
	{
		setlinecolor(RED);
		setlinestyle(PS_SOLID, 7);
		rectangle(startX + 3, startY + 3, startX + boardSize - 3, startY + boardSize - 3);
	}

}

//������ͣ�˵�
void DrawPauseMenu()
{
	//��͸������
	setfillcolor(RGB(0, 0, 0, 128));
	solidrectangle(0, 0, getwidth(), getheight());

	//��ͣ�˵���
	setfillcolor(RGB(240, 240, 240));
	fillrectangle(getwidth() / 2 - 150, getheight() / 2 - 150, getwidth() / 2 + 150, getheight() / 2 + 150);

	settextstyle(40, 0, _T("����"));
	settextcolor(BLACK);
	setbkmode(TRANSPARENT); // �������ֱ���͸��
	outtextxy(getwidth() / 2 - textwidth(_T("��Ϸ��ͣ")) / 2, getheight() / 2 - 120, _T("��Ϸ��ͣ"));

	//������Ϸ
	settextstyle(30, 0, _T("����"));
	setfillcolor(RGB(200, 230, 255));

	fillrectangle(getwidth() / 2 - 120, getheight() / 2 - 60, getwidth() / 2 + 120, getheight() / 2 - 10);
	setbkmode(TRANSPARENT); // �������ֱ���͸��
	outtextxy(getwidth() / 2 - textwidth(_T("������Ϸ")) / 2, getheight() / 2 - 55, _T("������Ϸ"));

	//�������˵�
	fillrectangle(getwidth() / 2 - 120, getheight() / 2 + 10, getwidth() / 2 + 120, getheight() / 2 + 60);
	setbkmode(TRANSPARENT); // �������ֱ���͸��
	outtextxy(getwidth() / 2 - textwidth(_T("�������˵�")) / 2, getheight() / 2 + 15, _T("�������˵�"));

	//�浵
	fillrectangle(getwidth() / 2 - 120, getheight() / 2 + 80, getwidth() / 2 + 120, getheight() / 2 + 130);
	setbkmode(TRANSPARENT); // �������ֱ���͸��
	outtextxy(getwidth() / 2 - textwidth(_T("�浵")) / 2, getheight() / 2 + 85, _T("�浵"));

}

//������Ϸ˵��
void DrawInstructions()
{
	cleardevice();

	//����ɫ
	setbkcolor(RGB(240, 240, 240));
	cleardevice();

	//����
	settextcolor(BLACK);
	settextstyle(50, 0, _T("����"));
	setbkmode(TRANSPARENT); // �������ֱ���͸��
	outtextxy(getwidth() / 2 - textwidth(_T("��Ϸ˵��")) / 2, 50, _T("��Ϸ˵��"));

	//��Ϸ����

	settextstyle(25, 0, _T("����"));

	const TCHAR* rules[] = {
		_T("��Ϸ����:"),
		_T("1. ����Ϊ9x9���ӣ���Ϊ9��3x3С��"),
		_T("2. �������֣���һ�ֿ�������λ������"),
		_T("3. ��������λ������һ�־���:"),
		_T("   ��һ��������С����(x,y)λ�ã�"),
		_T("   ����һ�ֱ����ڴ󹬵�(x,y)С������"),
		_T("4. ������һ��С��������һ����Ӯ�ø�С��"),
		_T("5. ͬһС���ɱ�˫��ͬʱӮ��")
		_T(",�����ڴ���������С��һ�߻�Ӯ�����С����ʤ"),
		_T("6. ����������δ��ʤ������С������ʤ"),
		_T("7. ��С���������Ϊ����")
	};

	setbkmode(TRANSPARENT); // �������ֱ���͸��

	for (int i = 0; i < sizeof(rules) / sizeof(rules[0]); i++)
	{
		outtextxy(100, 150 + i * 40, rules[i]);
	}

	// ���ذ�ť
	int buttonWidth = 200;
	int buttonHeight = 50;
	int buttonX = getwidth() / 2 - buttonWidth / 2;
	int buttonY = getheight() - 100;

	setfillcolor(RGB(200, 230, 255));
	fillrectangle(buttonX, buttonY, buttonX + buttonWidth, buttonY + buttonHeight);
	settextstyle(30, 0, _T("����"));
	setbkmode(TRANSPARENT);
	outtextxy(buttonX + (buttonWidth - textwidth(_T("����"))) / 2,
		buttonY + (buttonHeight - textheight(_T("����"))) / 2, _T("����"));
}

//������Ϸ��������
void DrawGameover()
{
	//��͸������
	setfillcolor(RGB(0, 0, 0, 128));
	solidrectangle(0, 0, getwidth(), getheight());

	//�����˵���
	setfillcolor(RGB(240, 240, 240));
	fillrectangle(getwidth() / 2 - 200, getheight() / 2 - 120, getwidth() / 2 + 200, getheight() / 2 + 150);

	settextstyle(40, 0, _T("����"));
	settextcolor(BLACK);

	setbkmode(TRANSPARENT); // �������ֱ���͸��

	//��ʾʤ��
	if (CheckBigBoardWinner() == BLACKPIECE)
	{
		outtextxy(getwidth() / 2 - textwidth(_T("�����ʤ!")) / 2, getheight() / 2 - 60, _T("�����ʤ!"));
	}
	else if (CheckBigBoardWinner() == WHITEPIECE)
	{
		outtextxy(getwidth() / 2 - textwidth(_T("�����ʤ!")) / 2, getheight() / 2 - 60, _T("�����ʤ!"));
	}
	else
	{
		outtextxy(getwidth() / 2 - textwidth(_T("˫������!")) / 2, getheight() / 2 - 60, _T("˫������!"));
	}

	settextstyle(30, 0, _T("����"));

	//�浵��ť
	setfillcolor(RGB(200, 230, 255));
	fillrectangle(getwidth() / 2 - 100, getheight() / 2 + 10, getwidth() / 2 + 100, getheight() / 2 + 50);
	setbkmode(TRANSPARENT); // �������ֱ���͸��
	outtextxy(getwidth() / 2 - textwidth(_T("�浵")) / 2, getheight() / 2 + 20, _T("�浵"));

	//�������˵���ť
	setfillcolor(RGB(200, 230, 255));
	fillrectangle(getwidth() / 2 - 100, getheight() / 2 + 50, getwidth() / 2 + 100, getheight() / 2 + 90);
	setbkmode(TRANSPARENT); // �������ֱ���͸��
	outtextxy(getwidth() / 2 - textwidth(_T("�������˵�")) / 2, getheight() / 2 + 60, _T("�������˵�"));
}

//���ƴ浵/��������
void DrawSaveLoadMenu(bool isSaving)
{
	cleardevice();

	//����ɫ
	setbkcolor(RGB(240, 240, 240));
	cleardevice();

	//����
	settextcolor(BLACK);
	settextstyle(50, 0, _T("����"));
	setbkmode(TRANSPARENT); // �������ֱ���͸��
	outtextxy(getwidth() / 2 - textwidth(isSaving ? _T("ѡ��浵��λ��") : _T("ѡ��浵����")) / 2,
		50, isSaving ? _T("ѡ��浵��λ��") : _T("ѡ��浵����"));

	//�浵��
	settextstyle(30, 0, _T("����"));
	for (int i = 0; i < MAX_SAVES; i++)
	{
		//�浵�۱���
		setfillcolor(i == currentSaveSlot ? RGB(180, 220, 255) : RGB(200, 230, 255));
		fillrectangle(getwidth() / 2 - 300, 150 + i * 100, getwidth() / 2 + 300, 220 + i * 100);

		//�浵��Ϣ
		if (!gameSaves[i].moves.empty())
		{
			//��ʾ�浵��ʱ��Ͳ���
			char timeStr[50];
			struct tm timeinfo;
			localtime_s(&timeinfo, &gameSaves[i].saveTime);
			strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", &timeinfo);

			wchar_t infoText[100];
			swprintf(infoText, 100, L"�浵 %d: %hs (%d��)", i + 1, timeStr, (int)gameSaves[i].moves.size());
			outtextxy(getwidth() / 2 - textwidth(infoText) / 2, 160 + i * 100, infoText);
		}
		else
		{
			//�մ浵
			TCHAR slotText[20];
			swprintf_s(slotText, _T("�浵 %d (��)"), i + 1);
			setbkmode(TRANSPARENT); // �������ֱ���͸��
			outtextxy(getwidth() / 2 - 50, 160 + i * 100, slotText);
		}
	}

	//���ذ�ť
	setfillcolor(RGB(200, 230, 255));
	fillrectangle(getwidth() / 2 - 100, getheight() - 100, getwidth() / 2 + 100, getheight() - 50);
	setbkmode(TRANSPARENT); // �������ֱ���͸��
	outtextxy(getwidth() / 2 - textwidth(_T("����")) / 2, getheight() - 90, _T("����"));

}

//���Ƹ��̽���
void DrawReplay()
{
	cleardevice();

	//����ɫ
	setbkcolor(RGB(240, 240, 240));
	cleardevice();

	//����
	settextcolor(BLACK);
	settextstyle(40, 0, _T("����"));
	setbkmode(TRANSPARENT); // �������ֱ���͸��
	outtextxy(getwidth() / 2 - textwidth(_T("�浵����")) / 2, 30, _T("�浵����"));

	//�������̣�����Ϸ������ͬ��
	int boardSize = min(getwidth(), getheight()) - 150;
	int startX = (getwidth() - boardSize) / 2;
	int startY = (getheight() - boardSize) / 2;

	setlinecolor(BLACK);

	//���ƴ����
	setlinestyle(PS_SOLID, 5);
	for (int i = 0; i <= 3; i++)
	{
		line(startX, startY + i * boardSize / 3, startX + boardSize, startY + i * boardSize / 3);
		line(startX + i * boardSize / 3, startY, startX + i * boardSize / 3, startY + boardSize);
	}

	//����С����
	setlinestyle(PS_SOLID, 2);
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			int smallStartX = startX + j * boardSize / 3;
			int smallStartY = startY + i * boardSize / 3;
			int smallCellSize = boardSize / 9;

			// ������С���Ѿ���Ӯ�ң������ɫ
			if (replayGame.smallBoards[i][j].blackWon || replayGame.smallBoards[i][j].whiteWon)
			{
				// ���˫����Ӯ�ˣ���ʾ�����ɫ
				if (replayGame.smallBoards[i][j].blackWon && replayGame.smallBoards[i][j].whiteWon)
				{
					setfillcolor(RGB(150, 150, 150));  // ��ɫ��ʾ˫����Ӯ��
				}
				else if (replayGame.smallBoards[i][j].blackWon)
				{
					setfillcolor(RGB(180, 180, 180));  // ��ɫ��ʾ����Ӯ��
				}
				else
				{
					setfillcolor(RGB(220, 220, 220));  // ǳɫ��ʾ����Ӯ��
				}
				fillrectangle(smallStartX, smallStartY, smallStartX + boardSize / 3, smallStartY + boardSize / 3);
			}

			//����С����
			for (int x = 0; x <= 3; x++)
			{
				line(smallStartX, smallStartY + x * smallCellSize, smallStartX + boardSize / 3, smallStartY + x * smallCellSize);
				line(smallStartX + x * smallCellSize, smallStartY, smallStartX + x * smallCellSize, smallStartY + boardSize / 3);
			}

			//��������
			GameSave tempGame;					//������ʱ��Ϸ״̬tempGame��ÿ�θ��̻���ʱ���³�ʼ��
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

	//���ư�ť
	settextstyle(25, 0, _T("����"));

	//��һ��
	setfillcolor(replayIndex > 0 ? RGB(200, 230, 255) : RGB(150, 150, 150));
	fillrectangle(getwidth() / 2 - 250, startY + boardSize + 20, getwidth() / 2 - 150, startY + boardSize + 60);
	setbkmode(TRANSPARENT); // �������ֱ���͸��
	outtextxy(getwidth() / 2 - 250 + (100 - textwidth(_T("��һ��"))) / 2, startY + boardSize + 25, _T("��һ��"));

	//��һ��
	setfillcolor(replayIndex < replayGame.moves.size() ? RGB(200, 230, 255) : RGB(150, 150, 150));
	fillrectangle(getwidth() / 2 - 50, startY + boardSize + 20, getwidth() / 2 + 50, startY + boardSize + 60);
	setbkmode(TRANSPARENT); // �������ֱ���͸��
	outtextxy(getwidth() / 2 - 50 + (100 - textwidth(_T("��һ��"))) / 2, startY + boardSize + 25, _T("��һ��"));

	//�˳�����
	setfillcolor(RGB(200, 230, 255));
	fillrectangle(getwidth() / 2 + 150, startY + boardSize + 20, getwidth() / 2 + 250, startY + boardSize + 60);
	setbkmode(TRANSPARENT); // �������ֱ���͸��
	outtextxy(getwidth() / 2 + 150 + (100 - textwidth(_T("�˳�����"))) / 2, startY + boardSize + 25, _T("�˳�����"));
}

//������������˵�
void HandleMainMenuClick(int x, int y)
{
	//��������ĸ���ť
	if (x >= getwidth() / 2 - 150 && x <= getwidth() / 2 + 150)
	{
		if (y >= 250 && y <= 310)			//��ʼ��Ϸ
		{
			InitNewGame();
			gameState = PLAYING;
			DrawGameBoard();
		}
		else if (y >= 350 && y <= 410)		//�浵����
		{
			gameState = SAVE_LOAD_MENU;
			isSaving = false;
			currentSaveSlot = 0;
			DrawSaveLoadMenu(false);
		}
		else if (y >= 450 && y <= 510)		//��Ϸ˵��
		{
			gameState = INSTRUCTIONS;
			DrawInstructions();
		}
		else if (y >= 550 && y <= 610)		//�˳���Ϸ
		{
			closegraph();
			exit(0);
		}
	}
}

//������Ϸҳ��ĵ��
void HandleGameClick(int x, int y)
{
	int boardSize = min(getwidth(), getheight()) - 100;
	int startX = (getwidth() - boardSize) / 2;
	int startY = (getheight() - boardSize) / 2;

	//������Ƿ���������
	if (x < startX || x >= startX + boardSize ||
		y < startY || y >= startY + boardSize)
	{
		return;
	}

	//�������Ĵ�����
	int boardCol = (x - startX) / (boardSize / 3);
	int boardRow = (y - startY) / (boardSize / 3);

	//����С���ڵ�����
	int cellX = (x - startX - boardCol * (boardSize / 3)) / (boardSize / 9);
	int cellY = (y - startY - boardRow * (boardSize / 3)) / (boardSize / 9);

	//������λ���Ƿ��Ѿ�������
	if (currentGame.smallBoards[boardRow][boardCol].cells[cellY][cellX] != EMPTY)
	{
		return;
	}

	//����Ƿ������ض�С����
	if (nextBoardX != -1 && nextBoardY != -1 &&
		(boardRow != nextBoardX || boardCol != nextBoardY))
	{
		return;
	}

	//��������
	currentGame.smallBoards[boardRow][boardCol].cells[cellY][cellX] = currentGame.currentPlayer;
	currentGame.moves.push_back(make_pair(boardRow * 3 + cellY, boardCol * 3 + cellX));

	//������С���Ƿ���Ӯ��
	CheckSmallBoardWinner(currentGame.smallBoards[boardRow][boardCol]);
	if (currentGame.smallBoards[boardRow][boardCol].blackWon)
	{
		currentGame.blackWins++;
	}
	if (currentGame.smallBoards[boardRow][boardCol].whiteWon)
	{
		currentGame.whiteWins++;
	}

	//�����Ϸ�Ƿ����
	if (CheckBigBoardWinner() != EMPTY || currentGame.moves.size() == 81)
	{
		gameState = GAME_OVER;
		DrawGameBoard();
		DrawGameover();
		return;
	}

	//�л����
	currentGame.currentPlayer = (currentGame.currentPlayer == BLACKPIECE) ? WHITEPIECE : BLACKPIECE;

	//ȷ����һ�������µ�С��
	nextBoardX = cellY;
	nextBoardY = cellX;

	//�����һ��С���Ѿ��������������������λ���������
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

//������ͣ�˵��ĵ��
void HandkePauseMenuClick(int x, int y)
{
	//��������ĸ���ť
	if (x >= getwidth() / 2 - 120 && x <= getwidth() / 2 + 120)
	{
		//������Ϸ
		if (y >= getheight() / 2 - 60 && y <= getheight() / 2 - 10)
		{
			gameState = PLAYING;
			DrawGameBoard();
		}
		//�������˵�
		else if (y >= getheight() / 2 + 10 && y <= getheight() / 2 + 60)
		{
			gameState = MAIN_MENU;
			DrawMainMenu();
		}
		//�浵
		else if (y >= getheight() / 2 + 80 && y <= getheight() / 2 + 130)
		{
			gameState = SAVE_LOAD_MENU;
			isSaving = true;
			currentSaveSlot = 0;
			DrawSaveLoadMenu(true);
		}
	}
}

//����浵/����������
void HandleSaveLoadMenuClick(int x, int y, bool isSaving)
{
	//���浵�۵��
	for (int i = 0; i < MAX_SAVES; i++)
	{
		if (x >= getwidth() / 2 - 300 && x <= getwidth() / 2 + 300 &&
			y >= 150 + i * 100 && y <= 220 + i * 100)
		{
			currentSaveSlot = i;

			if (isSaving)
			{
				//������Ϸ
				currentGame.saveTime = time(nullptr);
				gameSaves[currentSaveSlot] = currentGame;

				//���浽�ļ�
				ofstream file("saves.dat", ios::binary);
				if (file)
				{
					for (int i = 0; i < MAX_SAVES; i++)
					{
						//д������
						int moveCount = (int)gameSaves[i].moves.size();
						file.write((char*)&moveCount, sizeof(moveCount));

						//д��ÿһ��
						for (auto move : gameSaves[i].moves)
						{
							file.write((char*)&move.first, sizeof(move.first));
							file.write((char*)&move.second, sizeof(move.second));
						}

						//д��������Ϸ״̬
						file.write((char*)&gameSaves[i].currentPlayer, sizeof(gameSaves[i].currentPlayer));
						file.write((char*)&gameSaves[i].blackWins, sizeof(gameSaves[i].blackWins));
						file.write((char*)&gameSaves[i].whiteWins, sizeof(gameSaves[i].whiteWins));
						file.write((char*)&gameSaves[i].saveTime, sizeof(gameSaves[i].saveTime));

						//д��С��״̬
						for (int x = 0; x < 3; x++)
						{
							for (int y = 0; y < 3; y++)
							{
								file.write((char*)&gameSaves[i].smallBoards[x][y], sizeof(gameSaves[i].smallBoards[x][y]));
							}
						}
					}
				}

				MessageBox(GetHWnd(), _T("��Ϸ�Ѵ浵"), _T("��ʾ"), MB_OK);
				gameState = PLAYING;
				DrawGameBoard();
			}

			else
			{
				//���ش浵
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

	//��鷵�ذ�ť
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

//������Ϸ�������
void HandleGameOverClick(int x, int y)
{
	//����Ƿ����˴浵��ť
	if (x >= getwidth() / 2 - 100 && x <= getwidth() / 2 + 100)
	{
		//�浵
		if (y >= getheight() / 2 + 10 && y <= getheight() / 2 + 50)
		{
			gameState = SAVE_LOAD_MENU;
			isSaving = true;
			currentSaveSlot = 0;
			DrawSaveLoadMenu(true);
			return;
		}
		//�������˵�
		else if (y >= getheight() / 2 + 50 && y <= getheight() / 2 + 90)
		{
			gameState = MAIN_MENU;
			DrawMainMenu();
			return;
		}
	}
}

//������Ϸ˵�����
void HandleInstructionsClick(int x, int y)
{
	// ʹ�������ʱ��ͬ���������
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

//�����̽�����
void HandleReplayClick(int x, int y)
{
	int boardSize = min(getwidth(), getheight()) - 150;
	int startX = (getwidth() - boardSize) / 2;
	int startY = (getheight() - boardSize) / 2;

	//��������ĸ���ť
	if (y >= startY + boardSize + 20 && y <= startY + boardSize + 60)
	{
		//��һ��
		if (x >= getwidth() / 2 - 250 && x <= getwidth() / 2 - 150)
		{
			if (replayIndex > 0)
			{
				//ֻ�������һ��
				replayIndex--;

				//��ȡ���һ����λ��
				auto move = replayGame.moves[replayIndex];
				int boardRow = move.first / 3;
				int boardCol = move.second / 3;
				int cellRow = move.first % 3;
				int cellCol = move.second % 3;

				//�Ƴ����һ��������
				replayGame.smallBoards[boardRow][boardCol].cells[cellRow][cellCol] = EMPTY;

				//�������С����ʤ��״̬
				bool wasBlackWon = replayGame.smallBoards[boardRow][boardCol].blackWon;
				bool wasWhiteWon = replayGame.smallBoards[boardRow][boardCol].whiteWon;

				//�������С����ʤ��״̬
				replayGame.smallBoards[boardRow][boardCol].blackWon = false;
				replayGame.smallBoards[boardRow][boardCol].whiteWon = false;

				//���¼�����С����ʤ��
				CheckSmallBoardWinner(replayGame.smallBoards[boardRow][boardCol]);

				//������ʤ����
				if (wasBlackWon && !replayGame.smallBoards[boardRow][boardCol].blackWon)
				{
					replayGame.blackWins--;
				}
				if (wasWhiteWon && !replayGame.smallBoards[boardRow][boardCol].whiteWon)
				{
					replayGame.whiteWins--;
				}

				//�л����
				replayGame.currentPlayer = (replayGame.currentPlayer == BLACKPIECE) ? WHITEPIECE : BLACKPIECE;

				//�ػ����
				DrawReplay();
			}
		}

		//��һ��
		else if (x >= getwidth() / 2 - 50 && x <= getwidth() / 2 + 50)
		{
			if (replayIndex < replayGame.moves.size())
			{
				auto move = replayGame.moves[replayIndex];
				int boardRow = move.first / 3;
				int boardCol = move.second / 3;
				int cellRow = move.first % 3;
				int cellCol = move.second % 3;

				//��������
				replayGame.smallBoards[boardRow][boardCol].cells[cellRow][cellCol] = replayGame.currentPlayer;

				//������С���Ƿ���Ӯ��
				CheckSmallBoardWinner(replayGame.smallBoards[boardRow][boardCol]);
				if (replayGame.smallBoards[boardRow][boardCol].blackWon)
				{
					replayGame.blackWins++;
				}
				if (replayGame.smallBoards[boardRow][boardCol].whiteWon)
				{
					replayGame.whiteWins++;
				}

				//�л����
				replayGame.currentPlayer = (replayGame.currentPlayer == BLACKPIECE) ? WHITEPIECE : BLACKPIECE;

				replayIndex++;
				DrawReplay();
			}
		}

		//�������˵�
		else if (x >= getwidth() / 2 + 150 && x <= getwidth() / 2 + 250)
		{
			gameState = MAIN_MENU;
			DrawMainMenu();
		}
	}
}

//���ش浵
void LoadSaves()
{
	ifstream file("saves.dat", ios::binary);
	if (file)
	{
		for (int i = 0; i < MAX_SAVES; i++)
		{
			//��ȡ����
			int moveCount;
			file.read((char*)&moveCount, sizeof(moveCount));
			gameSaves[i].moves.resize(moveCount);

			//��ȡÿһ��
			for (int j = 0; j < moveCount; j++)
			{
				file.read((char*)&gameSaves[i].moves[j].first, sizeof(gameSaves[i].moves[j].first));
				file.read((char*)&gameSaves[i].moves[j].second, sizeof(gameSaves[i].moves[j].second));
			}

			//��ȡ������Ϸ״̬
			file.read((char*)&gameSaves[i].currentPlayer, sizeof(gameSaves[i].currentPlayer));
			file.read((char*)&gameSaves[i].blackWins, sizeof(gameSaves[i].blackWins));
			file.read((char*)&gameSaves[i].whiteWins, sizeof(gameSaves[i].whiteWins));
			file.read((char*)&gameSaves[i].saveTime, sizeof(gameSaves[i].saveTime));

			//��ȡС��״̬
			for (int x = 0; x < 3; x++)
			{
				for (int y = 0; y < 3; y++)
				{
					file.read((char*)&gameSaves[i].smallBoards[x][y], sizeof(gameSaves[i].smallBoards[x][y]));
				}
			}
		}
	}

	//��������
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
	//��ʼ��ͼ�δ���
	initgraph(1000, 700, SHOWCONSOLE);
	setbkcolor(WHITE);
	cleardevice();

	//��������
	LOGFONT f;
	gettextstyle(&f);
	_tcscpy_s(f.lfFaceName, _T("����"));
	f.lfQuality = ANTIALIASED_QUALITY;
	settextstyle(&f);

	//���ش浵
	LoadSaves();

	//�������˵�
	DrawMainMenu();

	//��Ϣѭ��
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

		//��ʱ
		Sleep(10);
	}

	closegraph();
	return 0;
}