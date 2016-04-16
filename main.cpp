#include <iostream>
#include <cstdlib>
#include <thread>
#include <SFML/Network.hpp>
#include <TGUI/TGUI.hpp>
#include <windows.h>
#include "FrameMaker.h"

using namespace sf;
using namespace std;
using namespace tgui;


int IMG_SIZE=200;
sf::RenderWindow window{ { 800, 600 }, "ScreenViewer" };

inline int GetFilePointer(HANDLE FileHandle) {
	return SetFilePointer(FileHandle, 0, 0, FILE_CURRENT);
}

bool SaveBMPFile(char *filename, HBITMAP bitmap, HDC bitmapDC, int width, int height) {
	bool Success = 0;
	HDC SurfDC = NULL;
	HBITMAP OffscrBmp = NULL;
	HDC OffscrDC = NULL;
	LPBITMAPINFO lpbi = NULL;
	LPVOID lpvBits = NULL;
	HANDLE BmpFile = INVALID_HANDLE_VALUE;
	BITMAPFILEHEADER bmfh;
	if ((OffscrBmp = CreateCompatibleBitmap(bitmapDC, width, height)) == NULL)
		return 0;
	if ((OffscrDC = CreateCompatibleDC(bitmapDC)) == NULL)
		return 0;
	HBITMAP OldBmp = (HBITMAP)SelectObject(OffscrDC, OffscrBmp);
	BitBlt(OffscrDC, 0, 0, width, height, bitmapDC, 0, 0, SRCCOPY);
	if ((lpbi = (LPBITMAPINFO)(new char[sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD)])) == NULL)
		return 0;
	ZeroMemory(&lpbi->bmiHeader, sizeof(BITMAPINFOHEADER));
	lpbi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	SelectObject(OffscrDC, OldBmp);
	if (!GetDIBits(OffscrDC, OffscrBmp, 0, height, NULL, lpbi, DIB_RGB_COLORS))
		return 0;
	if ((lpvBits = new char[lpbi->bmiHeader.biSizeImage]) == NULL)
		return 0;
	if (!GetDIBits(OffscrDC, OffscrBmp, 0, height, lpvBits, lpbi, DIB_RGB_COLORS))
		return 0;
	if ((BmpFile = CreateFile(filename,
		GENERIC_WRITE,
		0, NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL)) == INVALID_HANDLE_VALUE)
		return 0;
	DWORD Written;
	bmfh.bfType = 19778;
	bmfh.bfReserved1 = bmfh.bfReserved2 = 0;
	if (!WriteFile(BmpFile, &bmfh, sizeof(bmfh), &Written, NULL))
		return 0;
	if (Written < sizeof(bmfh))
		return 0;
	if (!WriteFile(BmpFile, &lpbi->bmiHeader, sizeof(BITMAPINFOHEADER), &Written, NULL))
		return 0;
	if (Written < sizeof(BITMAPINFOHEADER))
		return 0;
	int PalEntries;
	if (lpbi->bmiHeader.biCompression == BI_BITFIELDS)
		PalEntries = 3;
	else PalEntries = (lpbi->bmiHeader.biBitCount <= 8) ?
		(int)(1 << lpbi->bmiHeader.biBitCount) : 0;
	if (lpbi->bmiHeader.biClrUsed)
		PalEntries = lpbi->bmiHeader.biClrUsed;
	if (PalEntries) {
		if (!WriteFile(BmpFile, &lpbi->bmiColors, PalEntries * sizeof(RGBQUAD), &Written, NULL))
			return 0;
		if (Written < PalEntries * sizeof(RGBQUAD))
			return 0;
	}
	bmfh.bfOffBits = GetFilePointer(BmpFile);
	if (!WriteFile(BmpFile, lpvBits, lpbi->bmiHeader.biSizeImage, &Written, NULL))
		return 0;
	if (Written < lpbi->bmiHeader.biSizeImage)
		return 0;
	bmfh.bfSize = GetFilePointer(BmpFile);
	SetFilePointer(BmpFile, 0, 0, FILE_BEGIN);
	if (!WriteFile(BmpFile, &bmfh, sizeof(bmfh), &Written, NULL))
		return 0;
	if (Written < sizeof(bmfh))
		return 0;
	CloseHandle(BmpFile);
	return 1;
}
bool ScreenCapture(int x, int y, int width, int height, char *filename) {
	HDC hDc = CreateCompatibleDC(0);
	HBITMAP hBmp = CreateCompatibleBitmap(GetDC(0), width, height);
	SelectObject(hDc, hBmp);
	BitBlt(hDc, 0, 0, width, height, GetDC(0), x, y, SRCCOPY);
	bool ret = SaveBMPFile(filename, hBmp, hDc, width, height);
	DeleteObject(hBmp);
	return ret;
}


int getScreenW() {
	return GetSystemMetrics(SM_CXVIRTUALSCREEN) - GetSystemMetrics(SM_XVIRTUALSCREEN);
}

int getScreenH() {
	return GetSystemMetrics(SM_CYVIRTUALSCREEN) - GetSystemMetrics(SM_YVIRTUALSCREEN);
}


void runUdpServer(unsigned short port, IpAddress ip){
	UdpSocket socket;

	const char out[] = "hello";

	while (1) {
		socket.send(out, sizeof(out), ip, port);
	}
}

void runUdpClient(unsigned short port, IpAddress ip)
{
	UdpSocket socket;
	if (socket.bind(port) != sf::Socket::Done)
	{
	}
	char in[128];
	std::size_t received;


	while (window.isOpen()) {
		socket.receive(in, sizeof(in), received, ip, port);
		std::cout << "Message received from " << ip << ": \"" << in << "\"" << std::endl;
	}
}

void clientThread(unsigned short port, sf::IpAddress ip) {
	std::thread t(runUdpClient, port, ip);
	t.detach();
	return;
}

void serverThread(unsigned short port, sf::IpAddress ip) {
	std::thread t(runUdpServer, port, ip);
	t.detach();
	return;
}

void updateImg(Sprite &sprite, sf::Texture &texture) {
	ScreenCapture(0, 0, getScreenW(), getScreenH(), "file.png");
	texture.loadFromFile("file.png");
	sprite.setTexture(texture);
}




int main()
{
	Gui gui{ window };
	auto theme = std::make_shared<tgui::Theme>("TGUI-0.7/widgets/Black.txt");

	Button::Ptr button1 = theme->load("Button");
	Button::Ptr button2 = theme->load("Button");

	button1->setPosition(280, 500);
	button1->setText("Share");
	button1->setSize(100, 60);
	button1->connect("pressed", serverThread, 5000, "192.168.0.255");

	button2->setPosition(410, 500);
	button2->setText("Connect");
	button2->setSize(100, 60);
	button2->connect("pressed", clientThread, 5000, "192.168.0.255");

	auto picture = make_shared<Picture>("bg.png");
	picture->setSize(bindMax(800, 800),bindMax(600, 600));
	gui.add(picture);

	gui.add(button1);
	gui.add(button2);

	sf::Texture texture;
	Sprite frame;
	FrameMaker fm;
	
	frame.setPosition(60, 60);
	frame.setScale(0.5, 0.5);

	while (window.isOpen())
	{
		sf::Event event;

		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();
			gui.handleEvent(event); // Pass the event to the widgets
		}
		
		fm.ScreenCapture(0, 0, "file.png");
		texture.loadFromFile("file.png");
		frame.setTexture(texture);


		window.clear();
		gui.draw(); // Draw all widgets
		window.draw(frame);
		window.display();
	}

}
