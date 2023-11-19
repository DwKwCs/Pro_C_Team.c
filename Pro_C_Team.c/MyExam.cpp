#include "pch.h"
#include "tipsware.h"
#include <time.h>
#include <stdlib.h>

//추가필요 : 0대신 빈칸 출력, 10대신 지뢰 이미지 출력

typedef struct ApplicationData
{
	int a_num[100][100];
	int b_num[100][100];
	int a_cx;
	int a_cy;
	int a_mine;
	int flagMine;

	int fc;
	int fx;
	int fy;
}AD;

extern void PrintArray(AD* a_data, int px, int py);

//랜덤으로 지뢰를 배치한 배열을 만든다
void random(int** arr, int cx, int cy, int mine_num);

//첫 번째로 선택된 좌표와 그 주변 8개의 수는 지뢰에서 제외하기 위해 임시로 a값을 저장한다
void firstClick(int** arr, int cx, int cy, int fx, int fy, int a);

//지뢰 주변에 힌트 숫자를 배치한다
void Hint(int** arr, int cx, int cy, int mine_num);

//깃발을 그린다
void DrawFlag(int cx, int cy);

//깃발을 지운다
void DeleteFlag(int cx, int cy);

//0주변 8칸 드러내기
void FastZero(int cx, int cy, int px, int py);

//XI
void FastNumber(int cx, int cy, int px, int py);

//마우스 클릭 시 실행되는 함수
void LeftBtnDown(int a_mixed_key, POINT a_pos){

	AD* a_data = (AD*)GetAppData();//프로그램 내부 데이터 주소를 얻는다.

	if (a_data->fc == 0)
	{
		for (int i = 0; i < a_data->a_cy; i++)
		{
			if (a_pos.y > i * 20 + 60 && a_pos.y < i * 20 + 80)
			{
				for (int j = 0; j < a_data->a_cx; j++)
				{
					if (a_pos.x > j * 20 + 60 && a_pos.x < j * 20 + 80)
					{
						int cx = a_data->a_cx;
						int cy = a_data->a_cy;
						a_data->fx = j;
						a_data->fy = i;

						srand(time(NULL));

						int size = cx * cy;
						int mine_num = size / 7;
						a_data->a_mine = mine_num;
						int** arr = NULL;

						arr = (int**)malloc(sizeof(int*) * cy);

						for (int i = 0; i < cy; i++)
							arr[i] = (int*)malloc(sizeof(int*) * cx);

						for (int i = 0; i < cy; i++)
						{
							for (int j = 0; j < cx; j++)
							{
								arr[i][j] = 0;
							}
						}

						firstClick(arr, cx, cy, a_data->fx, a_data->fy, 10);
						random(arr, cx, cy, mine_num);
						firstClick(arr, cx, cy, a_data->fx, a_data->fy, 0);
						Hint(arr, cx, cy, mine_num);

						for (int i = 0; i < cy; i++)
							for (int j = 0; j < cx; j++)
							{
								a_data->a_num[i][j] = arr[i][j];
								a_data->b_num[i][j] = arr[i][j];
							}

						/*
						for (int i = 0; i < cy; i++)
						{
							for (int j = 0; j < cx; j++)
							{
								printf(j * 20 + 60, i * 20 + 60, "%2d ", arr[i][j]);
								printf(j * 20 + 60, i * 20 + 60, "%2d ", a_data->a_num[i][j]);
								printf(j * 20 + 60, i * 20 + 60, "%2d ", a_data->b_num[i][j]);
							}
						}
						*/

						for (int i = 0; i < cy; i++)
						{
							free(arr[i]);
						}
						free(arr);
						(a_data->fc) = 1;

					}
				}
			}
		}	
	}

	//ctrl+클릭을 했을 때 플래그 세우기&없애기
	if (a_mixed_key & MK_CONTROL)
	{
		if (a_data->fc == 0)
		{
			return;
		}

		for (int i = 0; i < a_data->a_cy; i++)
		{
			if (a_pos.y > i * 20 + 60 && a_pos.y < i * 20 + 80)
			{
				for (int j = 0; j < a_data->a_cx; j++)
				{
					if (a_pos.x > j * 20 + 60 && a_pos.x < j * 20 + 80)
					{
						if(a_data->a_num[i][j] < 10)
						{
							DrawFlag(j, i);
							a_data->a_num[i][j] += 50; //플래그가 세워진 곳은 50을 더해서 플래그가 세워졌음을 표시
						}
						else if (a_data->a_num[i][j] == 10)
						{
							DrawFlag(j, i);
							a_data->a_num[i][j] += 50; //플래그가 세워진 곳은 50을 더해서 플래그가 세워졌음을 표시
							(a_data->flagMine)++;
						}
						else if (a_data->a_num[i][j] >= 50 && a_data->a_num[i][j] <= 60)
						{
							DeleteFlag(j, i);
							a_data->a_num[i][j] -= 50; //플래그 삭제 후에는 50을 빼준다
						}
					}
					if (a_data->flagMine == a_data->a_mine)
					{
						printf(10, 10, "게임 승리!");
					}
				}
			}
		}
	}

	//일반 클릭을 했을 때 숫자 드러내기
	else
	{
		for (int i = 0; i < a_data->a_cy; i++)
		{
			if (a_pos.y > i * 20 + 60 && a_pos.y < i * 20 + 80)
			{
				for (int j = 0; j < a_data->a_cx; j++)
				{
					if (a_pos.x > j * 20 + 60 && a_pos.x < j * 20 + 80)
					{
						if (a_data->a_num[i][j] >= 50 && a_data->a_num[i][j] < 100) //이미 클릭한 0 또는 깃발꽂힌칸 제외
							return;
						else if (a_data->a_num[i][j] > 100) //클릭한칸 또 눌러서 힌트얻기
						{
							FastNumber(a_data->a_cx, a_data->a_cy, j, i);
						}
						else if (a_data->a_num[i][j] == 10) //종료조건
						{
							printf(10, 10, "게임 오버");
							for (i = 0; i < a_data->a_cy; i++)
							{
								for (j = 0; j < a_data->a_cx; j++)
								{
									PrintArray(a_data, j, i);
								}
							}
						}
						else
						{
							PrintArray(a_data, j, i);
							a_data->a_num[i][j] += 100; //숫자가 드러난 곳에는 100을 더해서 숫자가 드러났음을 표시
							FastZero(a_data->a_cx, a_data->a_cy, j, i);
						}

					}
				}
			}
		}
	}
	ShowDisplay();
}

//이 프로그램은 마우스 좌클릭을 사용한다.
MOUSE_MESSAGE(LeftBtnDown, NULL,NULL)

int main()
{
	CreateAppData(sizeof(AD));
	AD* a_data = (AD*)GetAppData();//프로그램 내부 데이터 주소를 얻는다.
	a_data->fc = 0;

	int cx = 15, cy = 10, i, j;
	
	a_data->a_cx = cx;
	a_data->a_cy = cy;
	
	for (int i = 0; i < cy; i++)
		for (int j = 0; j < cx; j++)
			Rectangle(j * 20 + 60, i * 20 + 60, j * 20 + 80, i * 20 + 80, RGB(0, 0, 0), RGB(158, 158, 158));

	for (int i = 0; i < 100 ;i++)
		for (int j = 0; j < 100; j++)
		{
			a_data->a_num[i][j] = -1;
			a_data->b_num[i][j] = -1;
		}

	ShowDisplay(); // 정보를 윈도우에 출력한다.

	return 0;
}

void PrintArray(AD* a_data, int px, int py)
{
	if (a_data->b_num[py][px] >= 0)
	{
		printf(px * 20 + 60, py * 20 + 60, "%2d", a_data->b_num[py][px]);
	}
}

//랜덤으로 지뢰를 배치한 배열을 만든다
void random(int** arr, int cx, int cy, int mine_num)
{
	int n1, n2;
	int mine = mine_num;

	while (mine)
	{
		n1 = rand() % cx; 
		n2 = rand() % cy;

		if (n1 >= 0 && n1 < cx && n2 >= 0 && n2 < cy)
		{
			if (arr[n2][n1] < 10) //지뢰 좌표일 경우
			{
				arr[n2][n1] = 10;
				mine--;
			}
		}
	}
}

//첫 번째로 선택된 좌표와 그 주변 8개의 수는 지뢰에서 제외하기 위해 임시로 a값을 저장한다
void firstClick(int** arr, int cx, int cy, int fx, int fy, int a)
{
	int i = fy;
	int j = fx;
	//모서리에 대한 예외 처리를 마친 상태입니다

	arr[i][j] = a; //첫 클릭한 곳을 a로

	if (i - 1 >= 0 && j - 1 >= 0)
	{
		arr[i - 1][j - 1] = a;
	}

	if (i - 1 >= 0)
	{
		arr[i - 1][j] = a;
	}

	if (i - 1 >= 0 && j + 1 < cy)
	{
		arr[i - 1][j + 1] = a;
	}

	if (j - 1 >= 0)
	{
		arr[i][j - 1] = a;
	}

	if (j + 1 < cy)
	{
		arr[i][j + 1] = a;
	}

	if (i + 1 < cx && j - 1 >= 0)
	{
		arr[i + 1][j - 1] = a;
	}

	if (i + 1 < cx)
	{
		arr[i + 1][j] = a;
	}

	if (i + 1 < cx && j + 1 < cy)
	{
		arr[i + 1][j + 1] = a;
	}

}

//지뢰 주변에 힌트 숫자를 배치한다
void Hint(int** arr, int cx, int cy, int mine_num)
{
	int i = 0, j = 0;
	int mine = mine_num;

	//모서리에 대한 예외 처리를 마친 상태입니다
	while (i < cy)
	{
		j = 0;
		while (j < cx)
		{
			if (mine == 0)
			{
				return;
			}

			if (arr[i][j] == 10)
			{
				if (i - 1 >= 0 && j - 1 >= 0 && arr[i - 1][j - 1] != 10)
				{
					arr[i - 1][j - 1] += 1;
				}

				if (i - 1 >= 0 && arr[i - 1][j] != 10)
				{
					arr[i - 1][j] += 1;
				}

				if (i - 1 >= 0 && j + 1 < cx && arr[i - 1][j + 1] != 10)
				{
					arr[i - 1][j + 1] += 1;
				}

				if (j - 1 >= 0 && arr[i][j - 1] != 10)
				{
					arr[i][j - 1] += 1;
				}

				if (j + 1 < cx && arr[i][j + 1] != 10)
				{
					arr[i][j + 1] += 1;
				}

				if (i + 1 < cy && j - 1 >= 0 && arr[i + 1][j - 1] != 10)
				{
					arr[i + 1][j - 1] += 1;
				}

				if (i + 1 < cy && arr[i + 1][j] != 10)
				{
					arr[i + 1][j] += 1;
				}

				if (i + 1 < cy && j + 1 < cx && arr[i + 1][j + 1] != 10)
				{
					arr[i + 1][j + 1] += 1;
				}

				j++;
				mine--;
			}
			else
				j++;
		}
		i++;
	}
}

//깃발을 그린다
void DrawFlag(int cx, int cy)
{
	SelectPenObject(RGB(255, 0, 0), 2);
	Line(20 * cx + 63, 20 * cy + 63, 20 * cx + 63, 20 * cy + 77);
	Line(20 * cx + 63, 20 * cy + 77, 20 * cx + 77, 20 * cy + 70);
	Line(20 * cx + 77, 20 * cy + 70, 20 * cx + 63, 20 * cy + 63);
}

//깃발을 지운다
void DeleteFlag(int cx, int cy)
{
	SelectPenObject(RGB(158, 158, 158), 2);
	Line(20 * cx + 63, 20 * cy + 63, 20 * cx + 63, 20 * cy + 77);
	Line(20 * cx + 63, 20 * cy + 77, 20 * cx + 77, 20 * cy + 70);
	Line(20 * cx + 77, 20 * cy + 70, 20 * cx + 63, 20 * cy + 63);
}

//0주변 8칸 드러내기
void FastZero(int cx, int cy, int px, int py)
{
	AD* a_data = (AD*)GetAppData();//프로그램 내부 데이터 주소를 얻는다.
	int* ptr;

	if (a_data->a_num[py][px] == 100)
	{
		//py-1 px-1
		if (py - 1 >= 0 && px - 1 >= 0 && a_data->a_num[py - 1][px - 1] >= 0 && a_data->a_num[py - 1][px - 1] <= 50)
		{
			if (a_data->a_num[py - 1][px - 1] == 50)
			{
				DeleteFlag(px - 1, py - 1);
			}

			ptr = &a_data->a_num[py - 1][px - 1];
			*ptr += 100;
			PrintArray(a_data, px - 1, py - 1);
			FastZero(cx, cy, px - 1, py - 1);
		}
		//py-1 px
		if (py - 1 >= 0 && a_data->a_num[py - 1][px] >= 0 && a_data->a_num[py - 1][px] <= 50)
		{
			if (a_data->a_num[py - 1][px] == 50)
			{
				DeleteFlag(px, py - 1);
			}

			ptr = &a_data->a_num[py - 1][px];
			*ptr += 100;
			PrintArray(a_data, px, py - 1);
			FastZero(cx, cy, px, py - 1);
		}
		//py-1 px+1
		if (py - 1 >= 0 && px + 1 < cx && a_data->a_num[py - 1][px + 1] >= 0 && a_data->a_num[py - 1][px + 1] <= 50)
		{
			if (a_data->a_num[py - 1][px + 1] == 50)
			{
				DeleteFlag(px + 1, py - 1);
			}

			ptr = &a_data->a_num[py - 1][px + 1];
			*ptr += 100;
			PrintArray(a_data, px + 1, py - 1);
			FastZero(cx, cy, px + 1, py - 1);
		}
		//px-1 py
		if (px - 1 >= 0 && a_data->a_num[py][px - 1] >= 0 && a_data->a_num[py][px - 1] <= 50)
		{
			if (a_data->a_num[py][px - 1] == 50)
			{
				DeleteFlag(px - 1, py);
			}

			ptr = &a_data->a_num[py][px - 1];
			*ptr += 100;
			PrintArray(a_data, px - 1, py);
			FastZero(cx, cy, px - 1, py);
		}
		//px+1 py
		if (px + 1 < cx && a_data->a_num[py][px + 1] >= 0 && a_data->a_num[py][px + 1] <= 50)
		{
			if (a_data->a_num[py][px + 1] == 50)
			{
				DeleteFlag(px + 1, py);
			}

			ptr = &a_data->a_num[py][px + 1];
			*ptr += 100;
			PrintArray(a_data, px + 1, py);
			FastZero(cx, cy, px + 1, py);
		}
		//py+1 px-1
		if (py + 1 < cy && px - 1 >= 0 && a_data->a_num[py + 1][px - 1] >= 0 && a_data->a_num[py + 1][px - 1] <= 50)
		{
			if (a_data->a_num[py + 1][px - 1] == 50)
			{
				DeleteFlag(px - 1, py + 1);
			}

			ptr = &a_data->a_num[py + 1][px - 1];
			*ptr += 100;
			PrintArray(a_data, px - 1, py + 1);
			FastZero(cx, cy, px - 1, py + 1);
		}
		//py+1 px
		if (py + 1 < cy && a_data->a_num[py + 1][px] >= 0 && a_data->a_num[py + 1][px] <= 50)
		{
			if (a_data->a_num[py + 1][px] == 50)
			{
				DeleteFlag(px, py + 1);
			}

			ptr = &a_data->a_num[py + 1][px];
			*ptr += 100;
			PrintArray(a_data, px, py + 1);
			FastZero(cx, cy, px, py + 1);
		}
		//py+1 px+1
		if (py + 1 < cy && px + 1 < cx && a_data->a_num[py + 1][px + 1] >= 0 && a_data->a_num[py + 1][px + 1] <= 50)
		{
			if (a_data->a_num[py + 1][px + 1] == 50)
			{
				DeleteFlag(px + 1, py + 1);
			}

			ptr = &a_data->a_num[py + 1][px + 1];
			*ptr += 100;
			PrintArray(a_data, px + 1, py + 1);
			FastZero(cx, cy, px + 1, py + 1);
		}
	}
	else
		return;
}

//XI
void FastNumber(int cx, int cy, int px, int py)
{
	int count = 0;
	int* ptr;
	AD* a_data = (AD*)GetAppData();//프로그램 내부 데이터 주소를 얻는다.

	if (&a_data->b_num[py][px] == 0)
	{
		return;
	}

	if (a_data->a_num[py + 1][px] == 60)
		count += 1;
	if (a_data->a_num[py][px + 1] == 60)
		count += 1;
	if (a_data->a_num[py - 1][px] == 60)
		count += 1;
	if (a_data->a_num[py][px - 1] == 60)
		count += 1;
	if (a_data->a_num[py + 1][px + 1] == 60)
		count += 1;
	if (a_data->a_num[py + 1][px - 1] == 60)
		count += 1;
	if (a_data->a_num[py - 1][px + 1] == 60)
		count += 1;
	if (a_data->a_num[py - 1][px - 1] == 60)
		count += 1;

	if (count == a_data->b_num[py][px] && count != 0)
	{
		if (0 <= a_data->a_num[py + 1][px] && a_data->a_num[py + 1][px] < 10)
		{
			PrintArray(a_data, px, py + 1);
			ptr = &a_data->a_num[py + 1][px];
			*ptr += 100;
			FastZero(a_data->a_cx, a_data->a_cy, px, py + 1);
		}

		if (0 <= a_data->a_num[py - 1][px] && a_data->a_num[py - 1][px] < 10)
		{
			PrintArray(a_data, px, py - 1);
			ptr = &a_data->a_num[py - 1][px];
			*ptr += 100;
			FastZero(a_data->a_cx, a_data->a_cy, px, py - 1);
		}

		if (0 <= a_data->a_num[py][px + 1] && a_data->a_num[py][px + 1] < 10)
		{
			PrintArray(a_data, px + 1, py);
			ptr = &a_data->a_num[py][px + 1];
			*ptr += 100;
			FastZero(a_data->a_cx, a_data->a_cy, px + 1, py);
		}

		if (0 <= a_data->a_num[py][px - 1] && a_data->a_num[py][px - 1] < 10)
		{
			PrintArray(a_data, px - 1, py);
			ptr = &a_data->a_num[py][px - 1];
			*ptr += 100;
			FastZero(a_data->a_cx, a_data->a_cy, px - 1, py);
		}

		if (0 <= a_data->a_num[py + 1][px + 1] && a_data->a_num[py + 1][px + 1] < 10)
		{
			PrintArray(a_data, px + 1, py + 1);
			ptr = &a_data->a_num[py + 1][px + 1];
			*ptr += 100;
			FastZero(a_data->a_cx, a_data->a_cy, px + 1, py + 1);
		}

		if (0 <= a_data->a_num[py + 1][px - 1] && a_data->a_num[py + 1][px - 1] < 10)
		{
			PrintArray(a_data, px - 1, py + 1);
			ptr = &a_data->a_num[py + 1][px - 1];
			*ptr += 100;
			FastZero(a_data->a_cx, a_data->a_cy, px - 1, py + 1);
		}

		if (0 <= a_data->a_num[py - 1][px + 1] && a_data->a_num[py - 1][px + 1] < 10)
		{
			PrintArray(a_data, px + 1, py - 1);
			ptr = &a_data->a_num[py - 1][px + 1];
			*ptr += 100;
			FastZero(a_data->a_cx, a_data->a_cy, px + 1, py - 1);
		}

		if (0 <= a_data->a_num[py - 1][px - 1] && a_data->a_num[py - 1][px - 1] < 10)
		{
			PrintArray(a_data, px - 1, py - 1);
			ptr = &a_data->a_num[py - 1][px - 1];
			*ptr += 100;
			FastZero(a_data->a_cx, a_data->a_cy, px - 1, py - 1);
		}

	}
	else
		count = 0; // 무의미 좌표 배제
}