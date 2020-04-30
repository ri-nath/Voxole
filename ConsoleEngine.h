# include <vector>

class ConsoleEngine
{
private:
	void updateFrame(float dt);

protected:
	ConsoleEngine(int screen_width, int screen_height);

	void drawPixel(short x, short y, short c = 0x2588, short col = 0x00F);

	HANDLE m_buffer;
	CHAR_INFO *m_screen;
	SMALL_RECT m_rect_window;

	vector<unsigned int> m_heightmap;

	short m_screen_width = 360;
	short m_screen_height = 160;

	int m_map_width = 16;
	int m_map_height = 16;

	float m_playerX = 14.0f;
	float m_playerY = 7.0f;
	float m_playerZ = 0.0f;
	float m_player_height = 2.0f;
	float m_player_theta = 0.0f;
	float m_player_azumith = 0.0f;	
	float m_player_max_speed = 5.0f;

	float m_clickable_dist = 6.0f;

	float m_hfov = 3.14159f / 3.0f;
	float m_vfov = 3.14159f / 4.0f; // Figure this out
	float m_render_dist = 12.0f;

public:
	virtual void onTick(float dt) = 0;
	void createWindow(int font_width, int font_height);
	void start();
};