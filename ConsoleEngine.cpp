# include <stdexcept>
# include <chrono>
#include <string>
using namespace std;

# include <Windows.h>
# include "ConsoleEngine.h"

ConsoleEngine::ConsoleEngine(int screen_width, int screen_height)
	:m_screen_width(screen_width), 
	m_screen_height(screen_height)
{
	m_screen = new CHAR_INFO[m_screen_width * m_screen_height];
	memset(m_screen, 0, sizeof(CHAR_INFO) * m_screen_width * m_screen_height);

	m_buffer = CreateConsoleScreenBuffer(
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		CONSOLE_TEXTMODE_BUFFER,
		NULL
	);
}

void ConsoleEngine::updateFrame(float dt)
{	
	for (int nx = 0; nx < m_map_width; nx++)
		for (int ny = 0; ny < m_map_height; ny++)
		{
			drawPixel(ny + 1, nx, L'0' + m_heightmap[ny * m_map_width + nx]);
		}
	drawPixel(m_playerY, (m_playerX + 1) * m_screen_width, 'P');
	drawPixel(m_screen_width, m_screen_height - 1, 'P');

	WriteConsoleOutput(
		m_buffer,
		m_screen,
		{ m_screen_width, m_screen_height },
		{ 0, 0 },
		& m_rect_window
	);
}

void ConsoleEngine::createWindow(int font_width, int font_height)
{
	COORD screen_coords = { m_screen_width, m_screen_height };
	if (!SetConsoleScreenBufferSize(m_buffer, screen_coords))
		throw std::runtime_error("Unable to set Console Screen Buffer size.");

	if (!SetConsoleActiveScreenBuffer(m_buffer))
		throw std::runtime_error("Unable to set Console Screen Buffer as Active.");

	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(cfi);
	cfi.nFont = 0;
	cfi.dwFontSize.X = font_width;
	cfi.dwFontSize.Y = font_height;
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = FW_NORMAL;

	wcscpy_s(cfi.FaceName, L"Consolas");
	if (!SetCurrentConsoleFontEx(m_buffer, false, &cfi))
		throw std::runtime_error("Unable to set Console font.");

	m_rect_window = { 0, 0, m_screen_width - 1, m_screen_height - 1 };
	if (!SetConsoleWindowInfo(m_buffer, TRUE, &m_rect_window))
		throw std::runtime_error("Unable to resize Console Window.");
}

void ConsoleEngine::start()
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

void ConsoleEngine::drawPixel(short x, short y, short ch, short col) 
{
	if (x >= 0 && x < m_screen_width && y >= 0 && y < m_screen_height)
	{
		m_screen[y * m_screen_width + x].Char.UnicodeChar = ch;
		m_screen[y * m_screen_width + x].Attributes = col;
	}
}

