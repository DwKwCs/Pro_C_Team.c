#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void set_mine(int** field, int x, int y, int mine_num);
void print_mine(int** field, int x, int y);
void print_cover(char** cover, int x, int y, int* mine_rem);

int main() 
{
	int x, y;  //  2차원 배열 크기 입력. x = 가로, y = 세로.
	int a, b;  //  사용자가 입력한 지뢰의 좌표
	int mine_num;  //  초기 지뢰의 수
	int i, j;
	int mine_rem;  //  남은 지뢰의 수
	printf("지뢰 찾기 영역 크기 입력(가로, 세로) : ");
	scanf("%d %d", &x, &y);

	mine_num = (x * y) / 4;
	mine_rem = mine_num;

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

	//print_mine(field, x, y);  지뢰 배치 확인 용

	//  지뢰 찾기 시작
	print_cover(cover, x, y, mine_num);
	printf("\n지뢰의 좌표는? : ");
	scanf("%d %d", &a, &b);

	/*
	while (mine_rem) {
		if (a >= x || b >= y) {
			printf("잘못된 좌표입력입니다.\n");
		}

		check_mine(field, cover, a, b, mine_rem);
		print_field(field, x, y, mine_rem);
	}
	*/

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

void print_cover(char** cover, int x, int y, int* mine_rem) {
	int i, j;

	printf("\n\n---------- Print Field ----------\n\n");
	printf("남은 지뢰 개수 : %d\n\n", mine_rem);

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