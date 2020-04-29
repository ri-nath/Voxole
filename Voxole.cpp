/*
	OneLoneCoder.com - Command Line First Person Shooter (FPS) Engine
	"Why were games not done like this is 1990?" - @Javidx9

	License
	~~~~~~~
	Copyright (C) 2018  Javidx9
	This program comes with ABSOLUTELY NO WARRANTY.
	This is free software, and you are welcome to redistribute it
	under certain conditions; See license for details.
	Original works located at:
	https://www.github.com/onelonecoder
	https://www.onelonecoder.com
	https://www.youtube.com/javidx9

	GNU GPLv3
	https://github.com/OneLoneCoder/videos/blob/master/LICENSE

	From Javidx9 :)
	~~~~~~~~~~~~~~~
	Hello! Ultimately I don't care what you use this for. It's intended to be
	educational, and perhaps to the oddly minded - a little bit of fun.
	Please hack this, change it and use it in any way you see fit. You acknowledge
	that I am not responsible for anything bad that happens as a result of
	your actions. However this code is protected by GNU GPLv3, see the license in the
	github repo. This means you must attribute me if you use it. You can view this
	license here: https://github.com/OneLoneCoder/videos/blob/master/LICENSE
	Cheers!

	Background
	~~~~~~~~~~
	Whilst waiting for TheMexicanRunner to start the finale of his NesMania project,
	his Twitch stream had a counter counting down for a couple of hours until it started.
	With some time on my hands, I thought it might be fun to see what the graphical
	capabilities of the console are. Turns out, not very much, but hey, it's nice to think
	Wolfenstein could have existed a few years earlier, and in just ~200 lines of code.

	IMPORTANT!!!!
	~~~~~~~~~~~~~
	READ ME BEFORE RUNNING!!! This program expects the console dimensions to be set to
	120 Columns by 40 Rows. I recommend a small font "Consolas" at size 16. You can do this
	by running the program, and right clicking on the console title bar, and specifying
	the properties. You can also choose to default to them in the future.

	Controls: A = Turn Left, D = Turn Right, W = Walk Forwards, S = Walk Backwards

	Future Modifications
	~~~~~~~~~~~~~~~~~~~~
	1) Shade block segments based on angle from player, i.e. less light reflected off
	walls at side of player. Walls straight on are brightest.
	2) Find an interesting and optimised ray-tracing method. I'm sure one must exist
	to more optimally search the map space
	3) Add bullets!
	4) Add bad guys!

	Author
	~~~~~~
	Twitter: @javidx9
	Blog: www.onelonecoder.com

	Video:
	~~~~~~
	https://youtu.be/xW8skO7MFYw

	Last Updated: 27/02/2017
*/

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
		m_map += L"#########.......";
		m_map += L"#...............";
		m_map += L"#.......########";
		m_map += L"#..............#";
		m_map += L"#......##......#";
		m_map += L"#......##......#";
		m_map += L"#..............#";
		m_map += L"###............#";
		m_map += L"##.............#";
		m_map += L"#......####..###";
		m_map += L"#......#.......#";
		m_map += L"#......#.......#";
		m_map += L"#..............#";
		m_map += L"#......#########";
		m_map += L"#..............#";
		m_map += L"################";

		/*m_intmap = {
			0,1,2,3,4,5,6,7,8,9,
			11,12,1,3


		}*/
	}

public:
	virtual void onTick(float dt) {
		if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
			m_playerZ -= (m_player_max_speed) * dt;

		if (GetAsyncKeyState(VK_SPACE) & 0x8000)
			m_playerZ += (m_player_max_speed) * dt;

		if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
			m_player_theta -= (m_player_max_speed * 0.75f) * dt;

		// Handle CW Rotation
		if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
			m_player_theta += (m_player_max_speed * 0.75f) * dt;

		// Handle Forwards movement & collision
		if (GetAsyncKeyState((unsigned short)'W') & 0x8000)
		{
			m_playerX += sinf(m_player_theta) * m_player_max_speed * dt;;
			m_playerY += cosf(m_player_theta) * m_player_max_speed * dt;;
			if (m_map.c_str()[(int)m_playerX * m_map_width + (int)m_playerY] == '#')
			{
				m_playerX -= sinf(m_player_theta) * m_player_max_speed * dt;;
				m_playerY -= cosf(m_player_theta) * m_player_max_speed * dt;;
			}
		}

		// Handle backwards movement & collision
		if (GetAsyncKeyState((unsigned short)'S') & 0x8000)
		{
			m_playerX -= sinf(m_player_theta) * m_player_max_speed * dt;;
			m_playerY -= cosf(m_player_theta) * m_player_max_speed * dt;;
			if (m_map.c_str()[(int)m_playerX * m_map_width + (int)m_playerY] == '#')
			{
				m_playerX += sinf(m_player_theta) * m_player_max_speed * dt;;
				m_playerY += cosf(m_player_theta) * m_player_max_speed * dt;;
			}
		}

		for (int x = 0; x < m_screen_width; x++)
		{
			for (int y = 0; y < m_screen_height; y++) {
				// For each column, calculate the projected ray angle into world space
				float ray_angle = (m_player_theta - m_hfov / 2.0f) + ((float)x / (float)m_screen_width) * m_hfov;
				float ray_azumith = (m_player_azumith - m_vfov / 2.0f) + (((float)m_screen_height - y) / (float)m_screen_height) * m_vfov;

				// Find distance to wall
				float step_size = 0.1f;		  // Increment size for ray casting, decrease to increase										
				float dist_to_wall = 0.0f; //                                      resolution

				HitType hit = None;		// Set when ray hits boundary between two wall blocks

				float unit_i = sinf(ray_angle); // Unit vector for ray in player space
				float unit_j = cosf(ray_angle);
				float unit_k = sinf(ray_azumith);

				// Incrementally cast ray from player, along ray angle, testing for 
				// intersection with a block
				while (hit == None && dist_to_wall < m_render_dist)
				{
					dist_to_wall += step_size;
					int nTestX = (int)(m_playerX + unit_i * dist_to_wall);
					int nTestY = (int)(m_playerY + unit_j * dist_to_wall);
					int nTestZ = (int)(m_playerZ + unit_k * dist_to_wall);

					// Test if ray is out of bounds
					if (nTestX < 0 || nTestX >= m_map_width || nTestY < 0 || nTestY >= m_map_height)
					{
						hit = Wall;			// Just set distance to maximum depth
						dist_to_wall = m_render_dist;
					}
					else
					{
						// Ray is inbounds so test to see if the ray cell is a wall block
						if (m_map.c_str()[nTestX * m_map_width + nTestY] == '#' || nTestZ <= 0)
						{
							// Ray has hit wall
							if (nTestZ <= 0) {
								hit = Floor;
							}
							else {
								hit = Wall;

							}
								// To highlight tile boundaries, cast a ray from each corner
								// of the tile, to the player. The more coincident this ray
								// is to the rendering ray, the closer we are to a tile 
								// boundary, which we'll shade to add detail to the walls

						};
					}
				}

				// Calculate distance to ceiling and floor
				// Shader walls based on distance
				short nShade = ' ';
				if (dist_to_wall <= m_render_dist / 4.0f)			nShade = 0x2588;	// Very close	
				else if (dist_to_wall < m_render_dist / 3.0f)		nShade = 0x2593;
				else if (dist_to_wall < m_render_dist / 2.0f)		nShade = 0x2592;
				else if (dist_to_wall < m_render_dist)				nShade = 0x2591;
				else				
					nShade = ' ';		// Too far away

				// Each Row
				if (hit == Wall) {
					m_screen[y * m_screen_width + x] = nShade;
				}
				else if (hit == Floor) // Floor
				{
					// Shade floor based on distance
					float b = 1.0f - (((float)y - m_screen_height / 2.0f) / ((float)m_screen_height / 2.0f));
					if (b < 0.25)		nShade = '#';
					else if (b < 0.5)	nShade = 'x';
					else if (b < 0.75)	nShade = '.';
					else if (b < 0.9)	nShade = '-';
					else				nShade = ' ';
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