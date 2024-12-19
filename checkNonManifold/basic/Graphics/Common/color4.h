#ifndef _COLOR4_
#define _COLOR4_

#include "point3.h"
#include "point4.h"

namespace sn3DCore
{
	template <class T>
	class Color4 : public Point4<T>
	{
	public:
		/*颜色常量*/
		enum ColorConstant {
			Black = 0xff000000,
			Gray = 0xff808080,
			White = 0xffffffff,
			Red = 0xff0000ff,
			Green = 0xff00ff00,
			Blue = 0xffff0000,
			Cyan = 0xffffff00,
			Yellow = 0xff00ffff,
			Magenta = 0xffff00ff,
			LightGray = 0xffc0c0c0,
			LightRed = 0xff8080ff,
			LightGreen = 0xff80ff80,
			LightBlue = 0xffff8080,
			DarkGray = 0xff404040,
			DarkRed = 0xff000040,
			DarkGreen = 0xff004000,
			DarkBlue = 0xff400000
		};
		/*构造器*/
		inline Color4(const T nx, const T ny, const T nz, const T nw) :Point4<T>(nx, ny, nz, nw) {};
		inline Color4() {};
		inline Color4(ColorConstant cc);
		template <class Q>
		inline void Import(const Color4<Q> & b)
		{
			Point4<T>::_v[0] = T(b[0]);
			Point4<T>::_v[1] = T(b[1]);
			Point4<T>::_v[2] = T(b[2]);
			Point4<T>::_v[3] = T(b[3]);
		}


		inline void lerp(const Color4 &c0, const Color4 &c1, const float x);
		inline void lerp(const Color4 &c0, const Color4 &c1, const Color4 &c2, const Point3f &ip);

		/*给定一浮点数和一范围*/
		inline void ColorRamp(const float &minf, const float  &maxf, float v);

		inline void SetRGB(unsigned char r, unsigned char g, unsigned char b)
		{
			Point4<T>::_v[0] = r;
			Point4<T>::_v[1] = g;
			Point4<T>::_v[2] = b;
			Point4<T>::_v[3] = 0;
		}

		void SetHSVColor(float h, float s, float v) {
			float r, g, b;
			if (s == 0.0) {	// 灰度颜色
				r = g = b = v;
				Point4<T>::_v[0] = (unsigned char)(255 * r);
				Point4<T>::_v[1] = (unsigned char)(255 * g);
				Point4<T>::_v[2] = (unsigned char)(255 * b);
				Point4<T>::_v[3] = 255;
				return;
			}
			if (h == 1.0) h = 0.0;

			int i = int(floor(h*6.0));
			float f = float(h*6.0f - floor(h*6.0f));

			float p = v*(1.0f - s);
			float q = v*(1.0f - s*f);
			float t = v*(1.0f - s*(1.0f - f));

			switch (i) {
			case 0: r = v; g = t; b = p; break;
			case 1: r = q; g = v; b = p; break;
			case 2: r = p; g = v; b = t; break;
			case 3: r = p; g = q; b = v; break;
			case 4: r = t; g = p; b = v; break;
			case 5: r = v; g = p; b = q; break;
			}
			Point4<T>::_v[0] = (unsigned char)(255 * r);
			Point4<T>::_v[1] = (unsigned char)(255 * g);
			Point4<T>::_v[2] = (unsigned char)(255 * b);
			Point4<T>::_v[3] = 255;

		}

		inline static Color4 GrayShade(float f)
		{
			return Color4(f, f, f, 1);
		}

		inline void SetGrayShade(float f)
		{
			Import(Color4<float>(f, f, f, 1));
		}


		/*给定一整数,返回排序的颜色，使得各颜色尽量分离 n 最大期待值 v 请求位置*/
		inline static Color4 Scatter(int n, int a, float Sat = .3f, float Val = .9f)
		{
			int b, k, m = n;
			int r = n;

			for (b = 0, k = 1; k < n; k <<= 1)
				if (a << 1 >= m) {
					if (b == 0) r = k;
					b += k;
					a -= (m + 1) >> 1;
					m >>= 1;
				}
				else m = (m + 1) >> 1;
				if (r > n - b) r = n - b;
				Color4 rc;
				rc.SetHSVColor(float(b) / float(n), Sat, Val);
				return rc;
		}

	};
	template <class T>
	inline void Color4<T>::lerp(const Color4<T> &c0, const Color4<T> &c1, const float x)
	{
		assert(x >= 0);
		assert(x <= 1);

		Point4<T>::_v[0] = (T)(c1._v[0] * x + c0._v[0] * (1.0f - x));
		Point4<T>::_v[1] = (T)(c1._v[1] * x + c0._v[1] * (1.0f - x));
		Point4<T>::_v[2] = (T)(c1._v[2] * x + c0._v[2] * (1.0f - x));
		Point4<T>::_v[3] = (T)(c1._v[3] * x + c0._v[3] * (1.0f - x));
	}

	template <class T>
	inline void Color4<T>::lerp(const Color4<T> &c0, const Color4<T> &c1, const Color4<T> &c2, const Point3f &ip)
	{
		assert(fabs(ip[0] + ip[1] + ip[2] - 1) < 0.00001);

		Point4<T>::_v[0] = (T)(c0[0] * ip[0] + c1[0] * ip[1] + c2[0] * ip[2]);
		Point4<T>::_v[1] = (T)(c0[1] * ip[0] + c1[1] * ip[1] + c2[1] * ip[2]);
		Point4<T>::_v[2] = (T)(c0[2] * ip[0] + c1[2] * ip[1] + c2[2] * ip[2]);
		Point4<T>::_v[3] = (T)(c0[3] * ip[0] + c1[3] * ip[1] + c2[3] * ip[2]);
	}


	template <class T>
	inline void Color4<T>::ColorRamp(const float &minf, const float  &maxf, float v)
	{
		if (minf > maxf) { ColorRamp(maxf, minf, maxf + (minf - v)); return; }
		if (v < minf) { *this = Color4<T>(Color4<T>::Red); return; }

		float step = (maxf - minf) / 5;
		v -= minf;
		if (v < step) { lerp(Color4<T>(Color4<T>::Red), Color4<T>(Color4<T>::Yellow), v / step); return; }
		v -= step;
		if (v < step) { lerp(Color4<T>(Color4<T>::Yellow), Color4<T>(Color4<T>::Green), v / step); return; }
		v -= step;
		if (v < step) { lerp(Color4<T>(Color4<T>::Green), Color4<T>(Color4<T>::Cyan), v / step);  return; }
		v -= step;
		if (v < step) { lerp(Color4<T>(Color4<T>::Cyan), Color4<T>(Color4<T>::Blue), v / step);   return; }
		v -= step;
		if (v < step) { lerp(Color4<T>(Color4<T>::Blue), Color4<T>(Color4<T>::White), v / step);   return; }

		*this = Color4<T>(Color4<T>::White);

	}


#if !defined(__GNUC__) || (__GNUC__ > 3)
	template <>
#endif
	template <>
	inline void Color4<float>::Import(const Color4<unsigned char> &b)
	{
		this->_v[0] = b[0] / 255.0f;
		this->_v[1] = b[1] / 255.0f;
		this->_v[2] = b[2] / 255.0f;
		this->_v[3] = b[3] / 255.0f;
	}

#if !defined(__GNUC__) || (__GNUC__ > 3)
	template <>
#endif
	template <>
	inline void Color4<unsigned char>::Import(const Color4<float> &b)
	{
		this->_v[0] = (unsigned char)(b[0] * 255.0f);
		this->_v[1] = (unsigned char)(b[1] * 255.0f);
		this->_v[2] = (unsigned char)(b[2] * 255.0f);
		this->_v[3] = (unsigned char)(b[3] * 255.0f);
	}



	template<>
	inline Color4<unsigned char>::Color4(Color4<unsigned char>::ColorConstant cc)
	{
		*((int *)this) = cc;
	}

	template<>
	inline Color4<float>::Color4(Color4<float>::ColorConstant cc)
	{
		Import(Color4<unsigned char>((Color4<unsigned char>::ColorConstant)cc));
	}


	typedef Color4<unsigned char>  Color4b;
	typedef Color4<float>  Color4f;
}
#endif
