#include "Vector2D.hpp"

Vector2D::Vector2D(const Vector2D &vOther)
{
	x = vOther.x; y = vOther.y;
}

// arithmetic operations
Vector2D& Vector2D::operator+=(const Vector2D& v)
{
	x += v.x; y += v.y;
	return *this;
}

Vector2D& Vector2D::operator-=(const Vector2D& v)
{
	x -= v.x; y -= v.y;
	return *this;
}

Vector2D& Vector2D::operator*=(float fl)
{
	x *= fl;
	y *= fl;
	return *this;
}

Vector2D& Vector2D::operator*=(const Vector2D& v)
{
	x *= v.x;
	y *= v.y;
	return *this;
}

Vector2D& Vector2D::operator/=(const Vector2D& v)
{
	x /= v.x;
	y /= v.y;
	return *this;
}

// this ought to be an opcode.
Vector2D& Vector2D::operator+=(float fl)
{
	x += fl;
	y += fl;
	return *this;
}

// this ought to be an opcode.
Vector2D& Vector2D::operator/=(float fl)
{
	x /= fl;
	y /= fl;
	return *this;
}
Vector2D& Vector2D::operator-=(float fl)
{
	x -= fl;
	y -= fl;
	return *this;
}

// Get the vector's magnitude squared.
vec_t Vector2D::LengthSqr(void) const
{
	return (x*x + y*y);
}

// return true if this vector is (0,0,0) within tolerance
bool Vector2D::IsZero(float tolerance) const
{
	return (x > -tolerance && x < tolerance &&
		y > -tolerance && y < tolerance);
}