#pragma once
#include <SFML/Network.hpp>
#include <TGUI/TGUI.hpp>
#include <windows.h>

using namespace sf;
using namespace std;

class FrameMaker
{
public:
	FrameMaker();
	~FrameMaker();
	//Sprite getSprite() { return sprite; }
	//void setScale(int X, int Y) { sprite.setScale(X, Y); }
	//void setPos(int X, int Y) { sprite.setPosition(X, Y); }
	int getScreenW() { return GetSystemMetrics(SM_CXVIRTUALSCREEN) - GetSystemMetrics(SM_XVIRTUALSCREEN); }
	int getScreenH() { return GetSystemMetrics(SM_CYVIRTUALSCREEN) - GetSystemMetrics(SM_YVIRTUALSCREEN); }
	bool SaveBMPFile(char *filename, HBITMAP bitmap, HDC bitmapDC, int width, int height);
	void ScreenCapture(int x, int y, char *filename);
private:
	//sf::Texture texture;
	Sprite sprite;
};

