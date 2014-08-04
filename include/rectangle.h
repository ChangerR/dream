#ifndef __DREAM_rectangleANGLE
#define __DREAM_rectangleANGLE
#include "position2d.h"
template <class T>
class rectangle
{
public:

	//! Default constructor creating empty rectangleangle at (0,0)
	rectangle() : UpperLeftCorner(0,0), LowerRightCorner(0,0) {}

	//! Constructor with two corners
	rectangle(T x, T y, T x2, T y2)
		: UpperLeftCorner(x,y), LowerRightCorner(x2,y2) {}

	//! Constructor with two corners
	rectangle(const position2d<T>& upperLeft, const position2d<T>& lowerRight)
		: UpperLeftCorner(upperLeft), LowerRightCorner(lowerRight) {}

	//! Constructor with upper left corner and dimension
	template <class U>
	rectangle(const position2d<T>& pos, const dimension2d<U>& size)
		: UpperLeftCorner(pos), LowerRightCorner(pos.X + size.Width, pos.Y + size.Height) {}

	//! move right by given numbers
	rectangle<T> operator+(const position2d<T>& pos) const
	{
		rectangle<T> ret(*this);
		return ret+=pos;
	}

	//! move right by given numbers
	rectangle<T>& operator+=(const position2d<T>& pos)
	{
		UpperLeftCorner += pos;
		LowerRightCorner += pos;
		return *this;
	}

	//! move left by given numbers
	rectangle<T> operator-(const position2d<T>& pos) const
	{
		rectangle<T> ret(*this);
		return ret-=pos;
	}

	//! move left by given numbers
	rectangle<T>& operator-=(const position2d<T>& pos)
	{
		UpperLeftCorner -= pos;
		LowerRightCorner -= pos;
		return *this;
	}

	//! equality operator
	bool operator==(const rectangle<T>& other) const
	{
		return (UpperLeftCorner == other.UpperLeftCorner &&
			LowerRightCorner == other.LowerRightCorner);
	}

	//! inequality operator
	bool operator!=(const rectangle<T>& other) const
	{
		return (UpperLeftCorner != other.UpperLeftCorner ||
			LowerRightCorner != other.LowerRightCorner);
	}

	//! compares size of rectangleangles
	bool operator<(const rectangle<T>& other) const
	{
		return getArea() < other.getArea();
	}

	//! Returns size of rectangleangle
	T getArea() const
	{
		return getWidth() * getHeight();
	}

	//! Returns if a 2d point is within this rectangleangle.
	/** \param pos Position to test if it lies within this rectangleangle.
	\return True if the position is within the rectangleangle, false if not. */
	bool isPointInside(const position2d<T>& pos) const
	{
		return (UpperLeftCorner.X <= pos.X &&
			UpperLeftCorner.Y <= pos.Y &&
			LowerRightCorner.X >= pos.X &&
			LowerRightCorner.Y >= pos.Y);
	}

	//! Check if the rectangleangle collides with another rectangleangle.
	/** \param other rectangleangle to test collision with
	\return True if the rectangleangles collide. */
	bool isrectangleCollided(const rectangle<T>& other) const
	{
		return (LowerRightCorner.Y > other.UpperLeftCorner.Y &&
			UpperLeftCorner.Y < other.LowerRightCorner.Y &&
			LowerRightCorner.X > other.UpperLeftCorner.X &&
			UpperLeftCorner.X < other.LowerRightCorner.X);
	}

	//! Clips this rectangleangle with another one.
	/** \param other rectangleangle to clip with */
	void clipAgainst(const rectangle<T>& other)
	{
		if (other.LowerRightCorner.X < LowerRightCorner.X)
			LowerRightCorner.X = other.LowerRightCorner.X;
		if (other.LowerRightCorner.Y < LowerRightCorner.Y)
			LowerRightCorner.Y = other.LowerRightCorner.Y;

		if (other.UpperLeftCorner.X > UpperLeftCorner.X)
			UpperLeftCorner.X = other.UpperLeftCorner.X;
		if (other.UpperLeftCorner.Y > UpperLeftCorner.Y)
			UpperLeftCorner.Y = other.UpperLeftCorner.Y;

		// correctangle possible invalid rectangle resulting from clipping
		if (UpperLeftCorner.Y > LowerRightCorner.Y)
			UpperLeftCorner.Y = LowerRightCorner.Y;
		if (UpperLeftCorner.X > LowerRightCorner.X)
			UpperLeftCorner.X = LowerRightCorner.X;
	}

	//! Moves this rectangleangle to fit inside another one.
	/** \return True on success, false if not possible */
	bool constrainTo(const rectangle<T>& other)
	{
		if (other.getWidth() < getWidth() || other.getHeight() < getHeight())
			return false;

		T diff = other.LowerRightCorner.X - LowerRightCorner.X;
		if (diff < 0)
		{
			LowerRightCorner.X += diff;
			UpperLeftCorner.X  += diff;
		}

		diff = other.LowerRightCorner.Y - LowerRightCorner.Y;
		if (diff < 0)
		{
			LowerRightCorner.Y += diff;
			UpperLeftCorner.Y  += diff;
		}

		diff = UpperLeftCorner.X - other.UpperLeftCorner.X;
		if (diff < 0)
		{
			UpperLeftCorner.X  -= diff;
			LowerRightCorner.X -= diff;
		}

		diff = UpperLeftCorner.Y - other.UpperLeftCorner.Y;
		if (diff < 0)
		{
			UpperLeftCorner.Y  -= diff;
			LowerRightCorner.Y -= diff;
		}

		return true;
	}

	//! Get width of rectangleangle.
	T getWidth() const
	{
		return LowerRightCorner.X - UpperLeftCorner.X;
	}

	//! Get height of rectangleangle.
	T getHeight() const
	{
		return LowerRightCorner.Y - UpperLeftCorner.Y;
	}

	//! If the lower right corner of the rectangle is smaller then the upper left, the points are swapped.
	void repair()
	{
		if (LowerRightCorner.X < UpperLeftCorner.X)
		{
			T t = LowerRightCorner.X;
			LowerRightCorner.X = UpperLeftCorner.X;
			UpperLeftCorner.X = t;
		}

		if (LowerRightCorner.Y < UpperLeftCorner.Y)
		{
			T t = LowerRightCorner.Y;
			LowerRightCorner.Y = UpperLeftCorner.Y;
			UpperLeftCorner.Y = t;
		}
	}

	//! Returns if the rectangle is valid to draw.
	/** It would be invalid if the UpperLeftCorner is lower or more
	right than the LowerRightCorner. */
	bool isValid() const
	{
		return ((LowerRightCorner.X >= UpperLeftCorner.X) &&
			(LowerRightCorner.Y >= UpperLeftCorner.Y));
	}

	//! Get the center of the rectangleangle
	position2d<T> getCenter() const
	{
		return position2d<T>(
				(UpperLeftCorner.X + LowerRightCorner.X) / 2,
				(UpperLeftCorner.Y + LowerRightCorner.Y) / 2);
	}

	//! Get the dimensions of the rectangleangle
	dimension2d<T> getSize() const
	{
		return dimension2d<T>(getWidth(), getHeight());
	}


	//! Adds a point to the rectangleangle
	/** Causes the rectangleangle to grow bigger if point is outside of
	the box
	\param p Point to add to the box. */
	void addInternalPoint(const position2d<T>& p)
	{
		addInternalPoint(p.X, p.Y);
	}

	//! Adds a point to the bounding rectangleangle
	/** Causes the rectangleangle to grow bigger if point is outside of
	the box
	\param x X-Coordinate of the point to add to this box.
	\param y Y-Coordinate of the point to add to this box. */
	void addInternalPoint(T x, T y)
	{
		if (x>LowerRightCorner.X)
			LowerRightCorner.X = x;
		if (y>LowerRightCorner.Y)
			LowerRightCorner.Y = y;

		if (x<UpperLeftCorner.X)
			UpperLeftCorner.X = x;
		if (y<UpperLeftCorner.Y)
			UpperLeftCorner.Y = y;
	}

	//! Upper left corner
	position2d<T> UpperLeftCorner;
	//! Lower right corner
	position2d<T> LowerRightCorner;
};

//! rectangleangle with float values
typedef rectangle<f32> rectanglef;
//! rectangleangle with int values
typedef rectangle<s32> rectanglei;
#endif