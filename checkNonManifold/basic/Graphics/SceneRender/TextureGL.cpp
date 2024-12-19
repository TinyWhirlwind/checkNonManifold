//#include"stdafx.h"
#include "TextureGL.h"

namespace sn3DCore
{
	TextureGL::TextureGL()
	{
		m_TextureId = -1;
		m_flag = 0;
		m_Data = NULL;
		m_channel = 3;
	}

	void * TextureGL::GetImageData()
	{
		return (void*)m_Data;
	}

	const void * TextureGL::GetImageData() const
	{
		return (void*)m_Data;
	}

	int TextureGL::GetWidth()
	{
		return m_w;
	}

	const int TextureGL::GetWidth()const
	{
		return m_w;
	}

	const int TextureGL::GetHeight() const
	{
		return m_h;
	}

	int TextureGL::GetHeight()
	{
		return m_h;
	}
	int TextureGL::GetChannel()
	{
		return m_channel;
	}

	void TextureGL::SetImage(int w, int h, int channel, void * data)
	{
		m_channel = channel;
		if (channel == 3)
		{
			m_Data = new unsigned char[w*h * 3];

			for (int i = 0; i < h; i++)
				for (int j = 0; j < w; j++)
				{
					m_Data[i*w * 3 + j * 3] = ((unsigned char*)data)[i*w * 3 + j * 3];
					m_Data[i*w * 3 + j * 3 + 1] = ((unsigned char*)data)[i*w * 3 + j * 3 + 1];
					m_Data[i*w * 3 + j * 3 + 2] = ((unsigned char*)data)[i*w * 3 + j * 3 + 2];
				}

			SetColorImage();
		}
		else if (channel == 1)
		{
			m_Data = new unsigned char[w*h * 3];

			for (int i = 0; i < h; i++)
				for (int j = 0; j < w; j++)
				{
					m_Data[i*w * 3 + j * 3] = ((unsigned char*)data)[i*w + j];
					m_Data[i*w * 3 + j * 3 + 1] = ((unsigned char*)data)[i*w + j];
					m_Data[i*w * 3 + j * 3 + 2] = ((unsigned char*)data)[i*w + j];
				}

			ClearColorImage();
		}
		m_w = w;
		m_h = h;

	}

	void TextureGL::SetChannel(int channel)
	{
		m_channel = channel;
		if (m_channel != 1)this->SetColorImage();

	}

	void TextureGL::BuildTexture()
	{
		glEnable(GL_TEXTURE_2D);
		glGenTextures(1, &m_TextureId);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		glBindTexture(GL_TEXTURE_2D, m_TextureId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, m_channel, m_w, m_h, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, m_Data);
		glDisable(GL_TEXTURE_2D);

		SetRenderTexture();
		SetHasTexture();
	}
}