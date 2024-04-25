#include<iostream>
#include<string>
#include<ctime>
#include<Windows.h>
#include<conio.h>

#define ESC 27

#define ROWS 25
#define COLS 25

using namespace std;

typedef enum object {
	OBJECT_SPACE,
	OBJECT_WALL,
	OBJECT_HEAD,
	OBJECT_TAIL,
	OBJECT_APPLE
} object_t;

typedef enum Keyboard {
	RIGHT,
	LEFT,
	UP,
	DOWN
} keyboard_t;

void cursorView() {
	CONSOLE_CURSOR_INFO cursorInfo;
	cursorInfo.dwSize = 1;
	cursorInfo.bVisible = false; //커서 Visible TRUE(보임) FALSE(숨김)
	system("title 스네이크 게임");
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

void gotoxy(int x, int y) {
	//x, y 좌표 설정
	COORD pos = { x,y };
	//커서 이동
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void Init(object_t(*map)[COLS], int& headY, int& headX, int& Tail, int mainY[ROWS * COLS], int mainX[ROWS*COLS]) {
	cursorView();
	headY = 10;
	headX = 10;
	Tail = 1;
	mainY[0] = headY;
	mainX[0] = headX - 1;
	map[headY][headX] = OBJECT_HEAD;
	map[mainY[0]][mainX[0]] = OBJECT_TAIL;

	//벽 세우기
	for (int i = 0; i < ROWS; i++) {
		map[i][0] = OBJECT_WALL;
	}
	for (int i = 0; i < ROWS; i++) {
		map[i][COLS - 1] = OBJECT_WALL;
	}
	for (int i = 0; i < COLS; i++) {
		map[0][i] = OBJECT_WALL;
	}
	for (int i = 0; i < COLS; i++) {
		map[ROWS - 1][i] = OBJECT_WALL;
	}
}

bool Move(object_t(*map)[COLS], keyboard_t direction, int& headY, int& headX, int& tail,
	int mainY[ROWS * COLS], int mainX[ROWS * COLS], int& appleCnt, int& isApple, int& fieldAppleCnt)
{
	map[mainY[tail - 1]][mainX[tail - 1]] = OBJECT_SPACE;
	for (int i = tail - 1; i > 0; i--) {
		mainY[i] = mainY[i - 1];
		mainX[i] = mainX[i - 1];
		map[mainY[i]][mainX[i]] = OBJECT_TAIL;
	}

	mainY[0] = headY;
	mainX[0] = headX;
	map[mainY[0]][mainX[0]] = OBJECT_TAIL;

	if (direction == RIGHT) (headX)++;
	else if (direction == LEFT) (headX)--;
	else if (direction == UP) (headY)--;
	else (headY)++;

	//사과를 먹었을 때
	if (map[headY][headX] == OBJECT_APPLE) {
		(tail)++;
		(appleCnt)++;
		if (--fieldAppleCnt == 0) {
			isApple = 0;
		}

		mainY[tail - 1] = mainY[tail - 2] - (mainY[tail - 2] - (tail - 3 >= 0 ? mainY[tail - 3] : headY));
		mainX[tail - 1] = mainX[tail - 2] - (mainX[tail - 2] - (tail - 3 >= 0 ? mainX[tail - 3] : headX));

		map[mainY[tail - 1]][mainX[tail - 1]] = OBJECT_TAIL;
	}
	//벽에 닿았거나 자신의 꼬리에 닿았을 때
	else if (map[headY][headX] == OBJECT_WALL || map[headY][headX] == OBJECT_TAIL) {
		return true;
	}

	map[headY][headX] = OBJECT_HEAD;
	return false;
}

void RandomApple(object_t(*map)[COLS], int& fieldAppleCnt) {
	int randX = 0;
	int randY = 0;
	int temp = rand()%5+1; // 한 번에 생성될 사과 개수
	int appleCnt = temp;
	while (appleCnt) {
		randX = rand() % ROWS;
		randY = rand() % COLS;
		if (map[randX][randY] == OBJECT_SPACE) {
			map[randX][randY] = OBJECT_APPLE;
			appleCnt--;
		}
	}

	fieldAppleCnt = temp;
}

void MapPrint(object_t(*map)[COLS], int appleCnt) {
	cout<<"현재 먹은 사과의 개수 : "<<appleCnt<<"\n";
	for (int i = 0; i < ROWS; ++i) {
		for (int j = 0; j < COLS; ++j) {
			switch (map[i][j]) {
			case OBJECT_SPACE:
				cout<<". ";
				break;
			case OBJECT_WALL:
				cout<<"# ";
				break;
			case OBJECT_HEAD:
				cout<<"O ";
				break;
			case OBJECT_TAIL:
				cout<<"o ";
				break;
			case OBJECT_APPLE:
				cout<<"A ";
				break;
			default:
				break;
			}
		}
		cout<<"\n";
	}
}

void gameStart() {
	object_t map[ROWS][COLS] = { OBJECT_SPACE, };
	int appleCnt = 0, fieldAppleCnt = 0, isApple = 0, headX = 0, headY = 0, tail = 0;
	bool gameEnd = false;
	int* mainX = new int[ROWS * COLS], * mainY = new int[ROWS * COLS];
	char key;
	keyboard_t dir = RIGHT;

	Init(map, headY, headX, tail, mainY, mainX);

	while (1) {
		if (isApple == 0) {
			RandomApple(map, fieldAppleCnt);
			isApple = 1;
		}

		if (_kbhit()) {
			key = _getch();
			if (key == -32) {
				key = _getch();
			}
			switch (key) {
			case 77:
				if (dir == LEFT);
				else dir = RIGHT;
				break;
			case 75:
				if (dir == RIGHT);
				else dir = LEFT;
				break;
			case 72:
				if (dir == DOWN);
				else dir = UP;
				break;
			case 80:
				if (dir == UP);
				else dir = DOWN;
				break;
			case ESC:
				cout << "일시 정지화면입니다." << "\n";
				system("pause");
				system("cls");
			default:
				break;
			}
		}

		if (_kbhit()) {
			while (_kbhit()) _getch();
		}

		gameEnd = Move(map, dir, headY, headX, tail, mainY, mainX, appleCnt, isApple, fieldAppleCnt);

		MapPrint(map, appleCnt);
		if (gameEnd) {
			cout << "사망하셨습니다.\n";
			Sleep(3000);
			system("cls");			
			cout << "총 먹은 사과의 개수 : " << appleCnt << "개 입니다.\n";
			cout << "엔터 키를 누르시면 시작 화면으로 돌아갑니다.";
			while (1) {
				key = _getch();
				if (key == -32) {
					key = _getch();
				}
				switch (key) {
				case 13: //ENTER
					system("cls");
					return;
					break;
				default:
					break;
				}
			}
		}
		Sleep(200);
		gotoxy(0, 0);
	}



	delete[] mainX;
	delete[] mainY;
}

bool startMenu() {
	int x = 20;
	int y = 11;
	gotoxy(x - 2, y);
	cout << "> START";
	gotoxy(x, y+1);
	cout << " END";

	char key;
	while (1) {
		if (_kbhit()) {
			key = _getch();
			if (key == -32) {
				key = _getch();
			}
			switch (key) {
			case 72: //UP
				if (y == 12) {
					gotoxy(x - 2, y);
					cout <<" ";
					gotoxy(x - 2, --y);
					cout << ">";
				}
				break;
			case 80: // DOWN
				if (y == 11) {
					gotoxy(x - 2, y);
					cout << " ";
					gotoxy(x - 2, ++y);
					cout << ">";
				}
				break;
			case 13: // ENTER
				if (y == 11) {
					return true;
				}
				else {
					return false;
				}
				break;
			default:
				break;
			}
		}
	}
	return false;
}

bool startPage() {
	cout << "\n\n";
	cout << "######   ##    #      #      #  #   #####" << "\n";
	cout << "#        # #   #     # #     # #    #" << "\n";
	cout << "#####    #  #  #    #   #    ##     #####" << "\n";
	cout << "     #   #   # #   #######   # #    #" << "\n";
	cout << "#####    #    ##  #       #  #  #   #####" << "\n";
	cout << "          방향키로 선택해주세요.\n";
	cout << "   엔터를 누르시면 커서의 위치에서 시작됩니다.\n";
	return startMenu();
}

int main()
{
	ios::sync_with_stdio(0);
	srand(time(NULL));
	
	bool start = 0;
	while (1) {
		system("cls");
		start = startPage();
		if (start) {
			system("cls");
			gameStart();
			
		}
		else {
			break;
		}
	}
	return 0;
}