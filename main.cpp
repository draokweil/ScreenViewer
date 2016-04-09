#include <iostream>
#include <cstdlib>
#include <thread>
#include <SFML/Network.hpp>
#include <TGUI/TGUI.hpp>
#include <windows.h>


int IMG_SIZE=200;


void runUdpServer(unsigned short port, sf::IpAddress ip)
{
	sf::UdpSocket socket;
	const char out[] = "Hello";
	while (1) {
		socket.send(out, sizeof(out), ip, port);
	}
}

void runUdpClient(unsigned short port, sf::IpAddress ip)
{
	sf::UdpSocket socket;
	if (socket.bind(port) != sf::Socket::Done)
	{
	}
	char in[128];
	std::size_t received;
	sf::Packet p();
	unsigned short senderPort;
	while (1) {
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

HBITMAP GetScreenShot(void)
{
	int x1, y1, x2, y2, w, h;

	// get screen dimensions
	x1 = GetSystemMetrics(SM_XVIRTUALSCREEN);
	y1 = GetSystemMetrics(SM_YVIRTUALSCREEN);
	x2 = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	y2 = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	w = x2 - x1;
	h = y2 - y1;

	// copy screen to bitmap
	HDC     hScreen = GetDC(NULL);
	HDC     hDC = CreateCompatibleDC(hScreen);
	HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h);
	HGDIOBJ old_obj = SelectObject(hDC, hBitmap);
	BOOL    bRet = BitBlt(hDC, 0, 0, w, h, hScreen, x1, y1, SRCCOPY);


	// save bitmap to clipboard
	//OpenClipboard(NULL);
	//EmptyClipboard();
	//SetClipboardData(CF_BITMAP, hBitmap);
	//GetClipboardData(CF_BITMAP);
	//CloseClipboard();
	// clean up
	SelectObject(hDC, old_obj);
	DeleteDC(hDC);
	ReleaseDC(NULL, hScreen);
	DeleteObject(hBitmap);

	return hBitmap;
}

int main()
{
	sf::Image frame;
	sf::RenderWindow window{ { 800, 600 }, "ScreenViewer" };
	tgui::Gui gui{ window }; // Create the gui and attach it to the window

	auto theme = std::make_shared<tgui::Theme>("TGUI-0.7/widgets/Black.txt");
	tgui::Button::Ptr button1 = theme->load("Button");
	tgui::Button::Ptr button2 = theme->load("Button");
	button1->setPosition(280, 500);
	button1->setText("Share");
	button1->setSize(100, 60);
	button1->connect("pressed", serverThread, 5000, "192.168.0.255");

	button2->setPosition(410, 500);
	button2->setText("Connect");
	button2->setSize(100, 60);
	button2->connect("pressed", clientThread, 5000, "192.168.0.255");
	auto picture = std::make_shared<tgui::Picture>("bg.png");


	picture->setSize(tgui::bindMax(800, 800), tgui::bindMax(600, 600));
	gui.add(picture);

	gui.add(button1);
	gui.add(button2);
	GetScreenShot();

	while (window.isOpen())
	{
		sf::Event event;

		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			gui.handleEvent(event); // Pass the event to the widgets
		}

		window.clear();
		gui.draw(); // Draw all widgets
		window.display();
	}

}