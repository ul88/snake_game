#include<stdio.h>
#include<stdbool.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include<windows.h>
#include<conio.h>

#define ROWS 25
#define COLS 25

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

void gotoxy(int x, int y) {
	//x, y 좌표 설정
	COORD pos = { x,y };
	//커서 이동
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void Init(object_t (*map)[COLS],int *headY, int *headX,int *Tail, int* mainY, int* mainX) {
	*headY = 10;
	*headX = 10;
	*Tail = 1;
	mainY[0] = *headY;
	mainX[0] = *headX - 1;
	map[*headY][*headX] = OBJECT_HEAD;
	map[mainY[0]][mainX[0]] = OBJECT_TAIL;
	
	//벽 세우기
	for (int i = 0; i < ROWS; i++) {
		map[i][0] = OBJECT_WALL;
	}
	for (int i = 0; i < ROWS; i++) {
		map[i][COLS-1] = OBJECT_WALL;
	}
	for (int i = 0; i < COLS; i++) {
		map[0][i] = OBJECT_WALL;
	}
	for (int i = 0; i < COLS; i++) {
		map[ROWS-1][i] = OBJECT_WALL;
	}
}

bool Move(object_t(*map)[COLS],keyboard_t direction, int* headY, int* headX, int* tail,
	int *mainY,int *mainX, int *appleCnt, int* isApple) 
{
	map[mainY[*tail - 1]][mainX[*tail - 1]] = OBJECT_SPACE;
	for (int i = *tail-1; i > 0; i--) {
		mainY[i] = mainY[i-1];
		mainX[i] = mainX[i-1];
		map[mainY[i]][mainX[i]] = OBJECT_TAIL;
	}

	mainY[0] = *headY;
	mainX[0] = *headX;
	map[mainY[0]][mainX[0]] = OBJECT_TAIL;

	if (direction == RIGHT) (*headX)++;
	else if (direction == LEFT) (*headX)--;
	else if (direction == UP) (*headY)--;
	else (*headY)++;

	//사과를 먹었을 때
	if (map[*headY][*headX] == OBJECT_APPLE) {
		(*tail)++;
		(*appleCnt)++;
		*isApple = 0;

		mainY[*tail - 1] = mainY[*tail - 2] - (mainY[*tail - 2] - (*tail - 3 >= 0 ? mainY[*tail - 3] : *headY));
		mainX[*tail - 1] = mainX[*tail - 2] - (mainX[*tail - 2] - (*tail - 3 >= 0 ? mainX[*tail - 3] : *headX));

		map[mainY[*tail-1]][mainX[*tail-1]] = OBJECT_TAIL;
	}
	//벽에 닿았거나 자신의 꼬리에 닿았을 때
	else if (map[*headY][*headX] == OBJECT_WALL || map[*headY][*headX] == OBJECT_TAIL) {
		return true;
	}
		
	map[*headY][*headX] = OBJECT_HEAD;
	return false;
}

void RandomApple(object_t(*map)[COLS]) {
	int randX = 0;
	int randY = 0;
	while (1) {
		randX = rand() % ROWS;
		randY = rand() % COLS;
		if (map[randX][randY] == OBJECT_SPACE) break;
	}
	map[randX][randY]=OBJECT_APPLE;
}

void MapPrint(object_t (*map)[COLS],int *appleCnt) {
	printf("현재 먹은 사과의 개수 : %d\n",*appleCnt);
	for (int i = 0; i < ROWS; ++i) {
		for (int j = 0; j < COLS; ++j) {
			switch (map[i][j]) {
			case OBJECT_SPACE:
				printf(". ");
				break;
			case OBJECT_WALL:
				printf("# ");
				break;
			case OBJECT_HEAD:
				printf("O ");
				break;
			case OBJECT_TAIL:
				printf("o ");
				break;
			case OBJECT_APPLE:
				printf("A ");
				break;
			default:
				break;
			}
		}
		printf("\n");
	}
}

int main()
{
	srand(time(NULL));
	object_t map[ROWS][COLS] = { OBJECT_SPACE, };
	int appleCnt = 0, isApple = 0, headX = 0, headY = 0, tail = 0;
	bool gameEnd = false;
	int* mainX = (int*)malloc(sizeof(int)*ROWS*COLS), * mainY = (int*)malloc(sizeof(int)*ROWS*COLS);
	char key;
	keyboard_t dir = 0;

	Init(map,&headY,&headX,&tail,mainY,mainX);

	while (1) {
		if (isApple == 0) {
			RandomApple(map);
			isApple = 1;
		}

		if (_kbhit()) {
			key = _getch();
			if (key == -32) {
				key=_getch();
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
			default:
				break;
			}
		}
		
		gameEnd=Move(map,dir,&headY,&headX,&tail,mainY,mainX,&appleCnt,&isApple);

		MapPrint(map,&appleCnt);
		if (gameEnd) {
			printf("게임 종료\n총 먹은 사과의 개수 : %d", appleCnt);
			break;
		}
		Sleep(200);
		//system("cls");
		gotoxy(0, 0);
	}
	free(mainX);
	free(mainY);

	return 0;
}