#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <chrono>
using namespace std;

#include <stdio.h>
#include <Windows.h>

#include "ConsoleEngine.h"

enum HitType { Floor, Wall, Void, None };

bool inBounds(float bound, float val) 
{
	return (-1.0f * bound < val && bound > val);
}

float dist(float x1, float y1, float x2, float y2) 
{
	float res = sqrtf((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
	if (res < 0) 
	{
		return res * -1.0f;
	}
	else 
	{
		return res;
	}
}

class Voxole : public ConsoleEngine
{
public:
	Voxole(short width, short height)
		: ConsoleEngine(width, height)
	{
		m_heightmap = {
			1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
			1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
			1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
			1,0,0,0,0,1,2,3,4,5,6,7,8,0,0,1,
			1,0,0,0,0,0,0,0,0,0,0,0,9,0,0,1,
			1,0,0,0,0,0,0,0,0,0,0,0,8,0,0,1,
			1,0,0,0,0,0,0,0,0,0,0,0,7,0,0,1,
			1,0,0,0,0,0,0,0,0,0,0,0,6,0,0,1,
			1,0,0,0,0,0,0,0,0,0,0,0,5,0,0,1,
			1,0,0,0,0,0,5,0,0,0,0,0,0,0,0,1,
			1,0,0,0,0,4,0,0,6,0,0,0,0,0,0,1,
			1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
			1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
			1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
			1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
			1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		};
	}

public:
	virtual void onTick(float dt) {
		float step_size = 0.1f;

		float c_dist_to_wall = 0.0f;

		float c_unit_i = sinf(m_player_theta);
		float c_unit_j = cosf(m_player_theta);
		float c_unit_k = sinf(m_player_azumith);

		int selectedX = NULL;
		int selectedY = NULL;
		bool selected = false;

		while (!selected && c_dist_to_wall < m_clickable_dist)
		{
			c_dist_to_wall += step_size;
			float nTestX = m_playerX + c_unit_i * c_dist_to_wall;
			float nTestY = m_playerY + c_unit_j * c_dist_to_wall;
			float nTestZ = m_playerZ + m_player_height + c_unit_k * c_dist_to_wall;

			int curr_cell_height = m_heightmap[((int)nTestX) * m_map_width + ((int)nTestY)];

			if (nTestZ <= curr_cell_height)
			{
				selectedX = (int)nTestX;
				selectedY = (int)nTestY;
				selected = true;
			}
		}

		if (GetAsyncKeyState((unsigned short)'B') & 0x8000) {
			if (selected) {
				m_heightmap[selectedX * m_map_width + selectedY] = m_heightmap[selectedX * m_map_width + selectedY] + 1;
			}
		}

		if (GetAsyncKeyState((unsigned short)'V') & 0x8000) {
			if (selected) {
				m_heightmap[selectedX * m_map_width + selectedY] = m_heightmap[selectedX * m_map_width + selectedY] - 1;
			}
		}

		if (GetAsyncKeyState((unsigned short)'Z') & 0x8000)
			m_player_azumith += (m_player_max_speed * 0.30f) * dt;

		if (GetAsyncKeyState((unsigned short)'C') & 0x8000)
			m_player_azumith -= (m_player_max_speed * 0.30f) * dt;

		if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
			m_playerZ -= (m_player_max_speed) * dt;
			if (m_heightmap[(int)m_playerX * m_map_width + (int)m_playerY] > m_playerZ)
			{
				m_playerZ += (m_player_max_speed)  *dt;
			}
		}

		if (GetAsyncKeyState(VK_SPACE) & 0x8000)
			m_playerZ += (m_player_max_speed) * dt;

		if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
			m_player_theta -= (m_player_max_speed * 0.30f) * dt;

		if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
			m_player_theta += (m_player_max_speed * 0.30f) * dt;


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
				
				float dist_to_wall = 0.0f; 
				

				HitType hit = None;

				bool boundary = false;
				bool selected_boundary = false;

				float unit_i = sinf(ray_angle);
				float unit_j = cosf(ray_angle);
				float unit_k = sinf(ray_azumith);

				while (hit == None && dist_to_wall < m_render_dist)
				{
					dist_to_wall += step_size;
					float nTestX = m_playerX + unit_i * dist_to_wall;
					float nTestY = m_playerY + unit_j * dist_to_wall;
					float nTestZ = m_playerZ + m_player_height + unit_k * dist_to_wall;

					if (nTestX < 0 || nTestX >= m_map_width || nTestY < 0 || nTestY >= m_map_height || nTestZ < -1 || nTestZ > 9)
					{
						hit = Void;		
						dist_to_wall = m_render_dist;
					}
					else
					{
						int curr_cell_height = m_heightmap[((int) nTestX) * m_map_width + ((int) nTestY)];

						if (nTestZ <= curr_cell_height) 
						{
							float bound = 0.1f;
							float dz = curr_cell_height - nTestZ;

							float dx = nTestX - (int)nTestX;
							float dy = nTestY - (int)nTestY;
							float d2x = nTestX - (int)(nTestX + 1);
							float d2y = nTestY - (int)(nTestY + 1);
							float d2z = nTestZ - (int)nTestZ;
							float d3z = nTestZ - (int)(nTestZ + 1);
							boundary = (inBounds(bound, dx) && inBounds(bound, dy))
								|| (inBounds(bound, dx) && inBounds(bound, d2y))
								|| (inBounds(bound, d2x) && inBounds(bound, dy))
								|| (inBounds(bound, d2x) && inBounds(bound, d2y))
								|| (inBounds(bound, d2z));

							selected_boundary = ((int)nTestX == selectedX && (int)nTestY == selectedY);

							if (inBounds(bound, dz))
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

				short shade = ' ';
				short col = 0x000F;

				dist_to_wall += dist(x, y, m_screen_width / 2.0f, m_screen_height / 2.0f) / 50.0f;

				if (hit == Wall) {
					if (boundary) {
						if (selected_boundary) {
							shade = 0x2588;
							col = 0x000F;
						}
						else 
						{
							if (dist_to_wall <= m_render_dist / 4.0f)			shade = 0x2588;
							else if (dist_to_wall < m_render_dist / 3.0f)		shade = 0x2593;
							else if (dist_to_wall < m_render_dist / 2.0f)		shade = 0x2592;
							else if (dist_to_wall < m_render_dist)				shade = 0x2591;

							col = 0x000C;
						}
					}
				}
				else if (hit == Floor)
				{
					if (selected_boundary) {
						shade = 0x2588;
						col = 0x000F;
					}
					else 
					{
						if (dist_to_wall <= m_render_dist / 3.0f)			shade = 0x2588;
						else if (dist_to_wall < m_render_dist / 2.0f)		shade = 0x2593;
						else if (dist_to_wall < m_render_dist / 1.5f)		shade = 0x2592;
						else if (dist_to_wall < m_render_dist)				shade = 0x2591;

						col = 0x0001;
					}
				}

				drawPixel(x, y, shade, col);
			}
		}
	}
};


int main()
{
	Voxole game(180, 120);
	game.createWindow(4, 4);
	game.start();
	return 0;
}