#include <Windows.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <string>
int x_m, y_m;
bool flag;
int score;

struct snake_body
{
	int x;
	int y;
};
class Screen
{
private:
	int scr_width;
	int scr_height;
	wchar_t* screen_buffer;
	HANDLE screen;
	DWORD bytes_to_write;
public:
	void Create_Console(int width = 120, int height = 40)
	{
		scr_width = width;
		scr_height = height;
		bytes_to_write = 0;
		screen_buffer = new wchar_t[scr_width * scr_height];
		screen = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
		SetConsoleActiveScreenBuffer(screen);
		screen_buffer[scr_width * scr_height - 1] = '\0';
	}
	void clear()
	{
		for (int h = 0; h < scr_height; h++)
		{
			for (int w = 0; w < scr_width; w++)
			{
				screen_buffer[h * scr_width + w] = L' ';
			}
		}
	}

	void plot(const wchar_t& plot_symbol,int x,int y)
	{
		if (x > scr_width || y > scr_height) {
			return;
		}
		screen_buffer[y * scr_width + x] = plot_symbol;
	}
	void render_buffer()
	{
		WriteConsoleOutputCharacter(screen, screen_buffer, scr_width * scr_height, { 0,0 }, &bytes_to_write);
	}
	void display_score()
	{
		std::wstring f = std::to_wstring(score);
		swprintf_s(screen_buffer, f.length()+8, L"score: %i", score);
	}
	~Screen()
	{
		delete[] screen_buffer;
	}
};

class Snake
{
private:
	int x_vel, y_vel;
	snake_body trail_position;
public:
	std::vector <snake_body> part;//0 is the head,last element is tail
	int food_x, food_y;
	bool isDead, foodEaten;
	Snake()
	{
		part = { { 10,5 }};
		x_vel = 1;
		isDead = false;
		foodEaten = true;
		score = 0;
	}
	void poll_key_press()
	{
		if (GetAsyncKeyState((unsigned short)'W') & 0x8000) {
			if (x_vel != 0) {
				y_vel = -1;
				x_vel = 0;
			}
		}
		else if (GetAsyncKeyState((unsigned short)'S') & 0x8000)
		{
			if (x_vel != 0) {
				y_vel = 1;
				x_vel = 0;
			}
		}
		else if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
		{
			if (y_vel != 0) {
				y_vel = 0;
				x_vel = -1;
			}
		}
		else if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
		{
			if (y_vel != 0) {
				y_vel = 0;
				x_vel = 1;
			}
		}

	}
	void advance_snake()
	{
		snake_body prev_state, temp;
		prev_state = part[0]; //Save this state of snake head
		part[0].x += x_vel;
		part[0].y += y_vel;

		for (unsigned int i = 1; i < part.size(); i++)
		{
			temp = prev_state; //save i-1 segment state in temp
			prev_state = part[i]; //save the current state as prev state
			part[i] = temp; // assign the previous state of i-1 part to this part
		}
		trail_position = prev_state;
	}
	void perform_check()
	{
		if(part[0].x >= 120 || part[0].x <= 0 || part[0].y <= 0 || part[0].y >= 40 )
		{
			isDead = true;
		}
		
		for (unsigned int i = 1; i < part.size(); i++)
		{
			if (part[i].x == part[0].x && part[i].y == part[0].y) {
				isDead = true;
				break;
			}
		}
		if (part[0].x == food_x && part[0].y == food_y) {
			foodEaten = true;
			flag = false;
			part.push_back(trail_position);
			score++;
		}

	}

	void draw_snake(Screen& scr)
	{
		for (auto seg : part)
		{
			scr.plot(0x2588, seg.x, seg.y);
		}
	}

};

bool is_xy_in_Snake(Snake &s, int xf, int yf)
{
	bool r = false;

	for (auto seg : s.part)
	{
		if (seg.x == xf && seg.y == yf) {
			r = true;
			break;
		}
	}
	return r;
}

void set_food_position(Snake& s, Screen& scr)
{
	int sp_x, sp_y;
	srand(1000);
	do
	{
		sp_x = rand()%120;
		sp_y = rand()%40;

	} while (is_xy_in_Snake(s,sp_x,sp_y));
	s.food_x = sp_x;
	s.food_y = sp_y;
	x_m = sp_x;
	y_m = sp_y;
	//scr.plot(L'@', sp_x, sp_y);
	s.foodEaten = false;
	flag = true;
}

int main()
{
	Screen screen;
	screen.Create_Console();
	Snake player;
	while (!player.isDead) {
		std::this_thread::sleep_for(std::chrono::milliseconds(70));

		screen.clear();
		if (flag) screen.plot(L'@', x_m, y_m);// Draw the food on the screen
		if (player.foodEaten) {
			set_food_position(player, screen);
		}
		//Do some snake related stuff
		player.poll_key_press();
		player.advance_snake();
		player.perform_check();
		player.draw_snake(screen);
		screen.display_score();
		screen.render_buffer();
	}
	std::cout << "Game over!Your score:" << score << std::endl;
	return 0;
}