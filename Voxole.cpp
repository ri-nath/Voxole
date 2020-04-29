#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <chrono>
using namespace std;

#include <stdio.h>
#include <Windows.h>

#include "ConsoleEngine.h"

enum HitType { Floor, Wall, None };

class Voxole : public ConsoleEngine
{
public:
	Voxole()
	{
		m_heightmap = {
			1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
			1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
			1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
			1,0,0,0,0,1,2,3,4,5,0,0,0,0,0,1,
			1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
			1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
			1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
			1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
			1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
			1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
			1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
			1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
			1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
			1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
			1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
			1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		};
	}

public:
	virtual void onTick(float dt) {
		if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
			m_playerZ -= (m_player_max_speed)*dt;
		}

		if (GetAsyncKeyState(VK_SPACE) & 0x8000)
			m_playerZ += (m_player_max_speed) * dt;

		if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
			m_player_theta -= (m_player_max_speed * 0.15f) * dt;

		if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
			m_player_theta += (m_player_max_speed * 0.15f) * dt;

		if (GetAsyncKeyState((unsigned short)'W') & 0x8000)
		{
			m_playerX += sinf(m_player_theta) * m_player_max_speed * dt;
			m_playerY += cosf(m_player_theta) * m_player_max_speed * dt;
			if (m_heightmap[(int)m_playerX * m_map_width + (int)m_playerY] > m_playerZ)
			{
				m_playerX -= sinf(m_player_theta) * m_player_max_speed * dt;
				m_playerY -= cosf(m_player_theta) * m_player_max_speed * dt;
			}
		}

		if (GetAsyncKeyState((unsigned short)'S') & 0x8000)
		{
			m_playerX -= sinf(m_player_theta) * m_player_max_speed * dt;
			m_playerY -= cosf(m_player_theta) * m_player_max_speed * dt;
			if (m_heightmap[(int)m_playerX * m_map_width + (int)m_playerY] == 1)
			{
				m_playerX += sinf(m_player_theta) * m_player_max_speed * dt;
				m_playerY += cosf(m_player_theta) * m_player_max_speed * dt;
			}
		}

		for (int x = 0; x < m_screen_width; x++)
		{
			for (int y = 0; y < m_screen_height; y++) {
				float ray_angle = (m_player_theta - m_hfov / 2.0f) + ((float)x / (float)m_screen_width) * m_hfov;
				float ray_azumith = (m_player_azumith - m_vfov / 2.0f) + (((float)m_screen_height - y) / (float)m_screen_height) * m_vfov;

				float step_size = 0.1f;		  								
				float dist_to_wall = 0.0f; 

				HitType hit = None;

				float unit_i = sinf(ray_angle);
				float unit_j = cosf(ray_angle);
				float unit_k = sinf(ray_azumith);

				while (hit == None && dist_to_wall < m_render_dist)
				{
					dist_to_wall += step_size;
					int nTestX = (int)(m_playerX + unit_i * dist_to_wall);
					int nTestY = (int)(m_playerY + unit_j * dist_to_wall);
					float nTestZ = m_playerZ + m_player_height + unit_k * dist_to_wall;

					if (nTestX < 0 || nTestX >= m_map_width || nTestY < 0 || nTestY >= m_map_height)
					{
						hit = Wall;		
						dist_to_wall = m_render_dist;
					}
					else
					{
						int curr_cell_height = m_heightmap[nTestX * m_map_width + nTestY];


						if (nTestZ <= curr_cell_height) 
						{
							float bound = 0000.1f;
							float dz = curr_cell_height - nTestZ;
							if (-1.0f * bound < dz && bound > dz)
							{
								hit = Floor;
							}
							else 
							{
								hit = Wall;
							}
						}

					}
				}


				short nShade = ' ';

				if (hit == Wall) {
					if (dist_to_wall <= m_render_dist / 4.0f)			nShade = 0x2588;
					else if (dist_to_wall < m_render_dist / 3.0f)		nShade = 0x2593;
					else if (dist_to_wall < m_render_dist / 2.0f)		nShade = 0x2592;
					else if (dist_to_wall < m_render_dist)				nShade = 0x2591;

					m_screen[y * m_screen_width + x] = nShade;
				}
				else if (hit == Floor)
				{
					float b = 1.0f - (((float)y - m_screen_height / 2.0f) / ((float)m_screen_height / 2.0f));
					if (b < 0.25)		nShade = '#';
					else if (b < 0.5)	nShade = 'x';
					else if (b < 0.75)	nShade = '.';
					else if (b < 0.9)	nShade = '-';

					m_screen[y * m_screen_width + x] = nShade;
				}
			}
			
		}
	}
};

int main()
{
	Voxole game;
	game.createWindow(120, 40);
	game.start();
	return 0;
}

// That's It!! - Jx9