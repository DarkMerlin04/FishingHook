#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
#include <windows.h>
#include <time.h>
#include <string.h>
#define fishcount 20
#define x2count 5
#define betcount 1

#define screen_x 90
#define screen_y 30
HANDLE wHnd;
HANDLE rHnd;
DWORD fdwMode;
CHAR_INFO consoleBuffer[screen_x * screen_y];
COORD bufferSize = { screen_x  ,screen_y };
COORD characterPos = { 0,0 };
SMALL_RECT windowSize = { 0,0,101 - 1,screen_y - 1 };
struct ocean
{
	bool active = true;
	int x = 0;
	int y = 0;
};
ocean fish[fishcount];
ocean x2[x2count];
ocean bet[betcount];
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
		consoleBuffer[(fish[i].x) + screen_x * fish[i].y].Attributes = 7;
		consoleBuffer[(fish[i].x + 1) + screen_x * fish[i].y].Attributes = 7;
		consoleBuffer[(fish[i].x + 2) + screen_x * fish[i].y].Attributes = 7;
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
		consoleBuffer[(x2[i].x) + screen_x * x2[i].y].Attributes = 7;
		consoleBuffer[(x2[i].x + 1) + screen_x * x2[i].y].Attributes = 7;
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

		consoleBuffer[(bet[i].x) + screen_x * bet[i].y].Attributes = 7;
		consoleBuffer[(bet[i].x + 1) + screen_x * bet[i].y].Attributes = 7;
		consoleBuffer[(bet[i].x + 2) + screen_x * bet[i].y].Attributes = 7;

		consoleBuffer[(bet[i].x + 3) + screen_x * bet[i].y].Attributes = 7;
		consoleBuffer[(bet[i].x + 4) + screen_x * bet[i].y].Attributes = 7;
		consoleBuffer[(bet[i].x + 5) + screen_x * bet[i].y].Attributes = 7;

		consoleBuffer[(bet[i].x + 6) + screen_x * bet[i].y].Attributes = 7;
		consoleBuffer[(bet[i].x + 7) + screen_x * bet[i].y].Attributes = 7;
		consoleBuffer[(bet[i].x + 8) + screen_x * bet[i].y].Attributes = 7;
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
	printf("score : %d", score);
}
void draw_bait(int x, int y, int bait)
{
	COORD c = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
	setcolor(7, 0);
	printf("baits : %d ", bait);
}
int main()
{
	int score = 0;
	int bait = 20;
	srand(time(NULL));
	bool play = true;
	setConsole(screen_x, screen_y);
	int x = 2, y = 2, d = 0;
	int xh = 5, yh = 2, dh = 0;
	DWORD numEvents = 0;
	DWORD numEventsRead = 0;
	setcursor(0);
	init_fish();
	init_x2();
	init_bet();
	setMode();
	while (play)
	{
		fish_move();
		x2_move();
		bet_move();
		draw_score(90, 1, score);
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
						play = false;
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
					score += 1;
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
					score *= 2;
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
		if (bait < 0) { play = false; }
		fill_bet_to_buffer();
		del_bet_to_buffer();
		fill_x2_to_buffer();
		del_x2_to_buffer();
		fill_fish_to_buffer();
		del_fish_to_buffer();
		fill_buffer_to_console();
		Sleep(50);
	}
	return 0;
}