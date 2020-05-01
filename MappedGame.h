# include <vector>

# include "ConsoleGame.h"

class MappedGame : public ConsoleGame
{
private:
	std::vector<unsigned char> m_heightmap;
protected:
	MappedGame(short screen_width, short screen_length, int map_width, int map_length, unsigned char initial_height);

	const int m_map_width;
	const int m_map_length;

	float m_playerX;
	float m_playerY;
	float m_playerZ;

	virtual void onTick(float dt);

	bool isWithinMap(float x, float y);
	unsigned char getHeight(int x, int y);
	void setHeight(int x, int y, unsigned char height);
};

