#pragma once

#include <sstream>

namespace SDK
{
	class Vector
	{
	public:
		Vector(void)
		{
			Invalidate();
		}
		Vector(float X, float Y, float Z)
		{
			x = X;
			y = Y;
			z = Z;
		}
		Vector(const float* clr)
		{
			x = clr[0];
			y = clr[1];
			z = clr[2];
		}

		void Init(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f)
		{
			x = ix; y = iy; z = iz;
		}
		bool IsValid() const
		{
			return std::isfinite(x) && std::isfinite(y) && std::isfinite(z);
		}
		void Invalidate()
		{
			x = y = z = std::numeric_limits<float>::infinity();
		}

		float& operator[](int i)
		{
			return ((float*)this)[i];
		}
		float operator[](int i) const
		{
			return ((float*)this)[i];
		}

		void Zero()
		{
			x = y = z = 0.0f;
		}

		bool operator==(const Vector& src) const
		{
			return (src.x == x) && (src.y == y) && (src.z == z);
		}
		bool operator!=(const Vector& src) const
		{
			return (src.x != x) || (src.y != y) || (src.z != z);
		}

		Vector& operator+=(const Vector& v)
		{
			x += v.x; y += v.y; z += v.z;
			return *this;
		}
		Vector& operator-=(const Vector& v)
		{
			x -= v.x; y -= v.y; z -= v.z;
			return *this;
		}
		Vector& operator*=(float fl)
		{
			x *= fl;
			y *= fl;
			z *= fl;
			return *this;
		}
		Vector& operator*=(const Vector& v)
		{
			x *= v.x;
			y *= v.y;
			z *= v.z;
			return *this;
		}
		Vector& operator/=(const Vector& v)
		{
			x /= v.x;
			y /= v.y;
			z /= v.z;
			return *this;
		}
		Vector& operator+=(float fl)
		{
			x += fl;
			y += fl;
			z += fl;
			return *this;
		}
		Vector& operator/=(float fl)
		{
			x /= fl;
			y /= fl;
			z /= fl;
			return *this;
		}
		Vector& operator-=(float fl)
		{
			x -= fl;
			y -= fl;
			z -= fl;
			return *this;
		}

		Vector Vector::operator+(float fl) const
		{
			return Vector(x + fl, y + fl, z + fl);
		}

		Vector Vector::operator-(float fl) const
		{
			return Vector(x - fl, y - fl, z - fl);
		}

		Vector Unit() const
		{ //same as Normalized() function
			Vector res = *this;
			float length = Length();
			if (length == 0.f)
			{
				res.z = 1.f;
				return res;
			}
			res.x /= length;
			res.y /= length;
			res.z /= length;
			return res;
		}

		Vector UnitSqr() const
		{ //same as Normalized() function but with LengthSqr
			Vector res = *this;
			float length = LengthSqr();
			if (length == 0.f)
			{
				res.z = 1.f;
				return res;
			}
			res.x /= length;
			res.y /= length;
			res.z /= length;
			return res;
		}

		bool inRange(double lower, double upper) const
		{
			if (!(lower < x && x < upper)) // python > c++ < python
				return false;
			if (!(lower < y && y < upper))
				return false;
			if (!(lower < z && z < upper))
				return false;
			return true;
		}

		float DistTo(const Vector &vOther) const
		{
			Vector delta;

			delta.x = x - vOther.x;
			delta.y = y - vOther.y;
			delta.z = z - vOther.z;

			return delta.Length();
		}
		float DistToSqr(const Vector &vOther) const
		{
			Vector delta;

			delta.x = x - vOther.x;
			delta.y = y - vOther.y;
			delta.z = z - vOther.z;

			return delta.LengthSqr();
		}
		float Dot(const Vector& vOther) const
		{
			return (x*vOther.x + y*vOther.y + z*vOther.z);
		}
		float Length() const
		{
			return sqrt(x*x + y*y + z*z);
		}
		float LengthSqr(void) const
		{
			return (x*x + y*y + z*z);
		}
		float Length2D() const
		{
			return sqrt(x*x + y*y);
		}

		Vector& operator=(const Vector &vOther)
		{
			x = vOther.x; y = vOther.y; z = vOther.z;
			return *this;
		}

		Vector Vector::operator-(void) const
		{
			return Vector(-x, -y, -z);
		}
		Vector Vector::operator+(const Vector& v) const
		{
			return Vector(x + v.x, y + v.y, z + v.z);
		}
		Vector Vector::operator-(const Vector& v) const
		{
			return Vector(x - v.x, y - v.y, z - v.z);
		}
		Vector Vector::operator*(float fl) const
		{
			return Vector(x * fl, y * fl, z * fl);
		}
		Vector Vector::operator*(const Vector& v) const
		{
			return Vector(x * v.x, y * v.y, z * v.z);
		}
		Vector Vector::operator/(float fl) const
		{
			return Vector(x / fl, y / fl, z / fl);
		}
		Vector Vector::operator/(const Vector& v) const
		{
			return Vector(x / v.x, y / v.y, z / v.z);
		}

		float x, y, z;
	};

	inline Vector operator*(float lhs, const Vector& rhs)
	{
		return rhs * lhs;
	}
	inline Vector operator/(float lhs, const Vector& rhs)
	{
		return rhs / lhs;
	}

	class __declspec(align(16)) VectorAligned : public Vector
	{
	public:
		inline VectorAligned(void) {};
		inline VectorAligned(float X, float Y, float Z)
		{
			Init(X, Y, Z);
		}

	public:
		explicit VectorAligned(const Vector &vOther)
		{
			Init(vOther.x, vOther.y, vOther.z);
		}

		VectorAligned& operator=(const Vector &vOther)
		{
			Init(vOther.x, vOther.y, vOther.z);
			return *this;
		}

		VectorAligned& operator=(const VectorAligned &vOther)
		{
			Init(vOther.x, vOther.y, vOther.z);
			return *this;
		}

		float w;
	};
}