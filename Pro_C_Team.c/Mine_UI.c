#include "pch.h"         
#include <time.h>         
#include "tipsware.h"

#define Field_Start_x 225
#define Field_Start_y 100

void SetField(INT32 a_ctrl_id, INT32 a_notify_code, void* ap_ctrl) {
	int Field_Size_x = 0;
	int Field_Size_y = 0;

	int x = Field_Start_x;
	int y = Field_Start_y;

	int i = 0;

	char xy[15] = { NULL, };

	switch (a_ctrl_id) {
	case 1:
		GetCtrlName(FindControl(0), xy, 15);

		//Field_Size_x
		while (xy[i] != ',') {
			Field_Size_x += xy[i] - 48;
			if (xy[i + 1] != ',')
				Field_Size_x *= 10;
			i++;
		}
		i += 2;

		//Field_Size_y
		while (xy[i] != NULL) {
			Field_Size_y += xy[i] - 48;
			if (xy[i + 1] != NULL)
				Field_Size_y *= 10;
			i++;
		}

		if (Field_Size_x != 0 && Field_Size_y != 0) {
			for (int c = 0; c < Field_Size_y; c++) {
				for (int l = 0; l < Field_Size_x; l++) {
					Rectangle(x, y, x + 20, y + 20, RGB(0, 0, 0), RGB(222, 239, 255));
					x += 20;
				}
				x = Field_Start_x;
				y += 20;
			}
		}
		else
			printf(10, 30, "필드 생성 에러!");
		break;
	case 2:
		Field_Size_x = 6;
		Field_Size_y = 8;

		if (Field_Size_x != 0 && Field_Size_y != 0) {
			for (int c = 0; c < Field_Size_y; c++) {
				for (int l = 0; l < Field_Size_x; l++) {
					Rectangle(x, y, x + 20, y + 20, RGB(0, 0, 0), RGB(222, 239, 255));
					x += 20;
				}
				x = Field_Start_x;
				y += 20;
			}
		}
		break;
	case 3:
		Field_Size_x = 12;
		Field_Size_y = 14;

		if (Field_Size_x != 0 && Field_Size_y != 0) {
			for (int c = 0; c < Field_Size_y; c++) {
				for (int l = 0; l < Field_Size_x; l++) {
					Rectangle(x, y, x + 20, y + 20, RGB(0, 0, 0), RGB(222, 239, 255));
					x += 20;
				}
				x = Field_Start_x;
				y += 20;
			}
		}
		break;
	case 4:
		Field_Size_x = 18;
		Field_Size_y = 20;

		if (Field_Size_x != 0 && Field_Size_y != 0) {
			for (int c = 0; c < Field_Size_y; c++) {
				for (int l = 0; l < Field_Size_x; l++) {
					Rectangle(x, y, x + 20, y + 20, RGB(0, 0, 0), RGB(222, 239, 255));
					x += 20;
				}
				x = Field_Start_x;
				y += 20;
			}
		}
		break;
	case 5:

		break;
	}
	ShowDisplay();
}

CMD_MESSAGE(SetField)

// 프로그램에서 사용할 내부 데이터
typedef struct AppData
{
	char time_str[32];
} AD;

TIMER Timer1(NOT_USE_TIMER_DATA)
{
	AD* p_app = (AD*)GetAppData();

	RECT time_rect = { 290, 105, 400, 135 };
	Rectangle(&time_rect);

	_strtime_s(p_app->time_str, 32);
	TextCenterOut(&time_rect, p_app->time_str);

	ShowDisplay();
}

int main()
{
	//  창 크기 조절(그냥 함)
	ChangeWorkSize(1000, 700);

	//  왼쪽 사용자 창
	Rectangle(10, 10, 200, 690, RGB(0, 0, 0));
	//  오른쪽 지뢰찾기 필드 출력 창
	Rectangle(215, 10, 990, 690, RGB(0, 0, 0));
	Rectangle(225, 100, 980, 680, RGB(0, 0, 0));

	//  오른쪽 타이머 생성
	AppData data = { NULL, };
	SetAppData(&data, sizeof(AppData));
	printf(230, 105, "타이머 ");
	SetTimer(1, 500, Timer1);

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

	ShowDisplay();

	return 0;
}