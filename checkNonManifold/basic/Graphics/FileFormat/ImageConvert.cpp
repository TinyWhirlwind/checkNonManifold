#include "stdafx.h"


#include "ImageConvert.h"

void CImageConvert::ImageNearestScale( BYTE *destData,int destW, int destH, BYTE *srcData, int srcW, int srcH,BOOL bRGB)
{
	int bytesPixel = 1; 
	if( bRGB )//RGB
		bytesPixel = 3;

	double rx = srcW/(double)destW;
	double ry = srcH/(double)destH;
	BYTE *srcLine = srcData;
	int i,j;
	for ( j=0; j<destH; j++ )
	{
		for ( i=0; i<destW; i++ )
		{
			int xoff = ( (int)( rx*i ) )* bytesPixel;
			BYTE *p = srcLine + xoff;
			*destData++ = *p++;
			if( bRGB )
			{
				*destData++ = *p++;
				*destData++ = *p++;
			}
		}
		srcLine = srcData + ( (int)( ry*j ) )* srcW*bytesPixel;
	}
}
void CImageConvert::ImageCopyClip( BYTE *destData,int destW, int destH, 
								BYTE *srcData, int srcW, int srcH,
								int posX, int posY,BOOL bRGB )
{
	int bytesPixel = 1; 
	if( bRGB )//RGB
		bytesPixel = 3;
	int destSize = bytesPixel*destW*destH;
	memset(destData,0,destSize);

	// 每行要拷贝像素
	int cpLinePixel = destW<(srcW-posX) ? destW : (srcW-posX);
	int cpLineCount = destH<(srcH-posY) ? destH : (srcH-posY);
	if( cpLinePixel <= 0 || cpLineCount<=0 || 
		posX<0 || posY<0 || srcW<= posX || srcH<=posY )
		return;

	BYTE *destLine = destData;
	BYTE *srcLine = srcData + (srcW*posY + posX)*bytesPixel;
	int j;
	for ( j=0; j<cpLineCount; j++ )
	{
		memcpy(destLine,srcLine,cpLinePixel*bytesPixel);
		destLine += destW*bytesPixel;
		srcLine  += srcW*bytesPixel;
	}
}
void CImageConvert::ImageFlip(BYTE *dataRGB,int w, int h)
{
	int lineByte = w*3;
	BYTE *lineData = new BYTE[lineByte];
	BYTE *p1;
	BYTE *p2;
	for (int j=0; j<h/2; j++)
	{
		//第j行与第h-j行互换
		p1 = dataRGB+j*lineByte;
		p2 = dataRGB+(h-1-j)*lineByte;
		memcpy(lineData,p1,lineByte);
		memcpy(p1,p2,lineByte);
		memcpy(p2,lineData,lineByte);
	}
}

void CImageConvert::RedFullImage(BYTE *dataRGB,int w, int h)
{
	// 过量的地方显示为红色；
	BYTE * data = dataRGB;
	BYTE r,g,b;
	for (int j=0; j<h; j++)
	{
		for (int i=0; i<w; i++)
		{
			r = *data;
			g = *(data+1);
			b = *(data+2);
			if( r==255 && g==255 && b==255)
			{
				*data = 0;
				*(data+1) =0;
			}
			data=data+3;
		}
	}
}

BOOL CImageConvert::SaveBMP( char *szName, BYTE* rgb24Data, int w, int h)
{
	BITMAPINFO *bmpInfo;		//BITMAPINFO 结构指针，显示图像时使用
	//BYTE *m_imageBuffer;		//Bayer转换后缓冲区
	char chBmpBuf[2048];		//BIMTAPINFO 存储缓冲区，m_pBmpInfo即指向此缓冲区

	//	m_pBmpInfo即指向m_chBmpBuf缓冲区，用户可以自己分配BTIMAPINFO缓冲区	
	bmpInfo								= (BITMAPINFO *)chBmpBuf;
	//	初始化BITMAPINFO 结构，此结构在保存bmp文件、显示采集图像时使用
	bmpInfo->bmiHeader.biSize				= sizeof(BITMAPINFOHEADER);
	//	图像宽度，一般为输出窗口宽度
	bmpInfo->bmiHeader.biWidth			= w;
	//	图像宽度，一般为输出窗口高度
	bmpInfo->bmiHeader.biHeight			= h;

	/*
	*	以下设置一般相同，
	*	对于低于8位的位图，还应设置相应的位图调色板
	*/
	bmpInfo->bmiHeader.biPlanes			= 1;
	bmpInfo->bmiHeader.biBitCount		= 24;
	bmpInfo->bmiHeader.biCompression	= BI_RGB;
	bmpInfo->bmiHeader.biSizeImage		= 0;
	bmpInfo->bmiHeader.biXPelsPerMeter	= 0;
	bmpInfo->bmiHeader.biYPelsPerMeter	= 0;
	bmpInfo->bmiHeader.biClrUsed		= 0;
	bmpInfo->bmiHeader.biClrImportant	= 0;


	BITMAPFILEHEADER bfh	= {0};
	DWORD dwImageSize		= 0;
	DWORD dwBytesRead		= 0;
	BOOL bRVal				= TRUE;

	dwImageSize		=  w * h * 3;		//计算图像大小，由视频输出窗口和视频格式确定

	bfh.bfType		= (WORD)'M' << 8 | 'B';			//定义文件类型
	bfh.bfOffBits	= sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);	//定义文件头大小

	bfh.bfSize		= bfh.bfOffBits + dwImageSize;	//文件大小

	HANDLE hFile = ::CreateFile((LPCSTR)szName,
		GENERIC_WRITE ,
		0,
		NULL,
		CREATE_ALWAYS,	FILE_ATTRIBUTE_NORMAL,
		NULL
		);
	if (hFile == INVALID_HANDLE_VALUE) 
	{
		bRVal = FALSE;
	}
	else
	{
		::WriteFile(hFile, &bfh, sizeof(BITMAPFILEHEADER), &dwBytesRead, NULL );
		::WriteFile(hFile, bmpInfo, sizeof(BITMAPINFOHEADER), &dwBytesRead, NULL );
		::WriteFile(hFile, rgb24Data, dwImageSize, &dwBytesRead, NULL );

		CloseHandle(hFile);
	}
	return bRVal;
}