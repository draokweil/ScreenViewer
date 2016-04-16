#include "FrameMaker.h"



FrameMaker::FrameMaker()
{
}


FrameMaker::~FrameMaker()
{
}

inline int GetFilePointer(HANDLE FileHandle) {
	return SetFilePointer(FileHandle, 0, 0, FILE_CURRENT);
}

bool FrameMaker::SaveBMPFile(char *filename, HBITMAP bitmap, HDC bitmapDC, int width, int height) {
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
void FrameMaker::ScreenCapture(int x, int y,char *filename) {
	HDC hDc = CreateCompatibleDC(0);
	HBITMAP hBmp = CreateCompatibleBitmap(GetDC(0), getScreenW() , getScreenH());
	SelectObject(hDc, hBmp);
	BitBlt(hDc, 0, 0, getScreenW(), getScreenH(), GetDC(0), x, y, SRCCOPY);
	bool ret = SaveBMPFile(filename, hBmp, hDc, getScreenW(), getScreenH());
	DeleteObject(hBmp);
	//texture.loadFromFile(filename);
	//sprite.setTexture(texture);
	//return sprite;
}
