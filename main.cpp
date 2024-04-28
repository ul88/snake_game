#include<iostream>
#include<fstream>
#include<string>
#include<ctime>
#include<string>
#include<vector>
#include<algorithm>
#include<sstream>

#include<windows.h>
#include<conio.h>

#define ENTER 13
#define ESC 27
#define RIGHT 77
#define LEFT 75
#define UP 72
#define DOWN 80

#define ROWS 25
#define COLS 25

#define BOOM_SIZE 4
#define BOOM_HOLDING_TIME 30

using namespace std;

typedef enum object {
	OBJECT_SPACE,
	OBJECT_WALL,
	OBJECT_HEAD,
	OBJECT_TAIL,
	OBJECT_APPLE,
	OBJECT_BOOM,
	OBJECT_HALF_BOOM,
	OBJECT_EMPTY_BOOM
} object_t;

typedef struct BoomInformation {
	int x;
	int y;
	int time;
}boomInfo_t;

typedef struct UserRanking {
	string name;
	int score;
}userRanking_t;

// 전역 변수
vector<userRanking_t> userRank;
string userName = "";

void CursorView();
void gotoxy(int x,int y);
void InitFile();
void InitGame(object_t(*map)[COLS], int& headY, int& headX, int& tail, int mainY[ROWS * COLS], int mainX[ROWS * COLS]);
bool compare(userRanking_t a, userRanking_t b);
int KeyInput(int type);
void KeyInputGame(int& dir);
void LoginPage();
bool Move(object_t(*map)[COLS], int direction, int& headY, int& headX, int& tail,
	int mainY[ROWS * COLS], int mainX[ROWS * COLS], int& appleCnt, int& isApple, int& fieldAppleCnt);
void RandomApple(object_t(*map)[COLS], int& fieldAppleCnt);
void MapPrint(object_t(*map)[COLS], int appleCnt);
bool IsBoom(object_t(*map)[COLS], boomInfo_t boomLocation[BOOM_SIZE]);
void RandomBoom(object_t(*map)[COLS], boomInfo_t boomLocation[BOOM_SIZE]);
void StartGame();
int StartMenu();
int StartPage();
int RankCheck(int appleCnt);
void RankShow();

int main()
{
	ios::sync_with_stdio(0);
	srand(time(NULL));
	
	int start = 0;
	InitFile();
	LoginPage();
	while (1) {
		system("cls");
		start = StartPage();
		if (start == 1) {
			system("cls");
			StartGame();
		}
		else if (start == 2) {
			RankShow();
		}
		else {
			break;
		}
	}
	return 0;
}

void InitFile() {
	//파일 읽기
	ifstream file("rank_data.txt");
	if (file.is_open()) {
		string line;
		while (getline(file, line)) {
			string name, score;
			stringstream temp(line);
			temp >> name >> score;
			userRank.push_back({ name,stoi(score) });
		}
		sort(userRank.begin(), userRank.end(), compare);
		file.close();
	}
}

void CursorView() {
	CONSOLE_CURSOR_INFO cursorInfo;
	cursorInfo.dwSize = 1;
	cursorInfo.bVisible = false; //커서 Visible TRUE(보임) FALSE(숨김)
	system("title 스네이크 게임");
	system("mod con: lines=1000 cols=1000");
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

void gotoxy(int x, int y) {
	//x, y 좌표 설정
	COORD pos = { x,y };
	//커서 이동
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

bool compare(userRanking_t a, userRanking_t b) {
	return a.score > b.score;
}

void InitGame(object_t(*map)[COLS], int& headY, int& headX, int& tail, int mainY[ROWS * COLS], int mainX[ROWS * COLS]) {
	
	headY = 10;
	headX = 10;
	tail = 1;
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

void KeyInputGame(int& dir) {
	char key;
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
			gotoxy(0, 26);
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
}

int KeyInput(int type) {
	char key;
	if (type == 1) {
		if (_kbhit()) {
			while (_kbhit()) _getch();
		}
		key = _getch();
		if (key == -32) {
			key = _getch();
		}
		switch (key) {
		case ENTER:
			system("cls");
			return 0;
			break;
		default:
			break;
		}
		return 1;
	}
	else if (type == 2) {
		int x = 21;
		int y = 11;
		while (1) {
			if (_kbhit()) {
				key = _getch();
				if (key == -32) {
					key = _getch();
				}
				switch (key) {
				case UP:
					if (y > 11 && y<=13) {
						gotoxy(x - 2, y);
						cout << " ";
						gotoxy(x - 2, --y);
						cout << ">";
					}
					break;
				case DOWN:
					if (y >= 11 && y < 13) {
						gotoxy(x - 2, y);
						cout << " ";
						gotoxy(x - 2, ++y);
						cout << ">";
					}
					break;
				case ENTER:
					if (y == 11) {
						return 1;
					}
					else if (y == 12) {
						return 2;
					}
					else {
						return 0;
					}
					break;
				default:
					break;
				}
			}
		}
	}
	else if (type == 3) {
		if (!_kbhit()) return 0;
		key = _getch();
		if (key == -32) {
			key = _getch();
		}
		switch (key) {
		case ENTER:
			system("cls");
			return 1;
			break;
		default:
			break;
		}
		return 0;
	}
}

bool Move(object_t(*map)[COLS], int direction, int& headY, int& headX, int& tail,
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
		--fieldAppleCnt;

		mainY[tail - 1] = mainY[tail - 2] - (mainY[tail - 2] - (tail - 3 >= 0 ? mainY[tail - 3] : headY));
		mainX[tail - 1] = mainX[tail - 2] - (mainX[tail - 2] - (tail - 3 >= 0 ? mainX[tail - 3] : headX));

		map[mainY[tail - 1]][mainX[tail - 1]] = OBJECT_TAIL;
	}
	//벽에 닿았거나 자신의 꼬리에 닿았을 때
	else if (map[headY][headX] == OBJECT_WALL || map[headY][headX] == OBJECT_TAIL 
		|| map[headY][headX] == OBJECT_BOOM || map[headY][headX] == OBJECT_HALF_BOOM
		|| map[headY][headX] == OBJECT_EMPTY_BOOM) {
		return true;
	}

	map[headY][headX] = OBJECT_HEAD;
	return false;
}

void RandomApple(object_t(*map)[COLS], int& fieldAppleCnt) {
	int randX = 0;
	int randY = 0;
	int temp = rand() % 5 + 1; // 한 번에 생성될 사과 개수
	int appleCnt = temp;
	while (appleCnt) {
		randY = rand() % ROWS;
		randX = rand() % COLS;
		if (map[randY][randX] == OBJECT_SPACE) {
			map[randY][randX] = OBJECT_APPLE;
			appleCnt--;
		}
	}

	fieldAppleCnt = temp;
}

void MapPrint(object_t(*map)[COLS], int appleCnt) {
	cout << "현재 먹은 사과의 개수 : " << appleCnt << "\n";
	for (int i = 0; i < ROWS; ++i) {
		for (int j = 0; j < COLS; ++j) {
			switch (map[i][j]) {
			case OBJECT_SPACE:
				cout << ". ";
				break;
			case OBJECT_WALL:
				cout << "# ";
				break;
			case OBJECT_HEAD:
				cout << "O ";
				break;
			case OBJECT_TAIL:
				cout << "o ";
				break;
			case OBJECT_APPLE:
				cout << "A ";
				break;
			case OBJECT_BOOM:
				cout << "● ";
				break;
			case OBJECT_HALF_BOOM:
				cout << "◎ ";
				break;
			case OBJECT_EMPTY_BOOM:
				cout << "⊙ ";
				break;
			default:
				break;
			}
		}
		cout << "\n";
	}
}

void StartGame() {
	object_t map[ROWS][COLS] = { OBJECT_SPACE, };
	int appleCnt = 0, fieldAppleCnt = 0, isApple = 0, headX = 0, headY = 0, tail = 0;
	bool gameEnd = false;
	int* mainX = new int[ROWS * COLS], * mainY = new int[ROWS * COLS];
	boomInfo_t* boomLocation = new boomInfo_t[BOOM_SIZE] {};
	bool isBoom = true;
	char key;
	int dir = RIGHT;

	InitGame(map, headY, headX, tail, mainY, mainX);

	while (1) {
		if (fieldAppleCnt <= 0) {
			RandomApple(map, fieldAppleCnt);
		}
		if (IsBoom(map ,boomLocation)) {
			RandomBoom(map, boomLocation);
		}

		KeyInputGame(dir);

		gameEnd = Move(map, dir, headY, headX, tail, mainY, mainX, appleCnt, isApple, fieldAppleCnt);

		MapPrint(map, appleCnt);
		if (gameEnd) {
			cout << "사망하셨습니다.\n";
			Sleep(3000);
			system("cls");
			cout << "\n\n";
			cout << "#####    ##    #    ####" << "\n";
			cout << "#        # #   #    #   #" << "\n";
			cout << "#####    #  #  #    #   #" << "\n";
			cout << "#        #   # #    #   #" << "\n";
			cout << "#####    #    ##    ####" << "\n\n";
			cout << "총 먹은 사과의 개수 : " << appleCnt << "개 입니다.\n";
			userRank.push_back({userName,appleCnt});
			sort(userRank.begin(), userRank.end(),compare);
			cout << userName << "님의 순위는 " << RankCheck(appleCnt) << "입니다.\n";
			
			//파일 쓰기
			ofstream file("rank_data.txt", ios::app);
			if (file.is_open()) {
				string line = userName + " " + to_string(appleCnt)+"\n";
				file << line;
				file.close();
			}

			cout << "엔터 키를 누르시면 시작 화면으로 돌아갑니다.";
			while (KeyInput(1));
			break;
		}
		Sleep(200);
		gotoxy(0, 0);
	}

	delete[] mainX;
	delete[] mainY;
	delete[] boomLocation;
}

int StartMenu() {
	int x = 21;
	int y = 11;
	gotoxy(x - 2, y);
	cout << "> START";
	gotoxy(x, y + 1);
	cout << "RANK";
	gotoxy(x, y + 2);
	cout << "END";

	return KeyInput(2);
}

void LoginPage() {
	cout << "\n\n";
	cout << "USER NAME : ";
	cin >> userName;
	CursorView();
	system("cls");
	cout << "\n\n";
	cout << "    " << userName << "님 환영합니다!" << "\n";
	while (1) {
		gotoxy(4, 3);
		cout << "엔터키를 누르시오" << "\n";
		if (KeyInput(3)) break;
		Sleep(350);
		if (KeyInput(3)) break;
		gotoxy(4, 3);
		cout << "                " << "\n";
		Sleep(350);
		if (KeyInput(3)) break;
	}
}

int StartPage() {
	cout << "\n\n";
	cout << "#####    ##    #       #       #  #    #####" << "\n";
	cout << "#        # #   #      # #      # #     #" << "\n";
	cout << "#####    #  #  #     #   #     ##      #####" << "\n";
	cout << "     #   #   # #    #######    # #     #" << "\n";
	cout << "#####    #    ##   #       #   #  #    #####" << "\n";
	cout << "           방향키로 선택해주세요.\n";
	cout << "    엔터를 누르시면 커서의 위치에서 시작됩니다.\n";
	return StartMenu();
}

bool IsBoom(object_t(*map)[COLS], boomInfo_t boomLocation[BOOM_SIZE]) {
	bool flag = true;
	for (int i = 0; i < BOOM_SIZE; i++) {
		if (boomLocation[i].time != 0) {
			flag = false;
			boomLocation[i].time--;
			if (boomLocation[i].time == BOOM_HOLDING_TIME / 3 * 2) {
				map[boomLocation[i].y][boomLocation[i].x] = OBJECT_HALF_BOOM;
			}
			else if (boomLocation[i].time == BOOM_HOLDING_TIME / 3) {
				map[boomLocation[i].y][boomLocation[i].x] = OBJECT_EMPTY_BOOM;
			}
			else if (boomLocation[i].time == 0) {
				map[boomLocation[i].y][boomLocation[i].x] = OBJECT_SPACE;
			}
		}
	}
	return flag;
}

void RandomBoom(object_t(*map)[COLS], boomInfo_t boomLocation[BOOM_SIZE]) {
	int createBoomCnt = rand() % BOOM_SIZE;
	for (int i = 0; i < createBoomCnt; i++) {
		while (1) {
			int randY = rand() % ROWS;
			int randX = rand() % COLS;
			if (map[randY][randX] == OBJECT_SPACE) {
				map[randY][randX] = OBJECT_BOOM;
				boomLocation[i].y = randY;
				boomLocation[i].x = randX;
				break;
			}
		}
		boomLocation[i].time = BOOM_HOLDING_TIME;
	}
}

int RankCheck(int appleCnt) {
	int rank = 1;
	for (int i = 0; i < userRank.size(); i++) {
		if (i > 1) {
			if (userRank[i].score != userRank[i - 1].score) rank++;
		}
		if (userRank[i].score == appleCnt) {
			return rank;
		}
	}
}

void RankShow() {
	while (1) {
		int time = 150;
		system("cls");
		cout << "#####      #      ##    #    #  #" << "\n";
		if (KeyInput(3)) break;
		Sleep(time);
		cout << "#   #     # #     # #   #    # #" << "\n";
		if (KeyInput(3)) break;
		Sleep(time);
		cout << "#####    #   #    #  #  #    ##" << "\n";
		if (KeyInput(3)) break;
		Sleep(time);
		cout << "#  #    #######   #   # #    # #" << "\n";
		if (KeyInput(3)) break;
		Sleep(time);
		cout << "#   #  #       #  #    ##    #  #" << "\n";
		if (KeyInput(3)) break;
		Sleep(time);
		cout << "---------------------------------"<<"\n";
		if (KeyInput(3)) break;
		Sleep(time);
		cout << "| 순 위 |      이름     | 점 수 |" << "\n";
		if (KeyInput(3)) break;
		Sleep(time);
		cout << "---------------------------------" << "\n";
		if (KeyInput(3)) break;
		Sleep(time);
		int y = 8;
		int rank = 1;
		for (int i = 0; i < userRank.size(); i++) {
			if (i != 0) {
				if (userRank[i].score != userRank[i - 1].score) rank++;
			}
			if (KeyInput(3)) break;
			Sleep(time);
			gotoxy(0, y);
			//cout << "| " << i + 1;
			printf("| %3d", rank);
			gotoxy(8, y);
			cout << "| " << userRank[i].name;
			gotoxy(24, y);
			//cout << "| " << userRank[i].score;
			printf("| %3d", userRank[i].score);
			gotoxy(32, y);
			cout << "|" << "\n";
			if (KeyInput(3)) break;
			Sleep(time);
			cout << "---------------------------------" << "\n";
			y += 2;
		}
		if (KeyInput(3)) break;
		Sleep(1500);
	}
}