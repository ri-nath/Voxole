#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <chrono>
using namespace std;

#include <stdio.h>
#include <Windows.h>

#include "ConsoleGame.h"
#include "MappedGame.h"

enum class HitType { Floor, Wall, Void, None };

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

class Voxole : public MappedGame
{
private:
	float m_player_height = 2.0f;
	float m_player_theta = 0.0f;
	float m_player_azumith = 0.0f;
	
	const float m_player_max_speed = 5.0f;
	const float m_player_max_angular_speed = 3.5f;

	const float m_clickable_dist = 6.0f;

	const float m_hfov = 3.14159f / 3.0f;
	const float m_vfov = 3.14159f / 3.0f;
	const float m_render_dist = 12.0f;

public:
	Voxole(short screen_width, short screen_length, int map_width, int map_length, unsigned char initial_height)
		: MappedGame(screen_width, screen_length, map_width, map_length, initial_height)
	{
	}

	virtual void onTick(float dt) {
		const float step_size = 0.1f;

		float c_dist_to_wall = 0.0f;

		const float c_unit_i = sinf(m_player_theta);
		const float c_unit_j = cosf(m_player_theta);
		const float c_unit_k = sinf(m_player_azumith);

		int selectedX = NULL;
		int selectedY = NULL;
		bool selected = false;

		while (!selected && c_dist_to_wall < m_clickable_dist)
		{
			c_dist_to_wall += step_size;
			float nTestX = m_playerX + c_unit_i * c_dist_to_wall;
			float nTestY = m_playerY + c_unit_j * c_dist_to_wall;
			float nTestZ = m_playerZ + m_player_height + c_unit_k * c_dist_to_wall;

			int curr_cell_height = MappedGame::getHeight((int) nTestX, (int) nTestY);

			if (nTestZ <= curr_cell_height)
			{
				selectedX = (int)nTestX;
				selectedY = (int)nTestY;
				selected = true;
			}
		}

		if (GetAsyncKeyState((unsigned short)'B') & 0x8000) {
			if (selected) {
				MappedGame::setHeight(selectedX, selectedY, MappedGame::getHeight(selectedX, selectedY) + 1);
			}
		}

		if (GetAsyncKeyState((unsigned short)'V') & 0x8000) {
			if (selected) {
				MappedGame::setHeight(selectedX, selectedY, MappedGame::getHeight(selectedX, selectedY) - 1);
			}
		}

		const int curr_height = getHeight(m_playerX, m_playerY);

		if (GetAsyncKeyState((unsigned short)'Z') & 0x8000) m_player_azumith += m_player_max_angular_speed * 0.75f * dt;
		if (GetAsyncKeyState((unsigned short)'C') & 0x8000) m_player_azumith -= m_player_max_angular_speed * 0.75f * dt;

		if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
			m_playerZ -= (m_player_max_speed) * dt;
			if (curr_height > m_playerZ)
			{
				m_playerZ += (m_player_max_speed) * dt;
			}
		}

		if (GetAsyncKeyState(VK_SPACE) & 0x8000) m_playerZ += (m_player_max_speed) * dt;

		if (GetAsyncKeyState((unsigned short)'W') & 0x8000)
		{
			m_playerX += sinf(m_player_theta) * m_player_max_speed * dt;
			m_playerY += cosf(m_player_theta) * m_player_max_speed * dt;
			if (curr_height > m_playerZ)
			{
				m_playerX -= sinf(m_player_theta) * m_player_max_speed * dt;
				m_playerY -= cosf(m_player_theta) * m_player_max_speed * dt;
			}
		}

		if (GetAsyncKeyState((unsigned short)'S') & 0x8000)
		{
			m_playerX -= sinf(m_player_theta) * m_player_max_speed * dt;
			m_playerY -= cosf(m_player_theta) * m_player_max_speed * dt;
			if (curr_height > m_playerZ)
			{
				m_playerX += sinf(m_player_theta) * m_player_max_speed * dt;
				m_playerY += cosf(m_player_theta) * m_player_max_speed * dt;
			}
		}

		if (GetAsyncKeyState((unsigned short)'A') & 0x8000) m_player_theta -= m_player_max_angular_speed * dt;
		if (GetAsyncKeyState((unsigned short)'D') & 0x8000) m_player_theta += m_player_max_angular_speed * dt;

		for (int x = 0; x < m_screen_width; x++)
		{
			for (int y = 0; y < m_screen_height; y++) {
				const float ray_angle = (m_player_theta - m_hfov / 2.0f) + ((float)x / (float)m_screen_width) * m_hfov;
				const float ray_azumith = (m_player_azumith - m_vfov / 2.0f) + (((float)m_screen_height - y) / (float)m_screen_height) * m_vfov;
				
				float dist_to_wall = 0.0f; 
				
				HitType hit = HitType::None;

				bool boundary = false;
				bool selected_boundary = false;

				const float unit_i = sinf(ray_angle);
				const float unit_j = cosf(ray_angle);
				const float unit_k = sinf(ray_azumith);

				while (hit == HitType::None && dist_to_wall < m_render_dist)
				{
					dist_to_wall += step_size;
					float nTestX = m_playerX + unit_i * dist_to_wall;
					float nTestY = m_playerY + unit_j * dist_to_wall;
					float nTestZ = m_playerZ + m_player_height + unit_k * dist_to_wall;

					if (!MappedGame::isWithinMap(nTestX, nTestY) || nTestZ < -1 || nTestZ > 9)
					{
						hit = HitType::Void;		
						dist_to_wall = m_render_dist;
					}
					else
					{
						int curr_cell_height = MappedGame::getHeight((int) nTestX, (int) nTestY);

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

							selected_boundary = selected && ((int)nTestX == selectedX && (int)nTestY == selectedY);

							if (inBounds(bound, dz))
							{
								hit = HitType::Floor;
							}
							else 
							{
								hit = HitType::Wall;
							}
						}

					}
				}

				short symbol = PIXEL::NONE;
				short col = COLOR::BLACK;

				dist_to_wall += dist(x, y, m_screen_width / 2.0f, m_screen_height / 2.0f) / 50.0f;

				if (hit == HitType::Wall) {
					if (boundary) {
						if (selected_boundary) {
							symbol = PIXEL::FULL;
							col = COLOR::WHITE;
						}
						else 
						{
							if (dist_to_wall <= m_render_dist / 4.0f)			symbol = PIXEL::FULL;
							else if (dist_to_wall < m_render_dist / 3.0f)		symbol = PIXEL::THREE_QUARTERS;
							else if (dist_to_wall < m_render_dist / 2.0f)		symbol = PIXEL::HALF;
							else if (dist_to_wall < m_render_dist)				symbol = PIXEL::QUARTER;

							col = COLOR::GREY;
						}
					}
				}
				else if (hit == HitType::Floor)
				{
					if (selected_boundary) {
						symbol = PIXEL::FULL;
						col = COLOR::WHITE;
					}
					else
					{
						if (dist_to_wall <= m_render_dist / 4.0f)			symbol = PIXEL::FULL;
						else if (dist_to_wall < m_render_dist / 3.0f)		symbol = PIXEL::THREE_QUARTERS;
						else if (dist_to_wall < m_render_dist / 2.0f)		symbol = PIXEL::HALF;
						else if (dist_to_wall < m_render_dist)				symbol = PIXEL::QUARTER;

						col = COLOR::BLUE;
					}
				}

				drawPixel(x, y, symbol, col);
			}
		}

		MappedGame::onTick(dt);
	}
};


int main()
{
	Voxole game(120, 80, 16, 16, 2);
	game.createWindow(8, 8);
	game.start();
	return 0;
}