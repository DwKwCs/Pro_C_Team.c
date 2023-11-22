#include "pch.h"   
#include "tipsware.h"
#include <time.h>
#include <stdlib.h>

//  필드를 생성하는 시작 지점
int Field_Start_x = 600;
int Field_Start_y = 390;

//  생성할 필드 크기
int Field_Size_x = 0;
int Field_Size_y = 0;

int cx, cy;

// 프로그램에서 사용할 내부 데이터
//  지뢰찾기 정보 구조체
typedef struct ApplicationData
{
	int a_num[29][38];  //  지뢰 배열
	int b_num[29][38];  //  출력용 배열
	int c_num[29][38];  //  a_num 복사
	int a_mine;
	int flagMine;

	int fc;  //  초기 필드 생성 시 1. 생성된 적 없으면 0.
	int fx;  //  처음 클릭한 x좌표
	int fy;  //  처음 클릭한 y좌표

	int WinCount;  //  남은 칸수

	//  타이머 시간 정보
	int sec;
	int min;
	int hour;

	//  성공 시간 저장
	int p_time[10][3];
}AD;

//  처음 실행 시 초기 화면 생성하는 함수
void Init(AD* a_data);

//  타이머 함수. 1초마다 실행
TIMER StopWatchProc(NOT_USE_TIMER_DATA);

//  버튼 컨트롤, 필드 생성 함수
void SetField(INT32 a_ctrl_id, INT32 a_notify_code, void* ap_ctrl);

//  0 : 빈칸, 10 : 지뢰
void PrintArray(AD* a_data, int px, int py);

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

//마우스 클릭 시 실행되는 함수
void LeftBtnDown(int a_mixed_key, POINT a_pos);

//이 프로그램은 마우스 좌클릭을 사용한다. & 버튼 컨트롤
MOUSE_CMD_MESSAGE(LeftBtnDown, NULL, NULL, SetField)

int main()
{
	//  내부 데이터 생성
	CreateAppData(sizeof(AD));
	AD* a_data = (AD*)GetAppData();//프로그램 내부 데이터 주소를 얻는다.
	a_data->sec = 0;
	a_data->min = 0;
	a_data->hour = 0;
	for (int b = 0; b < 10; b++) {
		for (int a = 0; a < 3; a++)
			a_data->p_time[b][a] = 0;
	}

	Init(a_data);

	ShowDisplay();

	return 0;
}

void Init(AD* a_data)
{
	//  창 크기 조절(그냥 함)
	ChangeWorkSize(1000, 700);

	//  왼쪽 사용자 창
	Rectangle(10, 10, 200, 690, RGB(0, 0, 0));
	//  오른쪽 지뢰찾기 필드 출력 창. 가로 : 755, 세로 : 580. 중앙값 : 377.5, 290.
	Rectangle(215, 10, 990, 690, RGB(0, 0, 0));
	Rectangle(225, 100, 980, 680, RGB(0, 0, 0));

	//  오른쪽 타이머 생성
	Rectangle(800, 20, 980, 90);
	printf(805, 30, RGB(0, 0, 0), "플레이 타임 ");
	SelectBrushObject(RGB(222, 239, 255));
	RECT timer = { 810, 45, 970, 85 };
	Rectangle(&timer);
	printf(857, 57, RGB(0, 0, 0), "00:00:00");

	//  ex) 8, 10 => '필드 생성' 클릭으로 필드 생성
	//  최대 크기 : 38, 29
	printf(15, 15, "지뢰찾기 사용자 생성");
	CreateEdit(15, 35, 40, 20, 0, 0);
	CreateButton("필드 생성", 60, 35, 50, 20, 1);

	// 난이도별 필드 생성(난이도는 필드 크기에 비례)
	printf(15, 65, "지뢰찾기 난이도 생성");
	CreateButton("난이도 하", 15, 85, 50, 20, 2);
	CreateButton("난이도 중", 15, 110, 50, 20, 3);
	CreateButton("난이도 상", 15, 135, 50, 20, 4);

	//  기록
	printf(15, 160, "플레이 타임 기록");
	Rectangle(15, 180, 195, 660);

	//  지뢰찾기 시작, 지우기
	CreateButton("지뢰찾기 지우기", 225, 20, 100, 70, 6);

	//  칸 선택 : 좌클릭 , 깃발 세우기 : ctrl + 좌클릭.
	SelectBrushObject(RGB(255, 255, 255));
	Rectangle(340, 20, 600, 90);
	printf(360, 30, RGB(0, 0, 0), "칸 선택 : 좌클릭");
	printf(360, 60, RGB(0, 0, 0), "깃발 세우기 : ctrl + 좌클릭");

	//  지뢰찾기 결과 출력 창, "성공" or "실패"
	Rectangle(620, 20, 790, 90, RGB(0, 0, 0));
	printf(630, 50, "지뢰찾기 ");

	//  내부 데이터 초기화
	for (int c = 0; c < 29; c++) {
		for (int l = 0; l < 38; l++)
			a_data->a_num[c][l] = 0;
	}
	for (int c = 0; c < 29; c++) {
		for (int l = 0; l < 38; l++)
			a_data->b_num[c][l] = 0;
	}
	for (int c = 0; c < 29; c++) {
		for (int l = 0; l < 38; l++)
			a_data->c_num[c][l] = 0;
	}
	a_data->a_mine = 0;
	a_data->flagMine = 0;
	a_data->fc = 0;
	a_data->fx = 0;
	a_data->fy = 0;
	a_data->WinCount = 0;
	a_data->sec = 0;
	a_data->min = 0;
	a_data->hour = 0;

	//  플레이 시간 출력
	int t = 0;
	while (a_data->p_time[t][0] != 0 || a_data->p_time[t][1] != 0 || a_data->p_time[t][2] != 0) {
		if (a_data->p_time[t][0] < 10 && a_data->p_time[t][1] < 10 && a_data->p_time[t][2] < 10) {
			printf(20, 190 + t * 20, RGB(0, 0, 0), "%d판 - 0%d:0%d:0%d", t + 1, a_data->p_time[t][0], a_data->p_time[t][1], a_data->p_time[t][2]);
		}
		else if (a_data->p_time[t][0] >= 10 && a_data->p_time[t][1] < 10 && a_data->p_time[t][2] < 10) {
			printf(20, 190 + t * 20, RGB(0, 0, 0), "%d판 - %d:0%d:0%d", t + 1, a_data->p_time[t][0], a_data->p_time[t][1], a_data->p_time[t][2]);
		}
		else if (a_data->p_time[t][0] < 10 && a_data->p_time[t][1] >= 10 && a_data->p_time[t][2] < 10) {
			printf(20, 190 + t * 20, RGB(0, 0, 0), "%d판 - 0%d:%d:0%d", t + 1, a_data->p_time[t][0], a_data->p_time[t][1], a_data->p_time[t][2]);
		}
		else if (a_data->p_time[t][0] < 10 && a_data->p_time[t][1] < 10 && a_data->p_time[t][2] >= 10) {
			printf(20, 190 + t * 20, RGB(0, 0, 0), "%d판 - 0%d:0%d:%d", t + 1, a_data->p_time[t][0], a_data->p_time[t][1], a_data->p_time[t][2]);
		}
		else if (a_data->p_time[t][0] >= 10 && a_data->p_time[t][1] >= 10 && a_data->p_time[t][2] < 10) {
			printf(20, 190 + t * 20, RGB(0, 0, 0), "%d판 - %d:%d:0%d", t + 1, a_data->p_time[t][0], a_data->p_time[t][1], a_data->p_time[t][2]);
		}
		else if (a_data->p_time[t][0] >= 10 && a_data->p_time[t][1] < 10 && a_data->p_time[t][2] >= 10) {
			printf(20, 190 + t * 20, RGB(0, 0, 0), "%d판 - %d:0%d:%d", t + 1, a_data->p_time[t][0], a_data->p_time[t][1], a_data->p_time[t][2]);
		}
		else if (a_data->p_time[t][0] < 10 && a_data->p_time[t][1] >= 10 && a_data->p_time[t][2] >= 10) {
			printf(20, 190 + t * 20, RGB(0, 0, 0), "%d판 - 0%d:%d:%d", t + 1, a_data->p_time[t][0], a_data->p_time[t][1], a_data->p_time[t][2]);
		}
		else if (a_data->p_time[t][0] >= 10 && a_data->p_time[t][1] >= 10 && a_data->p_time[t][2] >= 10) {
			printf(20, 190 + t * 20, RGB(0, 0, 0), "%d판 - %d:%d:%d", t + 1, a_data->p_time[t][0], a_data->p_time[t][1], a_data->p_time[t][2]);
		}
		t++;
	}
}

// 타이머가 0.1초마다 호출할 함수, 이 함수의 이름은 변경 가능하지만 함수의 형식은
// 정해져 있기 때문에 변경하지 말고 그대로 사용해야 함.
TIMER StopWatchProc(NOT_USE_TIMER_DATA)
{
	AD* a_data = (AD*)GetAppData();
	//DrawCount(a_data->time);

	Rectangle(810, 45, 970, 85, RGB(0, 0, 0), RGB(224, 239, 255));
	if (a_data->sec < 10 && a_data->min < 10 && a_data->hour < 10) {
		printf(857, 57, RGB(0, 0, 0), "0%d:0%d:0%d", a_data->hour, a_data->min, a_data->sec);
	}
	else if (a_data->sec >= 10 && a_data->min < 10 && a_data->hour < 10) {
		printf(857, 57, RGB(0, 0, 0), "0%d:0%d:%d", a_data->hour, a_data->min, a_data->sec);
	}
	else if (a_data->sec < 10 && a_data->min >= 10 && a_data->hour < 10) {
		printf(857, 57, RGB(0, 0, 0), "0%d:%d:0%d", a_data->hour, a_data->min, a_data->sec);
	}
	else if (a_data->sec < 10 && a_data->min < 10 && a_data->hour >= 10) {
		printf(857, 57, RGB(0, 0, 0), "%d:0%d:0%d", a_data->hour, a_data->min, a_data->sec);
	}
	else if (a_data->sec >= 10 && a_data->min >= 10 && a_data->hour < 10) {
		printf(857, 57, RGB(0, 0, 0), "%d:%d:0%d", a_data->hour, a_data->min, a_data->sec);
	}
	else if (a_data->sec >= 10 && a_data->min < 10 && a_data->hour >= 10) {
		printf(857, 57, RGB(0, 0, 0), "%d:0%d:%d", a_data->hour, a_data->min, a_data->sec);
	}
	else if (a_data->sec < 10 && a_data->min >= 10 && a_data->hour >= 10) {
		printf(857, 57, RGB(0, 0, 0), "0%d:%d:%d", a_data->hour, a_data->min, a_data->sec);
	}
	else if (a_data->sec >= 10 && a_data->min >= 10 && a_data->hour >= 10) {
		printf(857, 57, RGB(0, 0, 0), "%d:%d:%d", a_data->hour, a_data->min, a_data->sec);
	}

	ShowDisplay(); // 정보를 윈도우에 출력한다.

	//  1초씩 더함
	a_data->sec++;
	if (a_data->sec == 60) {
		a_data->sec = 0;
		a_data->min++;
		if (a_data->min == 60) {
			a_data->min = 0;
			a_data->hour++;
		}
	}
	SelectPenObject(RGB(0, 0, 0));
}

//  버튼 컨트롤, 필드 생성
void SetField(INT32 a_ctrl_id, INT32 a_notify_code, void* ap_ctrl) {
	int x = Field_Start_x;
	int y = Field_Start_y;

	int i = 0;

	char xy[10] = { NULL, };

	AD* a_data = (AD*)GetAppData();//프로그램 내부 데이터 주소를 얻는다.

	switch (a_ctrl_id) {
	case 1:  //  사용자 필드 생성 버튼 클릭 시
		GetCtrlName(FindControl(0), xy, 10);

		//Field_Size_x
		while (xy[i] != ',') {
			Field_Size_x += xy[i] - 48;
			if (xy[i + 1] != ',')
				Field_Size_x *= 10;
			i++;
		}
		i += 2;
		Field_Start_x -= (Field_Size_x / 2) * 20;
		x = Field_Start_x;

		//Field_Size_y
		while (xy[i] != NULL) {
			Field_Size_y += xy[i] - 48;
			if (xy[i + 1] != NULL)
				Field_Size_y *= 10;
			i++;
		}
		Field_Start_y -= (Field_Size_y / 2) * 20;
		y = Field_Start_y;

		cx = Field_Size_x;
		cy = Field_Size_y;

		//  필드 생성
		if (Field_Size_x != 0 && Field_Size_y != 0) {
			for (int c = 0; c < Field_Size_y; c++) {
				for (int l = 0; l < Field_Size_x; l++) {
					Rectangle(x, y, x + 20, y + 20, RGB(0, 0, 0), RGB(180, 231, 160));
					x += 20;
				}
				x = Field_Start_x;
				y += 20;
			}
		}
		else
			printf(10, 30, "필드 생성 에러!");
		break;
	case 2:  //  난이도 하 필드 생성
		Field_Size_x = 6;
		Field_Size_y = 8;

		cx = Field_Size_x;
		cy = Field_Size_y;

		Field_Start_x -= (Field_Size_x / 2) * 20;
		Field_Start_y -= (Field_Size_y / 2) * 20;

		x = Field_Start_x;
		y = Field_Start_y;

		//  필드 생성
		if (Field_Size_x != 0 && Field_Size_y != 0) {
			for (int c = 0; c < Field_Size_y; c++) {
				for (int l = 0; l < Field_Size_x; l++) {
					Rectangle(x, y, x + 20, y + 20, RGB(0, 0, 0), RGB(180, 231, 160));
					x += 20;
				}
				x = Field_Start_x;
				y += 20;
			}
		}
		break;
	case 3:  //  난이도 중 필드 생성
		Field_Size_x = 12;
		Field_Size_y = 14;

		cx = Field_Size_x;
		cy = Field_Size_y;

		Field_Start_x -= (Field_Size_x / 2) * 20;
		Field_Start_y -= (Field_Size_y / 2) * 20;

		x = Field_Start_x;
		y = Field_Start_y;

		//  필드 생성
		if (Field_Size_x != 0 && Field_Size_y != 0) {
			for (int c = 0; c < Field_Size_y; c++) {
				for (int l = 0; l < Field_Size_x; l++) {
					Rectangle(x, y, x + 20, y + 20, RGB(0, 0, 0), RGB(180, 231, 160));
					x += 20;
				}
				x = Field_Start_x;
				y += 20;
			}
		}
		break;
	case 4:  //  난이도 상 필드 생성
		Field_Size_x = 18;
		Field_Size_y = 20;

		cx = Field_Size_x;
		cy = Field_Size_y;

		Field_Start_x -= (Field_Size_x / 2) * 20;
		Field_Start_y -= (Field_Size_y / 2) * 20;

		x = Field_Start_x;
		y = Field_Start_y;

		//  필드 생성
		if (Field_Size_x != 0 && Field_Size_y != 0) {
			for (int c = 0; c < Field_Size_y; c++) {
				for (int l = 0; l < Field_Size_x; l++) {
					Rectangle(x, y, x + 20, y + 20, RGB(0, 0, 0), RGB(180, 231, 160));
					x += 20;
				}
				x = Field_Start_x;
				y += 20;
			}
		}
		break;
	case 6:  //  필드 지우기
		Field_Start_x = 600;
		Field_Start_y = 390;
		a_data->fc = 0;
		// 시작 시간을 초기화 한다.
		a_data->sec = 0;
		a_data->min = 0;
		a_data->hour = 0;
		KillTimer(1);
		Clear();
		Init(a_data);
		break;
	}

	ShowDisplay();
}

//  필드 출력
void PrintArray(AD* a_data, int px, int py)
{

	switch (a_data->b_num[py][px]) {
	case 0:
		Rectangle(px * 20 + Field_Start_x, py * 20 + Field_Start_y, (px + 1) * 20 + Field_Start_x, (py + 1) * 20 + Field_Start_y, RGB(0, 0, 0), RGB(255, 255, 255));
		printf(px * 20 + Field_Start_x, py * 20 + Field_Start_y, "  ");
		SelectPenObject(RGB(0, 0, 0));
		break;
	case 1:
		Rectangle(px * 20 + Field_Start_x, py * 20 + Field_Start_y, (px + 1) * 20 + Field_Start_x, (py + 1) * 20 + Field_Start_y, RGB(0, 0, 0), RGB(245, 245, 200));
		printf(px * 20 + Field_Start_x, py * 20 + Field_Start_y, RGB(250, 60, 60), "%2d", a_data->b_num[py][px]);
		SelectPenObject(RGB(0, 0, 0));
		break;
	case 2:
		Rectangle(px * 20 + Field_Start_x, py * 20 + Field_Start_y, (px + 1) * 20 + Field_Start_x, (py + 1) * 20 + Field_Start_y, RGB(0, 0, 0), RGB(245, 245, 200));
		printf(px * 20 + Field_Start_x, py * 20 + Field_Start_y, RGB(0, 220, 0), "%2d", a_data->b_num[py][px]);
		SelectPenObject(RGB(0, 0, 0));
		break;
	case 3:
		Rectangle(px * 20 + Field_Start_x, py * 20 + Field_Start_y, (px + 1) * 20 + Field_Start_x, (py + 1) * 20 + Field_Start_y, RGB(0, 0, 0), RGB(245, 245, 200));
		printf(px * 20 + Field_Start_x, py * 20 + Field_Start_y, RGB(30, 60, 255), "%2d", a_data->b_num[py][px]);
		SelectPenObject(RGB(0, 0, 0));
		break;
	case 4:
		Rectangle(px * 20 + Field_Start_x, py * 20 + Field_Start_y, (px + 1) * 20 + Field_Start_x, (py + 1) * 20 + Field_Start_y, RGB(0, 0, 0), RGB(245, 245, 200));
		printf(px * 20 + Field_Start_x, py * 20 + Field_Start_y, RGB(0, 200, 200), "%2d", a_data->b_num[py][px]);
		SelectPenObject(RGB(0, 0, 0));
		break;
	case 5:
		Rectangle(px * 20 + Field_Start_x, py * 20 + Field_Start_y, (px + 1) * 20 + Field_Start_x, (py + 1) * 20 + Field_Start_y, RGB(0, 0, 0), RGB(245, 245, 200));
		printf(px * 20 + Field_Start_x, py * 20 + Field_Start_y, RGB(240, 0, 130), "%2d", a_data->b_num[py][px]);
		SelectPenObject(RGB(0, 0, 0));
		break;
	case 6:
		Rectangle(px * 20 + Field_Start_x, py * 20 + Field_Start_y, (px + 1) * 20 + Field_Start_x, (py + 1) * 20 + Field_Start_y, RGB(0, 0, 0), RGB(245, 245, 200));
		printf(px * 20 + Field_Start_x, py * 20 + Field_Start_y, RGB(230, 220, 50), "%2d", a_data->b_num[py][px]);
		SelectPenObject(RGB(0, 0, 0));
		break;
	case 7:
		Rectangle(px * 20 + Field_Start_x, py * 20 + Field_Start_y, (px + 1) * 20 + Field_Start_x, (py + 1) * 20 + Field_Start_y, RGB(0, 0, 0), RGB(245, 245, 200));
		printf(px * 20 + Field_Start_x, py * 20 + Field_Start_y, RGB(240, 130, 40), "%2d", a_data->b_num[py][px]);
		SelectPenObject(RGB(0, 0, 0));
		break;
	case 8:
		Rectangle(px * 20 + Field_Start_x, py * 20 + Field_Start_y, (px + 1) * 20 + Field_Start_x, (py + 1) * 20 + Field_Start_y, RGB(0, 0, 0), RGB(245, 245, 200));
		printf(px * 20 + Field_Start_x, py * 20 + Field_Start_y, RGB(160, 0, 200), "%2d", a_data->b_num[py][px]);
		SelectPenObject(RGB(0, 0, 0));
		break;
	}
}

//랜덤으로 지뢰를 배치한 배열을 만든다
void random(int** arr, int cx, int cy, int mine_num)
{
	int n1, n2;
	int mine = mine_num;

	srand((unsigned int)time(NULL));

	while (mine)
	{
		n1 = rand() % cx;
		n2 = rand() % cy;

		if (n1 >= 0 && n1 < cx && n2 >= 0 && n2 < cy)
		{
			if (arr[n2][n1] == 10 || arr[n2][n1] == 100) //지뢰 좌표일 경우
			{
				while (arr[n2][n1]) {
					n1 = rand() % cx;
					n2 = rand() % cy;
				}
				arr[n2][n1] = 10;
			}
			else
				arr[n2][n1] = 10;
			mine--;
		}
	}
}

//첫 번째로 선택된 좌표와 그 주변 8개의 수는 지뢰에서 제외하기 위해 임시로 a값을 저장한다
void firstClick(int** arr, int cx, int cy, int fx, int fy, int a)
{
	int i = fy;
	int j = fx;
	//모서리에 대한 예외 처리를 마친 상태입니다

	for (int c = 0; c < 3; c++) {
		for (int l = 0; l < 3; l++)
			arr[(i - 1) + l][(j - 1) + c] = a;
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
			int set[8][2] = {
			{i + 1, j}, {i - 1, j}, {i, j + 1}, {i, j - 1},
			{i + 1, j + 1}, {i + 1, j - 1}, {i - 1, j + 1}, {i - 1, j - 1}
			};

			if (mine == 0)
			{
				return;
			}

			if (arr[i][j] == 10)
			{
				for (int l = 0; l < 8; l++)
				{
					int x = set[l][1];
					int y = set[l][0];

					if (y >= 0 && y < cy && x >= 0 && x < cx && arr[y][x] != 10)
					{
						arr[y][x] += 1;
					}
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
	Line(20 * cx + Field_Start_x + 3, 20 * cy + Field_Start_y + 3, 20 * cx + Field_Start_x + 3, 20 * cy + Field_Start_y + 17, RGB(250, 0, 0), 2);
	Line(20 * cx + Field_Start_x + 3, 20 * cy + Field_Start_y + 17, 20 * cx + Field_Start_x + 17, 20 * cy + Field_Start_y + 10, RGB(255, 0, 0), 2);
	Line(20 * cx + Field_Start_x + 17, 20 * cy + Field_Start_y + 10, 20 * cx + Field_Start_x + 3, 20 * cy + Field_Start_y + 3, RGB(255, 0, 0), 2);
	SelectPenObject(RGB(0, 0, 0));
}

//깃발을 지운다
void DeleteFlag(int cx, int cy)
{
	Line(20 * cx + Field_Start_x + 3, 20 * cy + Field_Start_y + 3, 20 * cx + Field_Start_x + 3, 20 * cy + Field_Start_y + 17, RGB(180, 231, 160), 2);
	Line(20 * cx + Field_Start_x + 3, 20 * cy + Field_Start_y + 17, 20 * cx + Field_Start_x + 17, 20 * cy + Field_Start_y + 10, RGB(180, 231, 160), 2);
	Line(20 * cx + Field_Start_x + 17, 20 * cy + Field_Start_y + 10, 20 * cx + Field_Start_x + 3, 20 * cy + Field_Start_y + 3, RGB(180, 231, 160), 2);
	SelectPenObject(RGB(0, 0, 0));
}

//  초기 클릭 시, 필드 지뢰 생성할 때 쓰는 함수
void FastZero(int cx, int cy, int px, int py)
{
	AD* a_data = (AD*)GetAppData();//프로그램 내부 데이터 주소를 얻는다.
	int* ptr;

	int set[8][2] = {
		{py + 1, px}, {py - 1, px}, {py, px + 1}, {py, px - 1},
		{py + 1, px + 1}, {py + 1, px - 1}, {py - 1, px + 1}, {py - 1, px - 1}
	};

	if (a_data->a_num[py][px] == 100)
	{
		for (int i = 0; i < 8; i++)
		{
			int y = set[i][0];
			int x = set[i][1];

			if (y >= 0 && y < cy && x >= 0 && x < cx && a_data->a_num[y][x] >= 0 && a_data->a_num[y][x] <= 50)
			{
				if (a_data->a_num[y][x] == 50)
				{
					DeleteFlag(x, y);
				}

				ptr = &a_data->a_num[y][x];
				*ptr += 100;
				a_data->WinCount++;
				PrintArray(a_data, x, y);
				FastZero(cx, cy, x, y);
			}
		}
	}
	else
		return;
}

//마우스 클릭 시 실행되는 함수
void LeftBtnDown(int a_mixed_key, POINT a_pos)
{
	AD* a_data = (AD*)GetAppData();//프로그램 내부 데이터 주소를 얻는다.

	//ctrl+클릭을 했을 때 플래그 세우기&없애기
	if (a_mixed_key & MK_CONTROL)
	{
		if (a_data->fc != 0)
		{
			for (int j = 0; j < 8; j++)
			{
				a_data->a_num[0][j] = a_data->c_num[0][j];
			}
		}

		if (a_data->fc == 0)
		{
			return;
		}

		for (int i = 0; i < cy; i++)
		{
			if (a_pos.y > i * 20 + Field_Start_y && a_pos.y < i * 20 + Field_Start_y + 20)
			{
				for (int j = 0; j < cx; j++)
				{
					if (a_pos.x > j * 20 + Field_Start_x && a_pos.x < j * 20 + Field_Start_x + 20)
					{
						if (a_data->a_num[i][j] < 10)
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
						else if (a_data->a_num[i][j] >= 50 && a_data->a_num[i][j] < 60)
						{
							DeleteFlag(j, i);
							a_data->a_num[i][j] -= 50; //플래그 삭제 후에는 50을 빼준다
						}
						else if (a_data->a_num[i][j] == 60)
						{
							DeleteFlag(j, i);
							a_data->a_num[i][j] -= 50;
							(a_data->flagMine)--;
						}
					}
					for (int j = 0; j < 8; j++)
					{
						a_data->c_num[0][j] = a_data->a_num[0][j];
					}
				}
			}
		}
		if (a_data->WinCount == (cx * cy - a_data->a_mine) && a_data->flagMine == a_data->a_mine)
		{
			printf(720, 50, RGB(0, 0, 255), "성공");
			SelectPenObject(RGB(0, 0, 0));

			int k = 0;
			while (a_data->p_time[k][0] != 0 || a_data->p_time[k][1] != 0 || a_data->p_time[k][2] != 0)
				k++;
			if (a_data->hour != 0)
				a_data->p_time[k][0] = a_data->hour;
			if (a_data->min != 0)
				a_data->p_time[k][1] = a_data->min;
			if (a_data->sec != 0)
				a_data->p_time[k][2] = a_data->sec - 1;

			int t = 0;
			while (a_data->p_time[t][0] != 0 || a_data->p_time[t][1] != 0 || a_data->p_time[t][2] != 0) {
				if (a_data->p_time[t][0] < 10 && a_data->p_time[t][1] < 10 && a_data->p_time[t][2] < 10) {
					printf(20, 190 + t * 20, RGB(0, 0, 0), "%d판 - 0%d:0%d:0%d", t + 1, a_data->p_time[t][0], a_data->p_time[t][1], a_data->p_time[t][2]);
				}
				else if (a_data->p_time[t][0] >= 10 && a_data->p_time[t][1] < 10 && a_data->p_time[t][2] < 10) {
					printf(20, 190 + t * 20, RGB(0, 0, 0), "%d판 - %d:0%d:0%d", t + 1, a_data->p_time[t][0], a_data->p_time[t][1], a_data->p_time[t][2]);
				}
				else if (a_data->p_time[t][0] < 10 && a_data->p_time[t][1] >= 10 && a_data->p_time[t][2] < 10) {
					printf(20, 190 + t * 20, RGB(0, 0, 0), "%d판 - 0%d:%d:0%d", t + 1, a_data->p_time[t][0], a_data->p_time[t][1], a_data->p_time[t][2]);
				}
				else if (a_data->p_time[t][0] < 10 && a_data->p_time[t][1] < 10 && a_data->p_time[t][2] >= 10) {
					printf(20, 190 + t * 20, RGB(0, 0, 0), "%d판 - 0%d:0%d:%d", t + 1, a_data->p_time[t][0], a_data->p_time[t][1], a_data->p_time[t][2]);
				}
				else if (a_data->p_time[t][0] >= 10 && a_data->p_time[t][1] >= 10 && a_data->p_time[t][2] < 10) {
					printf(20, 190 + t * 20, RGB(0, 0, 0), "%d판 - %d:%d:0%d", t + 1, a_data->p_time[t][0], a_data->p_time[t][1], a_data->p_time[t][2]);
				}
				else if (a_data->p_time[t][0] >= 10 && a_data->p_time[t][1] < 10 && a_data->p_time[t][2] >= 10) {
					printf(20, 190 + t * 20, RGB(0, 0, 0), "%d판 - %d:0%d:%d", t + 1, a_data->p_time[t][0], a_data->p_time[t][1], a_data->p_time[t][2]);
				}
				else if (a_data->p_time[t][0] < 10 && a_data->p_time[t][1] >= 10 && a_data->p_time[t][2] >= 10) {
					printf(20, 190 + t * 20, RGB(0, 0, 0), "%d판 - 0%d:%d:%d", t + 1, a_data->p_time[t][0], a_data->p_time[t][1], a_data->p_time[t][2]);
				}
				else if (a_data->p_time[t][0] >= 10 && a_data->p_time[t][1] >= 10 && a_data->p_time[t][2] >= 10) {
					printf(15, 190 + t * 20, RGB(0, 0, 0), "%d판 - %d:%d:%d", t + 1, a_data->p_time[t][0], a_data->p_time[t][1], a_data->p_time[t][2]);
				}
				t++;
			}
			KillTimer(1);
			a_data->WinCount = 0;
			a_data->flagMine = 0;
		}
	}

	//  좌클릭 시
	else
	{
		//  초기에 좌클릭을 했을 시, 클릭한 좌표 빼고 필드에 지뢰를 생성
		if (a_data->fc == 0)
		{
			a_data->fc += 1;
			for (int i = 0; i < cy; i++)
			{
				for (int j = 0; j < cx; j++)
				{
					if (a_pos.y > i * 20 + Field_Start_y && a_pos.y < i * 20 + Field_Start_y + 20)
					{
						if (a_pos.x > j * 20 + Field_Start_x && a_pos.x < j * 20 + Field_Start_x + 20)
						{
							for (int c = 0; c < 29; c++) {
								for (int r = 0; r < 38; r++)
								{
									a_data->a_num[c][r] = -1;
									a_data->b_num[c][r] = -1;
								}
							}

							a_data->fx = j;
							a_data->fy = i;

							//srand(time(NULL));

							int size = cx * cy;
							int mine_num = size / 7;
							a_data->a_mine = mine_num;
							int** arr = (int**)malloc(sizeof(int*) * cy);

							for (int i = 0; i < cy; i++)
								arr[i] = (int*)malloc(sizeof(int*) * cx);

							for (int i = 0; i < cy; i++)
							{
								for (int j = 0; j < cx; j++)
								{
									arr[i][j] = 0;
								}
							}

							firstClick(arr, cx, cy, j, i, 100);
							random(arr, cx, cy, mine_num);
							firstClick(arr, cx, cy, j, i, 0);
							Hint(arr, cx, cy, mine_num);

							for (int i = 0; i < cy; i++) {
								for (int j = 0; j < cx; j++)
								{
									a_data->a_num[i][j] = arr[i][j];
									a_data->b_num[i][j] = arr[i][j];
									a_data->c_num[i][j] = arr[i][j];
								}
							}

							for (int i = 0; i < cy; i++)
							{
								free(arr[i]);
							}
							free(arr);

							PrintArray(a_data, j, i);
							a_data->a_num[i][j] += 100; //숫자가 드러난 곳에는 100을 더해서 숫자가 드러났음을 표시
							a_data->WinCount++;
							FastZero(cx, cy, j, i);

							SetTimer(1, 1000, StopWatchProc);
						}
					}
				}
			}
		}

		//  지뢰 생성 후 좌클릭 시
		else
		{
			if (a_data->fc != 0)
			{
				for (int j = 0; j < 8; j++)
				{
					a_data->a_num[0][j] = a_data->c_num[0][j];
				}
			}


			for (int i = 0; i < cy; i++)
			{
				if (a_pos.y > i * 20 + Field_Start_y && a_pos.y < i * 20 + Field_Start_y + 20)
				{
					for (int j = 0; j < cx; j++)
					{
						if (a_pos.x > j * 20 + Field_Start_x && a_pos.x < j * 20 + Field_Start_x + 20)
						{
							if (a_data->a_num[i][j] >= 50 && a_data->a_num[i][j] < 100); //이미 클릭한 0 또는 깃발꽂힌칸 제외

							else if (a_data->a_num[i][j] == 10) //종료조건
							{
								printf(720, 50, RGB(255, 0, 0), "실패");
								SelectPenObject(RGB(0, 0, 0));
								KillTimer(1);
								for (i = 0; i < cy; i++)
								{
									for (j = 0; j < cx; j++)
									{
										DeleteFlag(j, i);
										if (a_data->b_num[i][j] == 10)
										{
											Rectangle(j * 20 + Field_Start_x, i * 20 + Field_Start_y, (j + 1) * 20 + Field_Start_x, (i + 1) * 20 + Field_Start_y, RGB(0, 0, 0), RGB(255, 00, 0));
											Ellipse(j * 20 + Field_Start_x + 3, i * 20 + Field_Start_y + 3, j * 20 + Field_Start_x + 17, i * 20 + Field_Start_y + 17, RGB(0, 0, 0), RGB(2, 5, 8));
										}
										else
											PrintArray(a_data, j, i);
									}
								}
							}
							else if (a_data->a_num[i][j] < 10)
							{
								PrintArray(a_data, j, i);
								a_data->a_num[i][j] += 100; //숫자가 드러난 곳에는 100을 더해서 숫자가 드러났음을 표시
								a_data->WinCount++;
								FastZero(cx, cy, j, i);
							}
							for (int j = 0; j < 8; j++)
							{
								a_data->c_num[0][j] = a_data->a_num[0][j];
							}

						}
					}
				}
			}

			if (a_data->WinCount == (cx * cy - a_data->a_mine) && a_data->flagMine == a_data->a_mine)
			{
				printf(720, 50, RGB(0, 0, 255), "성공");
				SelectPenObject(RGB(0, 0, 0));

				int k = 0;
				while (a_data->p_time[k][0] != 0 || a_data->p_time[k][1] != 0 || a_data->p_time[k][2] != 0)
					k++;
				if (a_data->hour != 0)
					a_data->p_time[k][0] = a_data->hour;
				if (a_data->min != 0)
					a_data->p_time[k][1] = a_data->min;
				if (a_data->sec != 0)
					a_data->p_time[k][2] = a_data->sec - 1;

				int t = 0;
				while (a_data->p_time[t][0] != 0 || a_data->p_time[t][1] != 0 || a_data->p_time[t][2] != 0) {
					if (a_data->p_time[t][0] < 10 && a_data->p_time[t][1] < 10 && a_data->p_time[t][2] < 10) {
						printf(20, 190 + t * 20, RGB(0, 0, 0), "%d판 - 0%d:0%d:0%d", t + 1, a_data->p_time[t][0], a_data->p_time[t][1], a_data->p_time[t][2]);
					}
					else if (a_data->p_time[t][0] >= 10 && a_data->p_time[t][1] < 10 && a_data->p_time[t][2] < 10) {
						printf(20, 190 + t * 20, RGB(0, 0, 0), "%d판 - %d:0%d:0%d", t + 1, a_data->p_time[t][0], a_data->p_time[t][1], a_data->p_time[t][2]);
					}
					else if (a_data->p_time[t][0] < 10 && a_data->p_time[t][1] >= 10 && a_data->p_time[t][2] < 10) {
						printf(20, 190 + t * 20, RGB(0, 0, 0), "%d판 - 0%d:%d:0%d", t + 1, a_data->p_time[t][0], a_data->p_time[t][1], a_data->p_time[t][2]);
					}
					else if (a_data->p_time[t][0] < 10 && a_data->p_time[t][1] < 10 && a_data->p_time[t][2] >= 10) {
						printf(20, 190 + t * 20, RGB(0, 0, 0), "%d판 - 0%d:0%d:%d", t + 1, a_data->p_time[t][0], a_data->p_time[t][1], a_data->p_time[t][2]);
					}
					else if (a_data->p_time[t][0] >= 10 && a_data->p_time[t][1] >= 10 && a_data->p_time[t][2] < 10) {
						printf(20, 190 + t * 20, RGB(0, 0, 0), "%d판 - %d:%d:0%d", t + 1, a_data->p_time[t][0], a_data->p_time[t][1], a_data->p_time[t][2]);
					}
					else if (a_data->p_time[t][0] >= 10 && a_data->p_time[t][1] < 10 && a_data->p_time[t][2] >= 10) {
						printf(20, 190 + t * 20, RGB(0, 0, 0), "%d판 - %d:0%d:%d", t + 1, a_data->p_time[t][0], a_data->p_time[t][1], a_data->p_time[t][2]);
					}
					else if (a_data->p_time[t][0] < 10 && a_data->p_time[t][1] >= 10 && a_data->p_time[t][2] >= 10) {
						printf(20, 190 + t * 20, RGB(0, 0, 0), "%d판 - 0%d:%d:%d", t + 1, a_data->p_time[t][0], a_data->p_time[t][1], a_data->p_time[t][2]);
					}
					else if (a_data->p_time[t][0] >= 10 && a_data->p_time[t][1] >= 10 && a_data->p_time[t][2] >= 10) {
						printf(20, 190 + t * 20, RGB(0, 0, 0), "%d판 - %d:%d:%d", t + 1, a_data->p_time[t][0], a_data->p_time[t][1], a_data->p_time[t][2]);
					}
					t++;
				}
				KillTimer(1);
				a_data->WinCount = 0;
				a_data->flagMine = 0;
			}
		}
	}

	ShowDisplay();
}