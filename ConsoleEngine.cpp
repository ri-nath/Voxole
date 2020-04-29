# include <stdexcept>
# include <chrono>
#include <string>
using namespace std;

# include <Windows.h>
# include "ConsoleEngine.h"

ConsoleEngine::ConsoleEngine()
	:m_screen(new wchar_t[m_screen_width * m_screen_height + 10000]), bytes_written(0)
{
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
	//swprintf_s(m_screen, m_screen_width * m_screen_height + 10000, L"X=%3.2f, Y=%3.2f, Z=%3.2f, T=%3.2f, A=%3.2f FPS=%3.2f ", m_playerX, m_playerY, m_playerZ, m_player_theta, m_player_azumith, 1.0f / dt);
	
	for (int nx = 0; nx < m_map_width; nx++)
		for (int ny = 0; ny < m_map_height; ny++)
		{
			m_screen[(ny + 1) * m_screen_width + nx] = (wchar_t) (L'0' + m_heightmap[ny * m_map_width + nx]);
		}
	m_screen[((int) m_playerX + 1) * m_screen_width + (int) m_playerY] = 'P';

	m_screen[m_screen_width * m_screen_height - 1] = '\0';
	WriteConsoleOutputCharacter(
		m_buffer,
		m_screen,
		m_screen_width * m_screen_height,
		{ 0, 0 },
		&bytes_written
	);
}

void ConsoleEngine::createWindow(short width, short height)
{
	m_screen_width = width;
	m_screen_height = height;

	COORD screen_coords = { width, height };
	if (!SetConsoleScreenBufferSize(m_buffer, screen_coords))
		throw std::runtime_error("Unable to set Console Screen Buffer size.");

	if (!SetConsoleActiveScreenBuffer(m_buffer))
		throw std::runtime_error("Unable to set Console Screen Buffer as Active.");

	SMALL_RECT screen_rect = { 0, 0, width - 1, height - 1 };
	if (!SetConsoleWindowInfo(m_buffer, TRUE, &screen_rect))
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

