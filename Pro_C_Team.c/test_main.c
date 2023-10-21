#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void set_mine(int** field, int x, int y, int mine_num);
void print_mine(int** field, int x, int y);
void print_cover(char** cover, int x, int y, int mine_num);
int check_mine(int** field, int a, int b, int x, int y);

int main() 
{
	int x, y;  //  2차원 배열 크기 입력. x = 가로, y = 세로.
	int a, b;  //  사용자가 입력한 지뢰의 좌표
	char f;  //  사용자가 깃발 생성. S->깃발x, F->깃발o.
	int mine_num;  //  초기 지뢰의 수
	int i, j;
	int g = 1;  //  지뢰찾기 종료 시 0
	printf("지뢰 찾기 영역 크기 입력(가로, 세로) : ");
	scanf("%d %d", &x, &y);

	mine_num = (x * y) * 4 / 25;

	//  2차원 배열 동적 할당. 지뢰를 배치할 field 생성.
	int** field = (int**)malloc(sizeof(int*) * y);
	for (i = 0; i < y; i++)
		field[i] = (int*)malloc(sizeof(int) * x);
	//  2차원 배열 0으로 초기화
	for (i = 0; i < y; i++) {
		for (j = 0; j < x; j++)
			field[i][j] = 0;
	}

	//  2차원 배열 동적 할당. 화면에 띄울 cover 생성.
	char** cover = (char**)malloc(sizeof(char*) * y);
	for (i = 0; i < y; i++)
		cover[i] = (char**)malloc(sizeof(char*) * x);
	//  2차원 배열 'O'으로 초기화
	for (i = 0; i < y; i++) {
		for (j = 0; j < x; j++)
			cover[i][j] = 'O';
	}

	//  지뢰 배치
	set_mine(field, x, y, mine_num);
	if (field == NULL) {
		printf("지뢰 배치 실패!\n");
		return 0;
	}
	else
		printf("\n지뢰 배치 성공! 지뢰 찾기 시작!\n");

	print_mine(field, x, y);  //  지뢰 배치 확인 용

	//  지뢰 찾기 시작
	print_cover(cover, x, y, mine_num);

	while (g) {
		while (1) {
			printf("\n깃발을 세우려면 앞에 F를 입력, 아니면 S를 입력. ex)좌표 : S 5 7\n지뢰의 좌표는? : ");
			scanf("%c %d %d", &f, &a, &b);
			if (a >= x || b >= y)
				printf("잘못된 좌표입력입니다!\n");
			else
				break;
		}

		if (f == 'S') {
			int c = check_mine(field, a, b, x, y);
			if (c != 0) {
				cover[b][a] = c + 48;
			}
			else if (c == 0) {
				cover[b][a] = '*';
				print_cover(cover, x, y, mine_num);
				printf("\n(%d, %d)좌표의 지뢰를 밟았습니다!\n", a, b);
				g = 0;
			}
		}
		else if (f == 'F') cover[b][a] = 'F';

		if(g == 1)
			print_cover(cover, x, y, mine_num);
		else if (g == 0) {
			for (i = 0; i < y; i++) {
				for (j = 0; j < x; j++) {
					if(field[i][j] == 1)
						cover[i][j] = '*';
				}
			}
			print_cover(cover, x, y, mine_num);
		}
	}

	//  2차원 배열 메모리 해제
	for (i = 0; i < y; i++)
		free(field[i]);
	free(field);

	return 0;
}

void set_mine(int** field, int x, int y, int mine_num) {
	srand((unsigned int)time(NULL));

	while(mine_num){
		int m_x = rand() % x;
		int m_y = rand() % y;
		if (field[m_y][m_x] == 1) {
			while (field[m_y][m_x]) {
				m_x = rand() % x;
				m_y = rand() % y;
			}
			field[m_y][m_x] = 1;
		}
		else
			field[m_y][m_x] = 1;  //  지뢰 = 1
		mine_num--;
	}
}

void print_mine(int** field, int x, int y) {
	int i, j;
	printf("\n\n---------- Print Mine ----------\n\n");

	printf("  ");
	for (i = 0; i < x; i++) {
		printf(" %d", i);
	}
	printf("\n\n");
	for (j = 0; j < y; j++) {
		printf("%d ", j);
		for (i = 0; i < x; i++) {
			printf(" %d", field[j][i]);
		}
		printf("\n");
	}
}

void print_cover(char** cover, int x, int y, int mine_num) {
	int i, j;

	printf("\n\n---------- Print Field ----------\n\n");
	printf("지뢰 개수 : %d\n\n", mine_num);

	printf("  ");
	for (i = 0; i < x; i++) {
		printf(" %d", i);
	}
	printf("\n\n");
	for (j = 0; j < y; j++) {
		printf("%d ", j);
		for (i = 0; i < x; i++) {
			printf(" %c", cover[j][i]);
		}
		printf("\n");
	}
}

int check_mine(int** field, int a, int b, int x, int y) {
	int m = 0;
	if (field[b][a] == 0) {
		if (a == 0) {
			if (b == 0) {
				if (field[b][a+1] == 1)
					m++;
				if (field[b+1][a+1] == 1)
					m++;
				if (field[b+1][a] == 1)
					m++;
			}
			else if (b == y-1) {
				if (field[b][a + 1] == 1)
					m++;
				if (field[b -1][a + 1] == 1)
					m++;
				if (field[b - 1][a] == 1)
					m++;
			}
			else {
				for (int i = 0; i < 2; i++) {
					if (field[b-1][a + i] == 1)
						m++;
				}
				if (field[b][a + 1] == 1)
					m++;
				for (int i = 0; i < 2; i++) {
					if (field[b + 1][a + i] == 1)
						m++;
				}
			}
		}
		else if (b == 0) {
			if (a == 0) {
				if (field[b][a + 1] == 1)
					m++;
				if (field[b + 1][a + 1] == 1)
					m++;
				if (field[b + 1][a] == 1)
					m++;
			}
			else if (a == x - 1) {
				if (field[b][a - 1] == 1)
					m++;
				if (field[b + 1][a - 1] == 1)
					m++;
				if (field[b + 1][a] == 1)
					m++;
			}
			else {
				for (int i = 0; i < 3; i++) {
					if (field[b + 1][(a-1) + i] == 1)
						m++;
				}
				if (field[b][a + 1] == 1)
					m++;
				if (field[b][a - 1] == 1)
					m++;
			}
		}
		else if (a == x - 1) {
			if (b == 0) {
				if (field[b][a - 1] == 1)
					m++;
				if (field[b + 1][a - 1] == 1)
					m++;
				if (field[b + 1][a] == 1)
					m++;
			}
			else if (b == y - 1) {
				if (field[b][a - 1] == 1)
					m++;
				if (field[b - 1][a - 1] == 1)
					m++;
				if (field[b - 1][a] == 1)
					m++;
			}
			else {
				for (int i = 0; i < 2; i++) {
					if (field[b - 1][(a-1) + i] == 1)
						m++;
				}
				if (field[b][a - 1] == 1)
					m++;
				for (int i = 0; i < 2; i++) {
					if (field[b + 1][(a-1) + i] == 1)
						m++;
				}
			}
		}
		else if (b == y - 1) {
			if (a == 0) {
				if (field[b][a + 1] == 1)
					m++;
				if (field[b - 1][a + 1] == 1)
					m++;
				if (field[b - 1][a] == 1)
					m++;
			}
			else if (a == x - 1) {
				if (field[b][a - 1] == 1)
					m++;
				if (field[b - 1][a - 1] == 1)
					m++;
				if (field[b - 1][a] == 1)
					m++;
			}
			else {
				for (int i = 0; i < 3; i++) {
					if (field[b - 1][(a - 1) + i] == 1)
						m++;
				}
				if (field[b][a + 1] == 1)
					m++;
				if (field[b][a - 1] == 1)
					m++;
			}
		}
		else {
			for (int i = 0; i < 3; i++) {
				if (field[b - 1][(a - 1) + i] == 1)
					m++;
			}
			if (field[b][a - 1] == 1)
				m++;
			if (field[b][a + 1] == 1)
				m++;
			for (int i = 0; i < 3; i++) {
				if (field[b + 1][(a - 1) + i] == 1)
					m++;
			}
		}
	}
	else if(field[b][a] == 1) return m;

	return m;
}