# ifndef ConsoleGame_HEADER
# define ConsoleGame_HEADER

# include <vector>
# include <wtypes.h>

class COLOR
{
public:
	static const short 
		BLACK = 0x0000,
		DARK_BLUE = 0x0001,
		DARK_GREEN = 0x0002,
		DARK_CYAN = 0x0003,
		DARK_RED = 0x0004,
		DARK_MAGENTA = 0x0005,
		DARK_YELLOW = 0x0006,
		DARK_GREY = 0x0007,
		GREY = 0x0008,
		BLUE = 0x0009,
		GREEN = 0x000A,
		CYAN = 0x000B,
		RED = 0x000C,
		MAGENTA = 0x000D,
		YELLOW = 0x000E,
		WHITE = 0x000F;
};

class PIXEL
{
public:
	static const short
		FULL = 0x2588,
		THREE_QUARTERS = 0x2593,
		HALF = 0x2592,
		QUARTER = 0x2591,
		NONE = ' ';
};

class ConsoleGame
{
private:
	void updateFrame(float dt);

protected:
	ConsoleGame(short screen_width, short screen_height);

	void drawPixel(short x, short y, short c = 0x2588, short col = 0x00F);

	HANDLE m_buffer;
	CHAR_INFO *m_screen;
	SMALL_RECT m_rect_window;

	const short m_screen_width;
	const short m_screen_height;
public:
	virtual void onTick(float dt) = 0;
	void createWindow(int font_width, int font_height);
	void start();
};

# endif