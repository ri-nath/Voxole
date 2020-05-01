# include "MappedGame.h"
# include "ConsoleGame.h"

MappedGame::MappedGame(short screen_width, short screen_length, int map_width, int map_length, unsigned char initial_height)
	: m_map_width(map_width),
	  m_map_length(map_length),
	  m_heightmap(map_width * map_length, initial_height),
	  m_playerX(map_width / 2.0f), m_playerY(map_length / 2.0f), m_playerZ(initial_height),
	  ConsoleGame(screen_width, screen_length)
{

}

bool MappedGame::isWithinMap(float x, float y)
{
	return x >= 0 && x < m_map_width && y >= 0 && y < m_map_length;
}

unsigned char MappedGame::getHeight(int x, int y)
{
	if (isWithinMap((float) x, (float) y))
	{
		return m_heightmap[y * m_map_width + x];
	}
	else
	{
		return 0;
	}
}

void MappedGame::setHeight(int x, int y, unsigned char height)
{
	if (isWithinMap((float) x, (float) y))
	{
		m_heightmap[y * m_map_width + x] = height;
	}
}

void MappedGame::onTick(float dt)
{
	for (int x = 0; x < m_map_width; x++)
	{
		for (int y = 0; y < m_map_length; y++)
		{
			drawPixel(x, y, L'0' + getHeight(x, y));
		}
	}

	while (m_playerZ > getHeight(m_playerX, m_playerY))
	{
		m_playerZ++;
	}

	drawPixel(m_playerX, m_playerY, 'P', COLOR::GREEN);
}
