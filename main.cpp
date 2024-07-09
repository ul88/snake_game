#include<iostream>
#include<fstream>
#include<string>
#include<ctime>
#include<string>
#include<vector>
#include<algorithm>
#include<queue>
#include<sstream>
#include<Windows.h>
#include<conio.h>
#include<mmsystem.h>

#pragma comment(lib, "winmm.lib")

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

enum object {
	OBJECT_SPACE,
	OBJECT_WALL,
	OBJECT_HEAD,
	OBJECT_TAIL,
	OBJECT_APPLE,
	OBJECT_BOOM,
	OBJECT_HALF_BOOM,
	OBJECT_EMPTY_BOOM
};

enum music {
	MAIN_MUSIC,
	GAME_MUSIC,
	GAME_OVER_MUSIC
};

struct Coordinate {
	int y;
	int x;
};

typedef struct BoomInfo {
	int x;
	int y;
	int time;
};

struct UserRanking {
	string name;
	int score;
};

struct SnakeObject {
	Coordinate head;
	queue <Coordinate> tail;
};

// 전역 변수
vector<UserRanking> userRank;
string userName = "";
bool music[4] = { 0,0,0,0 }; // 음악 환경 설정 true == 켜기, false == 끄기

void CursorView();
void gotoxy(int x,int y);
void InitFile();
void InitGame(object(*map)[COLS], int& headY, int& headX, int& tail, int mainY[ROWS * COLS], int mainX[ROWS * COLS]);
bool compare(UserRanking a, UserRanking b);
int KeyInput(int type);
void KeyInputGame(int& dir);
void LoginPage();
bool Move(object(*map)[COLS], int direction, int& headY, int& headX, int& tail,
	int mainY[ROWS * COLS], int mainX[ROWS * COLS], int& appleCnt, int& isApple, int& fieldAppleCnt);
void RandomApple(object(*map)[COLS], int& fieldAppleCnt);
void MapPrint(object(*map)[COLS], int appleCnt);
bool IsBoom(object(*map)[COLS], BoomInfo boomLocation[BOOM_SIZE]);
void RandomBoom(object(*map)[COLS], BoomInfo boomLocation[BOOM_SIZE]);
void StartGame();
void GameOverPage(int appleCnt);
int StartMenu();
int StartPage();
int RankCheck(int appleCnt);
void RankShow();
void SettingsShow();

int main()
{
	srand(time(NULL));
	
	int start = 0;

	system("title 스네이크 게임");
	system("mode con: lines=40 cols=50");

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
		else if (start == 3) {
			SettingsShow();
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
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

void gotoxy(int x, int y) {
	//x, y 좌표 설정
	COORD pos = { x,y };
	//커서 이동
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

bool compare(UserRanking a, UserRanking b) {
	return a.score > b.score;
}

void InitGame(object(*map)[COLS], SnakeObject& snake) {
	system("mode con: lines=60 cols=60");
	snake.head.y = 10;
	snake.head.x = 10;
	snake.tail.push({ snake.head.y,snake.head.x - 1 });
	/*mainY[0] = headY;
	mainX[0] = headX - 1;*/
	map[snake.head.y][snake.head.x] = OBJECT_HEAD;
	map[snake.tail.front().y][snake.tail.front().x] = OBJECT_TAIL;

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
	if (type == 2) {
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
					if (y > 11 && y<=14) {
						gotoxy(x - 2, y);
						cout << " ";
						gotoxy(x - 2, --y);
						cout << ">";
					}
					break;
				case DOWN:
					if (y >= 11 && y < 14) {
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
					else if (y == 13) {
						return 3;
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
		while (_kbhit()) _getch();
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
	else if (type == 4) {
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
					if (y > 11 && y <= 14) {
						gotoxy(x - 2, y);
						cout << " ";
						gotoxy(x - 2, --y);
						cout << ">";
					}
					break;
				case DOWN:
					if (y >= 11 && y < 14) {
						gotoxy(x - 2, y);
						cout << " ";
						gotoxy(x - 2, ++y);
						cout << ">";
					}
					break;
				case ENTER:
					if (y == 11) {
						music[0] = !music[0];
						return 1;
					}
					else if (y == 12) {
						music[1] = !music[1];
						return 1;
					}
					else if (y == 13) {
						music[2] = !music[2];
						return 1;
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
}

bool Move(object(*map)[COLS], int direction, SnakeObject& snake, int& appleCnt, int& isApple, int& fieldAppleCnt)
{
	map[snake.head.y][snake.head.x] = OBJECT_TAIL;
	snake.tail.push({ snake.head.y,snake.head.x });

	if (direction == RIGHT) (snake.head.x)++;
	else if (direction == LEFT) (snake.head.x)--;
	else if (direction == UP) (snake.head.y)--;
	else (snake.head.y)++;

	//사과를 먹었을 때
	if (map[snake.head.y][snake.head.x] == OBJECT_APPLE) {
		(appleCnt)++;
		--fieldAppleCnt;
	}
	//벽에 닿았거나 자신의 꼬리에 닿았을 때
	else if (map[snake.head.y][snake.head.x] == OBJECT_WALL || map[snake.head.y][snake.head.x] == OBJECT_TAIL
		|| map[snake.head.y][snake.head.x] == OBJECT_BOOM || map[snake.head.y][snake.head.x] == OBJECT_HALF_BOOM
		|| map[snake.head.y][snake.head.x] == OBJECT_EMPTY_BOOM) {
		return true;
	}
	else {
		map[snake.tail.front().y][snake.tail.front().x] = OBJECT_SPACE;
		snake.tail.pop();
	}

	map[snake.head.y][snake.head.x] = OBJECT_HEAD;
	
	return false;
}

void RandomApple(object(*map)[COLS], int& fieldAppleCnt) {
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

void MapPrint(object(*map)[COLS], int appleCnt) {
	cout << "현재 먹은 사과의 개수 : " << appleCnt << "\n";
	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLS; j++) {
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
	object map[ROWS][COLS] = { OBJECT_SPACE, };
	int appleCnt = 0, fieldAppleCnt = 0, isApple = 0, headX = 0, headY = 0, tail = 0;
	bool gameEnd = false;
	SnakeObject snake;
	int* mainX = new int[ROWS * COLS], * mainY = new int[ROWS * COLS];
	BoomInfo* boomLocation = new BoomInfo[BOOM_SIZE] {};
	bool isBoom = true;
	char key;
	int dir = RIGHT;

	InitGame(map, snake);
	if (music[GAME_MUSIC]) {
		PlaySoundA("game_music.wav", NULL, SND_FILENAME | SND_ASYNC);
	}
	else {
		PlaySound(nullptr, nullptr, 0);
	}

	while (1) {
		if (fieldAppleCnt <= 0) {
			RandomApple(map, fieldAppleCnt);
		}
		if (IsBoom(map ,boomLocation)) {
			RandomBoom(map, boomLocation);
		}

		KeyInputGame(dir);

		gameEnd = Move(map, dir, snake, appleCnt, isApple, fieldAppleCnt);

		MapPrint(map, appleCnt);
		if (gameEnd) {
			
			//파일 쓰기
			ofstream file("rank_data.txt", ios::app);
			if (file.is_open()) {
				string line = userName + " " + to_string(appleCnt)+"\n";
				file << line;
				file.close();
			}
			
			GameOverPage(appleCnt);

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
	if (music[MAIN_MUSIC]) {
		PlaySoundA("main_music.wav", NULL, SND_FILENAME | SND_ASYNC);
	}
	else {
		PlaySound(nullptr, nullptr, 0);
	}
	int x = 21;
	int y = 11;
	gotoxy(x - 2, y);
	cout << "> START";
	gotoxy(x, y + 1);
	cout << "RANK";
	gotoxy(x, y + 2);
	cout << "SETTINGS";
	gotoxy(x, y + 3);
	cout << "END";

	return KeyInput(2);
}

void LoginPage() {
	cout << "\n\n";
	cout << "                  USER NAME :\n                  ";
	cin >> userName;
	CursorView();
	
	system("cls");
	cout << "\n\n";
	cout << "                " << userName << "님 환영합니다!" << "\n";
	while (1) {
		gotoxy(16, 3);
		cout << "엔터키를 누르시오" << "\n";
		if (KeyInput(3)) break;
		Sleep(350);
		if (KeyInput(3)) break;
		gotoxy(16, 3);
		cout << "                     " << "\n";
		Sleep(350);
		if (KeyInput(3)) break;
	}
}

int StartPage() {
	cout << "\n\n";
	cout << "    #####    ##    #       #       #  #   #####" << "\n";
	cout << "   #         # #   #      # #      # #    #" << "\n";
	cout << "    #####    #  #  #     #   #     ##     #####" << "\n";
	cout << "         #   #   # #    #######    # #    #" << "\n";
	cout << "    #####    #    ##   #       #   #  #   #####" << "\n";
	cout << "           방향키로 선택해주세요.\n";
	cout << "    엔터를 누르시면 커서의 위치에서 시작됩니다.\n";
	return StartMenu();
}

bool IsBoom(object(*map)[COLS], BoomInfo boomLocation[BOOM_SIZE]) {
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

void RandomBoom(object(*map)[COLS], BoomInfo boomLocation[BOOM_SIZE]) {
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
		cout << "        #####      #      ##    #   #  #" << "\n";
		if (KeyInput(3)) return;
		Sleep(time);
		cout << "        #   #     # #     # #   #   # #" << "\n";
		if (KeyInput(3)) return;
		Sleep(time);
		cout << "        #####    #   #    #  #  #   ##" << "\n";
		if (KeyInput(3)) return;
		Sleep(time);
		cout << "        #  #    #######   #   # #   # #" << "\n";
		if (KeyInput(3)) return;
		Sleep(time);
		cout << "        #   #  #       #  #    ##   #  #" << "\n";
		if (KeyInput(3)) return;
		Sleep(time);
		cout << "        ---------------------------------"<<"\n";
		if (KeyInput(3)) return;
		Sleep(time);
		cout << "        | 순 위 |      이름     | 점 수 |" << "\n";
		if (KeyInput(3)) return;
		Sleep(time);
		cout << "        ---------------------------------" << "\n";
		if (KeyInput(3)) return;
		Sleep(time);
		int y = 8;
		int rank = 1;
		for (int i = 0; i < userRank.size(); i++) {
			if (i != 0) {
				if (userRank[i].score != userRank[i - 1].score) rank++;
			}
			if (KeyInput(3)) return;
			Sleep(time);
			gotoxy(8, y);
			//cout << "| " << i + 1;
			printf("| %3d", rank);
			gotoxy(16, y);
			cout << "| " << userRank[i].name;
			gotoxy(32, y);
			//cout << "| " << userRank[i].score;
			printf("| %3d", userRank[i].score);
			gotoxy(40, y);
			cout << "|" << "\n";
			if (KeyInput(3)) return;
			Sleep(time);
			cout << "        ---------------------------------" << "\n";
			y += 2;
		}

		if (KeyInput(3)) return;
		Sleep(1500);
	}
}

void GameOverPage(int appleCnt) {
	if (music[GAME_OVER_MUSIC]) {
		PlaySoundA("game_over.wav", 0, SND_FILENAME | SND_ASYNC);
	}
	else {
		PlaySound(nullptr, nullptr, 0);
	}


	//화면 사선으로 지우기
	Sleep(500);
	gotoxy(0, 0);
	cout << "                           ";
	for (int i = 0; i < COLS*2; i++) {
		int x = i;
		int y = 1;
		while (x >= 0 && x <= COLS*2 && y >= 0 && y <= ROWS) {
			gotoxy(x, y);
			cout << " ";
			x--,y++;
		}
		Sleep(10);
	}
	for (int i = 1; i <= ROWS; i++) {
		int x = COLS*2-1;
		int y = i;
		while (x >= 0 && x <= COLS * 2 && y >= 0 && y <= ROWS) {
			gotoxy(x, y);
			cout << " ";
			x--, y++;
		}
		Sleep(10);
	}

	system("cls");
	gotoxy(0, 8);
	cout << "총 먹은 사과의 개수 : " << appleCnt << "개 입니다.\n";
	userRank.push_back({ userName,appleCnt });
	sort(userRank.begin(), userRank.end(), compare);
	cout << userName << "님의 순위는 " << RankCheck(appleCnt) << "위 입니다.\n";
	cout << "엔터 키를 누르시면 시작 화면으로 돌아갑니다.";
	while (_kbhit()) _getch();
	while (1) {
		gotoxy(0, 0);
		cout << "\n\n";
		cout << "#####    ##    #    ####" << "\n";
		cout << "#        # #   #    #   #" << "\n";
		cout << "#####    #  #  #    #   #" << "\n";
		cout << "#        #   # #    #   #" << "\n";
		cout << "#####    #    ##    ####" << "\n\n";
		Sleep(600);
		if (KeyInput(3)) return;
		gotoxy(0, 0);
		if (KeyInput(3)) return;
		cout << "\n\n";
		cout << "                         " << "\n";
		cout << "                         " << "\n";
		cout << "                         " << "\n";
		cout << "                         " << "\n";
		cout << "                         " << "\n";
		Sleep(600);
		if (KeyInput(3)) return;	
	}
}

void SettingsShow() {
	int x = 21;
	int y = 11;
	gotoxy(x - 2, y);
	cout << "         ";
	gotoxy(x - 2, y + 1);
	cout << "         ";
	gotoxy(x - 2, y + 2);
	cout << "         ";
	gotoxy(x - 2, y + 3);
	cout << "         ";
	gotoxy(x - 2, y);
	cout << "> 메인 음악 " << (music[0] ? "끄기" : "켜기");
	gotoxy(x, y + 1);
	cout << "인 게임 음악 " << (music[1] ? "끄기" : "켜기");
	gotoxy(x, y + 2);
	cout << "사망시 효과음 " << (music[2] ? "끄기" : "켜기");
	gotoxy(x, y + 3);
	cout << "뒤로가기";
	if (KeyInput(4)) {
		SettingsShow();
	}
	else {
		return;
	}
}