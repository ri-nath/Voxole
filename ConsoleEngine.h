class ConsoleEngine
{
private:
	void updateFrame(float dt);

protected:
	ConsoleEngine();

	HANDLE buffer;
	wchar_t* screen;

	wstring m_map;

	DWORD bytes_written;

	short m_screen_width = 120;
	short m_screen_height = 40;

	int m_map_width = 16;
	int m_map_height = 16;

	float m_playerX = 14.0f;
	float m_playerY = 7.0f;
	float m_playerZ = 5.0f;
	float m_player_theta = 0.0f;
	float m_player_azumith = 0.0f;	
	float m_player_max_speed = 5.0f;

	float m_hfov = 3.14159f / 4.0f;
	float m_vfov = 3.14159f / 4.0f; // Figure this out
	float m_render_dist = 16.0f;

public:
	virtual void onTick(float dt) = 0;
	void createWindow(short width, short height);
	void start();
};