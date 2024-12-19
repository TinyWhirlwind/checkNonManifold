#ifndef _ImageConvert_H
#define _ImageConvert_H
#include<windows.h>
class CImageConvert
{
public:

	static void ImageNearestScale( BYTE *destData,int destW, int destH, 
		BYTE *srcData, int srcW, int srcH, BOOL bRGB = TRUE);
	static void ImageCopyClip( BYTE *destData,int destW, int destH, 
		BYTE *srcData, int srcW, int srcH, int posX, int posY, BOOL bRGB = TRUE);
	static void ImageFlip(BYTE *dataRGB,int w, int h);
	static void RedFullImage(BYTE *dataRGB,int w, int h);		// 饱和地方用红色表示

	static BOOL SaveBMP( char *name, BYTE* rgb24Data, int w, int h);
	static void Bayer2Rgb( BYTE* bayerData, BYTE* rgb24Data, int w, int h);
	
};

#endif