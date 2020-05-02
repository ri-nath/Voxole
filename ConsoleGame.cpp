# include <stdexcept>
# include <chrono>
#include <string>
using namespace std;

# include <Windows.h>

# include "ConsoleGame.h"

ConsoleGame::ConsoleGame(short screen_width, short screen_height)
	: m_screen_width(screen_width), 
	  m_screen_height(screen_height)
{
	m_screen = new CHAR_INFO[(int) m_screen_width * m_screen_height];
	memset(m_screen, 0, sizeof(CHAR_INFO) * m_screen_width * m_screen_height);

	m_buffer = CreateConsoleScreenBuffer(
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		CONSOLE_TEXTMODE_BUFFER,
		NULL
	);
}

void ConsoleGame::updateFrame(float dt)
{	
	wchar_t title[256];
	swprintf_s(title, 256, L"Console Game: FPS: %3.2f", 1.0f / dt);
	SetConsoleTitle(title);

	WriteConsoleOutput(
		m_buffer,
		m_screen,
		{ m_screen_width, m_screen_height },
		{ 0, 0 },
		& m_rect_window
	);
}

void ConsoleGame::createWindow(int font_width, int font_height)
{
	COORD screen_coords = { m_screen_width, m_screen_height };
	if (!SetConsoleScreenBufferSize(m_buffer, screen_coords))
		throw std::runtime_error("Unable to set Console Screen Buffer size.");

	if (!SetConsoleActiveScreenBuffer(m_buffer))
		throw std::runtime_error("Unable to set Console Screen Buffer as Active.");

	CONSOLE_FONT_INFOEX font_info;
	font_info.cbSize = sizeof(font_info);
	font_info.nFont = 0;
	font_info.dwFontSize.X = font_width;
	font_info.dwFontSize.Y = font_height;
	font_info.FontFamily = FF_DONTCARE;
	font_info.FontWeight = FW_NORMAL;

	wcscpy_s(font_info.FaceName, L"Consolas");
	if (!SetCurrentConsoleFontEx(m_buffer, false, &font_info))
		throw std::runtime_error("Unable to set Console font.");

	m_rect_window = { 0, 0, m_screen_width - 1, m_screen_height - 1 };
	if (!SetConsoleWindowInfo(m_buffer, TRUE, &m_rect_window))
		throw std::runtime_error("Unable to resize Console Window.");
}

void ConsoleGame::start()
{
	auto time_naught = chrono::system_clock::now();
	auto time_final = chrono::system_clock::now();

	while (TRUE)
	{
		time_final = chrono::system_clock::now();
		chrono::duration<float> chrono_dt = time_final - time_naught;
		time_naught = time_final;

		float dt = chrono_dt.count();

		onTick(dt);
		updateFrame(dt);
	}
}

void ConsoleGame::drawPixel(short x, short y, short ch, short col) 
{
	if (x >= 0 && x < m_screen_width && y >= 0 && y < m_screen_height)
	{
		m_screen[y * m_screen_width + x].Char.UnicodeChar = ch;
		m_screen[y * m_screen_width + x].Attributes = col;
	}
}

