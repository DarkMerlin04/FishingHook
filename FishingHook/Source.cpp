#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include <string.h>
#include <conio.h>
#include <algorithm>
#include <iostream>
#define fishcount 30
#define fishDeepcount 40
#define x2count 5
#define betcount 3
#define trashcount 70
#define screen_x 90
#define screen_y 35
HANDLE wHnd;
HANDLE rHnd;
DWORD fdwMode;
CHAR_INFO consoleBuffer[screen_x * screen_y];
COORD bufferSize = { screen_x  ,screen_y };
COORD characterPos = { 0,0 };
SMALL_RECT windowSize = { 0,0,101 - 1,screen_y - 1 };
FILE* fp;
void gameover(int x);
void menu();
struct ocean
{
	bool active = true;
	int x = 0;
	int y = 0;
};
ocean fish[fishcount];
ocean x2[x2count];
ocean bet[betcount];
ocean fishDeep[fishDeepcount];
ocean trash[trashcount];
struct build
{
	char name[50];
	int score;
};
build character[6];
void SetConsoleSize(int Width, int Height) {
	HWND console = GetConsoleWindow();
	RECT r;
	GetWindowRect(console, &r);
	MoveWindow(console, r.left, r.top, Width, Height, TRUE);
}
void SetConsoleFontSize(int width, int height) {
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
	PCONSOLE_FONT_INFOEX lpConsoleCurrenFontEx = new CONSOLE_FONT_INFOEX();
	lpConsoleCurrenFontEx->cbSize = sizeof(CONSOLE_FONT_INFOEX);
	GetCurrentConsoleFontEx(out, 0, lpConsoleCurrenFontEx);
	lpConsoleCurrenFontEx->dwFontSize.X = width;
	lpConsoleCurrenFontEx->dwFontSize.Y = height;
	SetCurrentConsoleFontEx(out, 0, lpConsoleCurrenFontEx);
}
char cursor(int x, int y)
{
	HANDLE hStd = GetStdHandle(STD_OUTPUT_HANDLE);
	char buf[2]; COORD c = { x,y }; DWORD num_read;
	if (
		!ReadConsoleOutputCharacter(hStd, (LPTSTR)buf, 1, c, (LPDWORD)&num_read))
		return '\0';
	else
		return buf[0];
}
void setcursor(bool visible)
{
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO lpCursor;
	lpCursor.bVisible = visible;
	lpCursor.dwSize = 20;
	SetConsoleCursorInfo(console, &lpCursor);
}
void setcolor(int fg, int bg)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, bg * 16 + fg);
}
void clear_buffer()
{
	for (int y = 0; y < screen_y; ++y) {
		for (int x = 0; x < screen_x; ++x) {
			consoleBuffer[x + screen_x * y].Char.AsciiChar = ' ';
			consoleBuffer[x + screen_x * y].Attributes = 0;
		}
	}
}
void fill_buffer_to_console()
{
	WriteConsoleOutputA(wHnd, consoleBuffer, bufferSize, characterPos,
		&windowSize);
}
void gotoxy(int x, int y) {
	COORD c;
	c.X = x;
	c.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}
int comparator(const void* p, const void* q)
{
	int l = ((struct build*)p)->score;
	int r = ((struct build*)q)->score;
	return (l - r);
}
void draw_ship(int x, int y)
{
	COORD c = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
	consoleBuffer[(c.X - 2) + screen_x * c.Y].Char.AsciiChar = '\\';
	consoleBuffer[(c.X - 1) + screen_x * c.Y].Char.AsciiChar = '_';
	consoleBuffer[c.X + screen_x * c.Y].Char.AsciiChar = '_';
	consoleBuffer[(c.X + 1) + screen_x * c.Y].Char.AsciiChar = '_';
	consoleBuffer[(c.X + 2) + screen_x * c.Y].Char.AsciiChar = '/';

	consoleBuffer[(c.X - 2) + screen_x * c.Y].Attributes = 7;
	consoleBuffer[(c.X - 1) + screen_x * c.Y].Attributes = 7;
	consoleBuffer[c.X + screen_x * c.Y].Attributes = 7;
	consoleBuffer[(c.X + 1) + screen_x * c.Y].Attributes = 7;
	consoleBuffer[(c.X + 2) + screen_x * c.Y].Attributes = 7;

}
void draw_hook(int x, int y)
{
	COORD c = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
	consoleBuffer[c.X + screen_x * c.Y].Char.AsciiChar = '|';
	consoleBuffer[c.X + screen_x * (c.Y + 1)].Char.AsciiChar = '|';
	consoleBuffer[c.X + screen_x * (c.Y + 2)].Char.AsciiChar = 'V';

	consoleBuffer[c.X + screen_x * c.Y].Attributes = 7;
	consoleBuffer[c.X + screen_x * (c.Y + 1)].Attributes = 7;
	consoleBuffer[c.X + screen_x * (c.Y + 2)].Attributes = 7;

}

void init_fish()
{
	for (int i = 0; i < fishcount; i++)
	{
		fish[i].y = rand() % screen_y + 8;
		fish[i].x = rand() % screen_x;
	}
}
void fish_move()
{
	for (int i = 0; i < fishcount; i++) {
		if (fish[i].x >= screen_x) {
			fish[i].y = ((rand() % screen_y) + 8);
			fish[i].x = 0;
			fish[i].active = true;
		}
		else {
			fish[i].x = fish[i].x + 1;
			fish[i].y = fish[i].y;
		}
	}
}
void fill_fish_to_buffer()
{
	for (int i = 0; i < fishcount; ++i) {
		consoleBuffer[(fish[i].x) + screen_x * fish[i].y].Char.AsciiChar = '>';
		consoleBuffer[(fish[i].x + 1) + screen_x * fish[i].y].Char.AsciiChar = 'O';
		consoleBuffer[(fish[i].x + 2) + screen_x * fish[i].y].Char.AsciiChar = 'D';
		consoleBuffer[(fish[i].x) + screen_x * fish[i].y].Attributes = 0x0010;
		consoleBuffer[(fish[i].x + 1) + screen_x * fish[i].y].Attributes = 0x0010;
		consoleBuffer[(fish[i].x + 2) + screen_x * fish[i].y].Attributes = 0x0010;
	}
}
void del_fish_to_buffer()
{
	for (int i = 0; i < fishcount; ++i) {
		if (fish[i].active == false)
		{
			consoleBuffer[(fish[i].x) + screen_x * fish[i].y].Char.AsciiChar = '>';
			consoleBuffer[(fish[i].x + 1) + screen_x * fish[i].y].Char.AsciiChar = 'O';
			consoleBuffer[(fish[i].x + 2) + screen_x * fish[i].y].Char.AsciiChar = 'D';
			consoleBuffer[(fish[i].x) + screen_x * fish[i].y].Attributes = 0;
			consoleBuffer[(fish[i].x + 1) + screen_x * fish[i].y].Attributes = 0;
			consoleBuffer[(fish[i].x + 2) + screen_x * fish[i].y].Attributes = 0;
		}
	}
}

void init_fishDeep()
{
	for (int i = 0; i < fishDeepcount; i++)
	{
		fishDeep[i].y = (rand() % 9) + 25;
		fishDeep[i].x = rand() % screen_x;
	}
}
void fishDeep_move()
{
	for (int i = 0; i < fishDeepcount; i++) {
		if (fishDeep[i].x >= screen_x) {
			fishDeep[i].y = ((rand() % 9) + 25);
			fishDeep[i].x = 0;
			fishDeep[i].active = true;
		}
		else {
			fishDeep[i].x = fishDeep[i].x + 1;
			fishDeep[i].y = fishDeep[i].y;
		}
	}
}
void fill_fishDeep_to_buffer()
{
	for (int i = 0; i < fishDeepcount; ++i) {
		if (fishDeep[i].y > 25 && fishDeep[i].y < 35)
		{
			consoleBuffer[(fishDeep[i].x) + screen_x * fishDeep[i].y].Char.AsciiChar = '}';
			consoleBuffer[(fishDeep[i].x + 1) + screen_x * fishDeep[i].y].Char.AsciiChar = '#';
			consoleBuffer[(fishDeep[i].x + 2) + screen_x * fishDeep[i].y].Char.AsciiChar = '#';
			consoleBuffer[(fishDeep[i].x + 3) + screen_x * fishDeep[i].y].Char.AsciiChar = ']';
			consoleBuffer[(fishDeep[i].x) + screen_x * fishDeep[i].y].Attributes = 0x0010;
			consoleBuffer[(fishDeep[i].x + 1) + screen_x * fishDeep[i].y].Attributes = 0x0010;
			consoleBuffer[(fishDeep[i].x + 2) + screen_x * fishDeep[i].y].Attributes = 0x0010;
			consoleBuffer[(fishDeep[i].x + 3) + screen_x * fishDeep[i].y].Attributes = 0x0010;
		}
		else
		{
			fishDeep[i].active = false;
		}
	}
}
void del_fishDeep_to_buffer()
{
	for (int i = 0; i < fishDeepcount; ++i) {
		if (fishDeep[i].active == false)
		{
			consoleBuffer[(fishDeep[i].x) + screen_x * fishDeep[i].y].Char.AsciiChar = '}';
			consoleBuffer[(fishDeep[i].x + 1) + screen_x * fishDeep[i].y].Char.AsciiChar = '#';
			consoleBuffer[(fishDeep[i].x + 2) + screen_x * fishDeep[i].y].Char.AsciiChar = '#';
			consoleBuffer[(fishDeep[i].x + 3) + screen_x * fishDeep[i].y].Char.AsciiChar = ']';
			consoleBuffer[(fishDeep[i].x) + screen_x * fishDeep[i].y].Attributes = 0;
			consoleBuffer[(fishDeep[i].x + 1) + screen_x * fishDeep[i].y].Attributes = 0;
			consoleBuffer[(fishDeep[i].x + 2) + screen_x * fishDeep[i].y].Attributes = 0;
			consoleBuffer[(fishDeep[i].x + 3) + screen_x * fishDeep[i].y].Attributes = 0;
		}
	}
}

void init_x2()
{
	for (int i = 0; i < x2count; i++)
	{
		x2[i].y = rand() % screen_y + 8;
		x2[i].x = rand() % screen_x;
	}
}
void x2_move()
{
	for (int i = 0; i < x2count; i++) {
		if (x2[i].x >= screen_x) {
			x2[i].y = ((rand() % screen_y) + 8);
			x2[i].x = 0;
			x2[i].active = true;
		}
		else {
			x2[i].x = x2[i].x + 1;
			x2[i].y = x2[i].y;
		}
	}
}
void fill_x2_to_buffer()
{
	for (int i = 0; i < x2count; ++i) {
		consoleBuffer[(x2[i].x) + screen_x * x2[i].y].Char.AsciiChar = 'x';
		consoleBuffer[(x2[i].x + 1) + screen_x * x2[i].y].Char.AsciiChar = '2';
		consoleBuffer[(x2[i].x) + screen_x * x2[i].y].Attributes = 0x0020;
		consoleBuffer[(x2[i].x + 1) + screen_x * x2[i].y].Attributes = 0x0020;
	}
}
void del_x2_to_buffer()
{
	for (int i = 0; i < x2count; ++i) {
		if (x2[i].active == false)
		{
			consoleBuffer[(x2[i].x) + screen_x * x2[i].y].Char.AsciiChar = 'x';
			consoleBuffer[(x2[i].x + 1) + screen_x * x2[i].y].Char.AsciiChar = '2';
			consoleBuffer[(x2[i].x) + screen_x * x2[i].y].Attributes = 0;
			consoleBuffer[(x2[i].x + 1) + screen_x * x2[i].y].Attributes = 0;
		}
	}
}

void init_bet()
{
	for (int i = 0; i < betcount; i++)
	{
		bet[i].y = rand() % screen_y + 8;
		bet[i].x = rand() % screen_x;
	}
}
void bet_move()
{
	for (int i = 0; i < betcount; i++) {
		if (bet[i].x >= screen_x) {
			bet[i].y = ((rand() % screen_y) + 8);
			bet[i].x = 0;
			bet[i].active = true;
		}
		else {
			bet[i].x = bet[i].x + 1;
			bet[i].y = bet[i].y;
		}
	}
}
void fill_bet_to_buffer()
{
	for (int i = 0; i < betcount; ++i) {
		consoleBuffer[(bet[i].x) + screen_x * bet[i].y].Char.AsciiChar = '-';
		consoleBuffer[(bet[i].x + 1) + screen_x * bet[i].y].Char.AsciiChar = '-';
		consoleBuffer[(bet[i].x + 2) + screen_x * bet[i].y].Char.AsciiChar = 'B';

		consoleBuffer[(bet[i].x + 3) + screen_x * bet[i].y].Char.AsciiChar = '-';
		consoleBuffer[(bet[i].x + 4) + screen_x * bet[i].y].Char.AsciiChar = '-';
		consoleBuffer[(bet[i].x + 5) + screen_x * bet[i].y].Char.AsciiChar = 'B';

		consoleBuffer[(bet[i].x + 6) + screen_x * bet[i].y].Char.AsciiChar = '-';
		consoleBuffer[(bet[i].x + 7) + screen_x * bet[i].y].Char.AsciiChar = '-';
		consoleBuffer[(bet[i].x + 8) + screen_x * bet[i].y].Char.AsciiChar = 'B';

		consoleBuffer[(bet[i].x) + screen_x * bet[i].y].Attributes = 0x0020;
		consoleBuffer[(bet[i].x + 1) + screen_x * bet[i].y].Attributes = 0x0020;
		consoleBuffer[(bet[i].x + 2) + screen_x * bet[i].y].Attributes = 0x0020;

		consoleBuffer[(bet[i].x + 3) + screen_x * bet[i].y].Attributes = 0x0020;
		consoleBuffer[(bet[i].x + 4) + screen_x * bet[i].y].Attributes = 0x0020;
		consoleBuffer[(bet[i].x + 5) + screen_x * bet[i].y].Attributes = 0x0020;

		consoleBuffer[(bet[i].x + 6) + screen_x * bet[i].y].Attributes = 0x0020;
		consoleBuffer[(bet[i].x + 7) + screen_x * bet[i].y].Attributes = 0x0020;
		consoleBuffer[(bet[i].x + 8) + screen_x * bet[i].y].Attributes = 0x0020;
	}
}
void del_bet_to_buffer()
{
	for (int i = 0; i < betcount; ++i) {
		if (bet[i].active == false)
		{
			consoleBuffer[(bet[i].x) + screen_x * bet[i].y].Char.AsciiChar = '-';
			consoleBuffer[(bet[i].x + 1) + screen_x * bet[i].y].Char.AsciiChar = '-';
			consoleBuffer[(bet[i].x + 2) + screen_x * bet[i].y].Char.AsciiChar = 'B';

			consoleBuffer[(bet[i].x + 3) + screen_x * bet[i].y].Char.AsciiChar = '-';
			consoleBuffer[(bet[i].x + 4) + screen_x * bet[i].y].Char.AsciiChar = '-';
			consoleBuffer[(bet[i].x + 5) + screen_x * bet[i].y].Char.AsciiChar = 'B';

			consoleBuffer[(bet[i].x + 6) + screen_x * bet[i].y].Char.AsciiChar = '-';
			consoleBuffer[(bet[i].x + 7) + screen_x * bet[i].y].Char.AsciiChar = '-';
			consoleBuffer[(bet[i].x + 8) + screen_x * bet[i].y].Char.AsciiChar = 'B';

			consoleBuffer[(bet[i].x) + screen_x * bet[i].y].Attributes = 0;
			consoleBuffer[(bet[i].x + 1) + screen_x * bet[i].y].Attributes = 0;
			consoleBuffer[(bet[i].x + 2) + screen_x * bet[i].y].Attributes = 0;

			consoleBuffer[(bet[i].x + 3) + screen_x * bet[i].y].Attributes = 0;
			consoleBuffer[(bet[i].x + 4) + screen_x * bet[i].y].Attributes = 0;
			consoleBuffer[(bet[i].x + 5) + screen_x * bet[i].y].Attributes = 0;

			consoleBuffer[(bet[i].x + 6) + screen_x * bet[i].y].Attributes = 0;
			consoleBuffer[(bet[i].x + 7) + screen_x * bet[i].y].Attributes = 0;
			consoleBuffer[(bet[i].x + 8) + screen_x * bet[i].y].Attributes = 0;
		}
	}
}

void init_trash()
{
	for (int i = 0; i < trashcount; i++)
	{
		trash[i].y = rand() % screen_y + 8;
		trash[i].x = rand() % screen_x;
	}
}
void trash_move()
{
	for (int i = 0; i < trashcount; i++) {
		if (trash[i].x >= screen_x) {
			trash[i].y = ((rand() % screen_y) + 8);
			trash[i].x = 0;
			trash[i].active = true;
		}
		else {
			trash[i].x = trash[i].x + 1;
			trash[i].y = trash[i].y;
		}
	}
}
void fill_trash_to_buffer()
{
	for (int i = 0; i < trashcount; ++i) {
		consoleBuffer[(trash[i].x) + screen_x * trash[i].y].Char.AsciiChar = 'P';
		consoleBuffer[(trash[i].x) + screen_x * trash[i].y].Attributes = 0x0040;
	}
}
void del_trash_to_buffer()
{
	for (int i = 0; i < trashcount; ++i) {
		if (trash[i].active == false)
		{
			consoleBuffer[(trash[i].x) + screen_x * trash[i].y].Char.AsciiChar = 'P';
			consoleBuffer[(trash[i].x) + screen_x * trash[i].y].Attributes = 0;
		}
	}
}
int setMode()
{
	rHnd = GetStdHandle(STD_INPUT_HANDLE);
	fdwMode = ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT |
		ENABLE_MOUSE_INPUT;
	SetConsoleMode(rHnd, fdwMode);
	return 0;
}
int setConsole(int x, int y)
{
	wHnd = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleWindowInfo(wHnd, TRUE, &windowSize);
	SetConsoleScreenBufferSize(wHnd, bufferSize);
	return 0;
}
void draw_score(int x, int y, int score)
{
	COORD c = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
	setcolor(7, 0);
	printf("score : %d ", score);
}
void draw_bait(int x, int y, int bait)
{
	COORD c = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
	setcolor(7, 0);
	printf("baits : %d ", bait);
}

void score()
{
	gotoxy(24,5);
	printf("+++++++++++++++++++++ SCORE TABLE +++++++++++++++++++");
	gotoxy(24, 7);
	printf("+          Name                         Score       +");
	fp = fopen("player_score.txt", "r");
	while (!feof(fp))
	{
		for (int i = 0; i < 5; i++)
		{
			gotoxy(34, 9 + 2*i);
			fscanf(fp, "%s %d\n", &character[i].name, &character[i].score);
			printf("%s\n", character[i].name);
			gotoxy(64, 9 + 2*i);
			printf("%d\n", character[i].score);
		}
	}
	gotoxy(24, 20);
	printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++");
	fclose(fp);
	gotoxy(24, 25); printf("          PRESS ANY KEY TO GO BACK MENU       ");
	_getch();
	system("CLS");
	menu();
}
void game() 
{
	int play = 1;
	int point = 0;
	int bait = 20;
	srand(time(NULL));
	int x = 2, y = 2, d = 0;
	int xh = 5, yh = 2, dh = 0;
	DWORD numEvents = 0;
	DWORD numEventsRead = 0;
	setcursor(0);
	setMode();
	init_fish();
	init_fishDeep();
	init_x2();
	init_bet();
	init_trash();
	while (play)
	{
		fish_move();
		fishDeep_move();
		x2_move();
		bet_move();
		trash_move();
		draw_score(90, 1, point);
		draw_bait(90, 2, bait);
		clear_buffer();
		GetNumberOfConsoleInputEvents(rHnd, &numEvents);
		if (numEvents != 0) {
			INPUT_RECORD* eventBuffer = new INPUT_RECORD[numEvents];
			ReadConsoleInput(rHnd, eventBuffer, numEvents, &numEventsRead);
			for (DWORD i = 0; i < numEventsRead; ++i) {
				if (eventBuffer[i].EventType == KEY_EVENT &&
					eventBuffer[i].Event.KeyEvent.bKeyDown == true) {
					if (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE) {
						play = 0;
					}
					if (eventBuffer[i].Event.KeyEvent.uChar.AsciiChar == 'd') {
						d = 1;
					}
					if (eventBuffer[i].Event.KeyEvent.uChar.AsciiChar == 'a') {
						d = -1;
					}
					if (eventBuffer[i].Event.KeyEvent.uChar.AsciiChar == 's') {
						d = 0;
					}
					if (eventBuffer[i].Event.KeyEvent.uChar.AsciiChar == ' ') {
						dh = 1;
						bait -= 1;
					}
				}
			}
		}
		if (dh == 1 && yh < screen_y && d == 0)
		{
			if ((cursor(xh, yh + 2) == '>' || cursor(xh, yh + 2) == 'O' || cursor(xh, yh + 2) == 'D'))
			{
				for (int i = 0; i < fishcount; i++)
				{
					if ((xh == fish[i].x - 1 || xh == fish[i].x || xh == fish[i].x + 1 || xh == fish[i].x + 2) && yh + 2 == fish[i].y)
					{
						fish[i].active = false;
					}
				}
				point += 1;
				yh = 2;
				dh = 0;
			}
			else if (cursor(xh, yh + 2) == 'x' || cursor(xh, yh + 2) == '2')
			{
				for (int i = 0; i < x2count; i++)
				{
					if ((xh == x2[i].x - 1 || xh == x2[i].x || xh == x2[i].x + 1 || xh == x2[i].x + 2) && yh + 2 == x2[i].y)
					{
						x2[i].active = false;
					}
				}
				point *= 2;
				yh = 2;
				dh = 0;
			}
			else if (cursor(xh, yh + 2) == '-' || cursor(xh, yh + 2) == 'B')
			{
				for (int i = 0; i < betcount; i++)
				{
					if ((xh == bet[i].x - 1 || xh == bet[i].x || xh == bet[i].x + 1 || xh == bet[i].x + 2 || xh == bet[i].x + 3)
						|| xh == bet[i].x + 4 || xh == bet[i].x + 5 || xh == bet[i].x + 6 || xh == bet[i].x + 7 || xh == bet[i].x + 8
						&& yh + 2 == x2[i].y)
					{
						bet[i].active = false;
					}
				}
				bait += 3;
				yh = 2;
				dh = 0;
			}
			else if ((cursor(xh, yh + 2) == '}' || cursor(xh, yh + 2) == '#' || cursor(xh, yh + 2) == ']'))
			{

				for (int i = 0; i < fishDeepcount; i++)
				{
					if ((xh == fishDeep[i].x - 1 || xh == fishDeep[i].x || xh == fishDeep[i].x + 1 || xh == fishDeep[i].x + 2) && yh + 2 == fishDeep[i].y)
					{
						fishDeep[i].active = false;
					}
				}
				point += 2;
				yh = 2;
				dh = 0;
			}
			else if (cursor(xh, yh + 2) == 'P')
			{
				for (int i = 0; i < trashcount; i++)
				{
					if (xh == trash[i].x || xh == trash[i].x - 1 && yh + 2 == trash[i].y)
					{
						trash[i].active = false;
					}
				}
				point -= 1;
				yh = 2;
				dh = 0;
			}
			else
			{
				draw_ship(x, y); draw_hook(xh, ++yh);
			}
		}
		else { yh = 2; dh = 0; }
		if (d == -1 && (x - 2) >= 1) { draw_ship(--x, y); draw_hook(--xh, yh); }
		else { draw_ship(x, y); draw_hook(xh, yh); }
		if (d == 1 && (x + 2) <= 87) { draw_ship(++x, y); draw_hook(++xh, yh); }
		else { draw_ship(x, y); draw_hook(xh, yh); }
		if (d == 0) { draw_ship(x, y); draw_hook(xh, yh); }
		if (bait == -1) 
		{ 
			play = 0;
			bait = 20;
		}
		fill_trash_to_buffer();
		del_trash_to_buffer();
		fill_bet_to_buffer();
		del_bet_to_buffer();
		fill_x2_to_buffer();
		del_x2_to_buffer();
		fill_fish_to_buffer();
		del_fish_to_buffer();
		fill_fishDeep_to_buffer();
		del_fishDeep_to_buffer();
		fill_buffer_to_console();
		Sleep(50);
	}
	system("CLS");
	clear_buffer();
	fill_buffer_to_console();
	gameover(point);
}

void menu()
{
	char c = '0';
	int scroll = 0;
	bool exit = false;
	while (exit == false)
	{
		setcolor(3, 0);
		gotoxy(10, 2);
		printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");

		setcolor(1, 0);
		gotoxy(45, 3);
		printf("Fishing Hook");

		setcolor(2, 0);
		gotoxy(45, 6);
		printf("W");
		gotoxy(45, 14);
		printf("S");

		setcolor(7, 0);
		gotoxy(42, 8);
		printf("Start");
		gotoxy(42, 10);
		printf("Score");
		gotoxy(42, 12);
		printf("Exit");

		setcolor(3, 0);
		gotoxy(10, 17);
		printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");

		while (c != char(13))
		{
			setcolor(2, 0);
			gotoxy(38, 8 + scroll);
			printf("->");
			c = _getch();
			gotoxy(38, 8 + scroll);
			printf(" ");
			gotoxy(38, 8 + scroll);
			printf(" ");
			printf("  ");
			if (c == 's') {
				if (scroll < 4) {
					scroll += 2;
				}
			}
			if (c == 'w') {
				if (scroll > 0) {
					scroll -= 2;
				}
			}
		}
		if (scroll == 4)
		{
			exit = true;
		}
		if (scroll == 2)
		{
			exit = true;
			system("CLS");
			score();
		}
		if (scroll == 0)
		{
			exit = true;
			game();
		}
		scroll = 2;
		c = ' ';
	}
}
void gameover(int x)
{
	char playername[50];
	setcursor(1);
	gotoxy(14, 4); printf("++++++++++++++++++++++++++++++++++++++++++++++++++");
	gotoxy(14, 5); printf("+                                                +");
	gotoxy(14, 6); printf("+                   Game Over!!!                 +");
	gotoxy(14, 7); printf("+                                                +");
	gotoxy(14, 8); printf("+                Your Score : %d", x);
	gotoxy(63, 8); printf("+");
	gotoxy(14, 9); printf("+                                                +");
	gotoxy(14, 10); printf("+               Enter Your Name : "); scanf("%s", playername);
	gotoxy(63, 10); printf("+");
	gotoxy(14, 11); printf("+                                                +");
	gotoxy(14, 12); printf("+                                                +");
	gotoxy(14, 13); printf("++++++++++++++++++++++++++++++++++++++++++++++++++");
	gotoxy(14, 17); printf("          PRESS ANY KEY TO GO BACK MENU       ");
	setcursor(0);
	fp = fopen("player_score.txt", "r");
	for (int i = 0; i < 5; i++)
	{
		fscanf(fp, "%s %d\n", &character[i].name, &character[i].score);
	}
	fclose(fp);
	strcpy(character[5].name, playername);
	character[5].score = x;
	qsort(character, sizeof(character) / sizeof(character[0]), sizeof(character[0]), comparator);
	fp = fopen("player_score.txt", "w");
	for (int i = 5; i > 0; i--)
	{
		fprintf(fp, "%s %d\n", character[i].name, character[i].score);
	}
	fclose(fp);
	_getch();
	system("CLS");
	menu();
}
int main()
{
	setcursor(0);
	setConsole(screen_x, screen_y);
	SetConsoleFontSize(20, 20);
	menu();
	return 0;
}