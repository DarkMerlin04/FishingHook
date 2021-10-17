#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
#include <windows.h>
#include <time.h>
#include <string.h>
#define fishcount 20

#define screen_x 90
#define screen_y 30
HANDLE wHnd;
HANDLE rHnd;
DWORD fdwMode;
CHAR_INFO consoleBuffer[screen_x * screen_y];
COORD bufferSize = {screen_x  ,screen_y };
COORD characterPos = { 0,0 };
SMALL_RECT windowSize = { 0,0,101 - 1,screen_y - 1 };
DWORD aj = 7;
struct ocean 
{
	bool active = true;
	int x = 0;
	int y = 0;
};
ocean fish[fishcount];
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
	consoleBuffer[c.X + screen_x * (c.Y+1)].Char.AsciiChar = '|';
	consoleBuffer[c.X + screen_x * (c.Y+2)].Char.AsciiChar = 'V';

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
		consoleBuffer[fish[i].x + screen_x * fish[i].y].Attributes = aj;
		consoleBuffer[(fish[i].x + 1) + screen_x * fish[i].y].Attributes = aj;
		consoleBuffer[(fish[i].x + 2) + screen_x * fish[i].y].Attributes = aj;
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
int main()
{
	int score = 0;
	srand(time(NULL));
	bool play = true;
	setConsole(screen_x, screen_y);
	int x = 2, y = 2, d = 0;
	int xh = 2, yh = 2, dh = 0;
	DWORD numEvents = 0;
	DWORD numEventsRead = 0;
	setcursor(0);
	init_fish();
	setMode();
	while (play)
	{
		aj = 7;
		fish_move();
		draw_score(90,1,score);
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
						d = 0;
					}
				}
			}
		}
		if (d == -1 && (x - 2) >= 1) { draw_ship(--x, y); draw_hook(--xh + 3, yh);}
		if (d == 1 && (x + 2) <= 90) { draw_ship(++x, y); draw_hook(++xh + 3, yh);}
		if (d == 0) { draw_ship(x, y); draw_hook(xh + 3, yh);}
		if (dh == 1 && (yh + 2) < screen_y) {
			if ((cursor(xh + 3, yh + 2)) == 'D' || (cursor(xh + 3, yh + 2)) == 'O' || (cursor(xh + 3, yh +2)) == '>')
			{
				score += 1;
				yh = 2;
				dh = 0;
			}
			else
			{
				draw_ship(x, y); draw_hook(xh + 3, ++yh);
			}
		}
		else { yh = 2; dh = 0; }
		fill_fish_to_buffer();
		fill_buffer_to_console();
		Sleep(50);
	}
	return 0;
}