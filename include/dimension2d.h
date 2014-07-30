#ifndef __DREAM_DIMENTION2D
#define __DREAM_DIMENTION2D
#include "dtype.h"
template <class U>
class vector2d;

template <class T>
class dimension2d {
public:
	dimension2d():Width(T(0)),Height(T(0)){};

	dimension2d(const T w,const T h):Width(w),Height(h){}

	template <class U>
	dimension2d(const U w,const U h):Width((T)w),Height((T)h){}

	template <class U>
	dimension2d(const dimension2d<U>& o) {
		Width = (T)o.Width;
		Height = (T)o.Height;
	}

	dimension2d(const vector2d<T>& v);

	bool operator == (const dimension2d<T>& o) const {
		return Width == o.Width&& Height == o.Height;
	}

	bool operator != (const dimension2d<T>& o) const {
		return !(Width == o.Width&&Height == o.Height);
	}

	bool operator == (const vector2d<T>& v) const ;

	bool operator != (const vector2d<T> &v) const {
		return !(*this == v);
	}

	dimension2d<T>& set(const T& w,const T& h) {
		Width = w;
		Height = h;
		return *this;
	}

	dimension2d<T>& operator /= (const T& scale) {
		Width /= scale;
		Height /= scale;
		return *this;
	}

	dimension2d<T>& operator / (const T& scale) {
		return dimension2d<T>(Width/scale,Height/scale);
	}

	dimension2d<T>& operator *= (const T& scale) {
		Width *= scale;
		Height *= scale;
		return *this;
	}

	dimension2d<T>& operator * (const T& scale) {
		return dimension2d<T>(Width*scale,Height*scale);
	}

	dimension2d<T>& operator + (const dimension2d<T>& o) {
		return dimension2d<T>(Width+o.Width,Height+o.Height);
	}

	dimension2d<T>& operator += (const dimension2d<T>& o) {
		Width += o.Width;
		Height += o.Height;
		return *this;
	}

	dimension2d<T>& operator -= (const dimension2d<T>& o) {
		Width -= o.Width;
		Height -= o.Height;
		return *this;
	}

	dimension2d<T>& operator - (const dimension2d<T>& o) {
		return dimension2d<T>(Width-o.Width,Height-o.Height);
	}

	T getArea() const {
		return Width*Height;
	}

	 dimension2d<T> getOptimalSize(
					bool requirePowerOfTwo=true,
					bool requireSquare=false,
					bool larger=true,
					u32 maxValue = 0) const
	{
		u32 i=1;
		u32 j=1;
		if (requirePowerOfTwo)
		{
			while (i<(u32)Width)
				i<<=1;
			if (!larger && i!=1 && i!=(u32)Width)
				i>>=1;
			while (j<(u32)Height)
				j<<=1;
			if (!larger && j!=1 && j!=(u32)Height)
				j>>=1;
		}
		else
		{
			i=(u32)Width;
			j=(u32)Height;
		}
		if (requireSquare)
		{
			if ((larger && (i>j)) || (!larger && (i<j)))
				j=i;
			else
				i=j;
		}

		if ( maxValue > 0 && i > maxValue)
			i = maxValue;
		if ( maxValue > 0 && j > maxValue)
			j = maxValue;

		return dimension2d<T>((T)i,(T)j);
	}

public:
	T Width,Height;	
};
typedef dimension2d<f32> dimension2df;
typedef dimension2d<u32> dimension2du;
typedef dimension2d<s32> dimension2di;
#endif
