#pragma once
#include <math.h>
#include "Vector.hpp"

namespace SDK
{
	class QAngle
	{
	public:
		QAngle(void);
		QAngle(float X, float Y, float Z);
		QAngle(const float* clr);
		QAngle(Vector From, Vector To);
		QAngle(Vector Offset);

		void Init(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f);

		float operator[](int i) const;
		float& operator[](int i);

		QAngle& operator+=(const QAngle& v);
		QAngle& operator-=(const QAngle& v);

		QAngle& operator*=(float fl);
		QAngle& operator*=(const QAngle& v);
		QAngle& operator/=(const QAngle& v);
		QAngle& operator+=(float fl);
		QAngle& operator/=(float fl);
		QAngle& operator-=(float fl);

		QAngle& operator=(const QAngle &vOther);

		QAngle operator-(void) const;
		QAngle operator+(const QAngle& v) const;
		QAngle operator-(const QAngle& v) const;
		QAngle operator*(float fl) const;
		QAngle operator*(const QAngle& v) const;
		QAngle operator/(float fl) const;
		QAngle operator/(const QAngle& v) const;

		float Length() const;
		float LengthSqr(void) const;
		float distanceTo(QAngle other);
		QAngle NormalizedTo(QAngle other);
		bool IsZero(float tolerance = 0.01f) const;

		void Normalize();
		void Unit();

		float pitch;
		float yaw;
		float roll;
	};
}