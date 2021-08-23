#include "QAngle.hpp"

namespace SDK
{
	QAngle::QAngle(void)
	{
		Init();
	}
	QAngle::QAngle(float X, float Y, float Z)
	{
		Init(X, Y, Z);
	}
	QAngle::QAngle(const float* clr)
	{
		Init(clr[0], clr[1], clr[2]);
	}

	QAngle::QAngle(Vector From, Vector To)
	{
		Vector offset = To - From;
		float p = -asin(offset.z / offset.Length()); //in radians
		float y = atan2f(offset.y, offset.x); //in radians
		Init(p, y, 0);
		*this *= 180 / 3.1415926f;
	}

	QAngle::QAngle(Vector offset)
	{
		float p = -asin(offset.z / offset.Length()); //in radians
		float y = atan2f(offset.y, offset.x); //in radians
		Init(p, y, 0);
		*this *= 180 / 3.1415926f;
	}

	void QAngle::Init(float ix, float iy, float iz)
	{
		pitch = ix;
		yaw = iy;
		roll = iz;
	}

	float QAngle::operator[](int i) const
	{
		return ((float*)this)[i];
	}
	float& QAngle::operator[](int i)
	{
		return ((float*)this)[i];
	}

	QAngle& QAngle::operator+=(const QAngle& v)
	{
		pitch += v.pitch; yaw += v.yaw; roll += v.roll;
		return *this;
	}
	QAngle& QAngle::operator-=(const QAngle& v)
	{
		pitch -= v.pitch; yaw -= v.yaw; roll -= v.roll;
		return *this;
	}
	QAngle& QAngle::operator*=(float fl)
	{
		pitch *= fl;
		yaw *= fl;
		roll *= fl;
		return *this;
	}
	QAngle& QAngle::operator*=(const QAngle& v)
	{
		pitch *= v.pitch;
		yaw *= v.yaw;
		roll *= v.roll;
		return *this;
	}
	QAngle& QAngle::operator/=(const QAngle& v)
	{
		pitch /= v.pitch;
		yaw /= v.yaw;
		roll /= v.roll;
		return *this;
	}
	QAngle& QAngle::operator+=(float fl)
	{
		pitch += fl;
		yaw += fl;
		roll += fl;
		return *this;
	}
	QAngle& QAngle::operator/=(float fl)
	{
		pitch /= fl;
		yaw /= fl;
		roll /= fl;
		return *this;
	}
	QAngle& QAngle::operator-=(float fl)
	{
		pitch -= fl;
		yaw -= fl;
		roll -= fl;
		return *this;
	}

	QAngle& QAngle::operator=(const QAngle &vOther)
	{
		pitch = vOther.pitch; yaw = vOther.yaw; roll = vOther.roll;
		return *this;
	}

	QAngle QAngle::operator-(void) const
	{
		return QAngle(-pitch, -yaw, -roll);
	}
	QAngle QAngle::operator+(const QAngle& v) const
	{
		return QAngle(pitch + v.pitch, yaw + v.yaw, roll + v.roll);
	}
	QAngle QAngle::operator-(const QAngle& v) const
	{
		return QAngle(pitch - v.pitch, yaw - v.yaw, roll - v.roll);
	}
	QAngle QAngle::operator*(float fl) const
	{
		return QAngle(pitch * fl, yaw * fl, roll * fl);
	}
	QAngle QAngle::operator*(const QAngle& v) const
	{
		return QAngle(pitch * v.pitch, yaw * v.yaw, roll * v.roll);
	}
	QAngle QAngle::operator/(float fl) const
	{
		return QAngle(pitch / fl, yaw / fl, roll / fl);
	}
	QAngle QAngle::operator/(const QAngle& v) const
	{
		return QAngle(pitch / v.pitch, yaw / v.yaw, roll / v.roll);
	}

	float QAngle::Length() const
	{
		return (float)sqrt(pitch*pitch + yaw*yaw + roll*roll);
	}
	float QAngle::LengthSqr(void) const
	{
		return (pitch*pitch + yaw*yaw + roll*roll);
	}
	float QAngle::distanceTo(QAngle other)
	{
		/*
		//General idea for a looping distance:

		//(current)->(-180)->(180)->(other)
		float negativeloop = ( current - (-180) ) + (180 - other);
		//(current)->(180)->(-180)->(other)
		float positiveloop = (180 - current) + (other - (-180));
		//change if we dont loop
		float noloop = fabsf(current - other);
		//we want the shortest change possible:
		float distance = fminf(fminf(negativeloop, positiveloop), noloop);
		*/

		//the above can be represented in this single `line`:
		float yawdis = fminf(
			/*negative distance*/
			fmodf((yaw - (-180)) + (180 - other.yaw), 360),
			/*positive distance*/
			fmodf((180 - yaw) + (other.yaw - (-180)), 360)
		);
		

		//same idea for pitch
		//note that in csgo, pitch cannot loop, and this is overkill
		float pitchdis = fminf(
			/*negative distance*/
			fmodf((pitch - (-180)) + (180 - other.pitch), 360),
			/*positive distance*/
			fmodf((180 - pitch) + (other.pitch - (-180)), 360)
		);

		//same idea for roll
		//note that in csgo, roll is always 0, and this is unnecessary
		float rolldis = fminf(
			/*negative distance*/
			fmodf((roll - (-180)) + (180 - other.roll), 360),
			/*positive distance*/
			fmodf((180 - roll) + (other.roll - (-180)), 360)
		);


		//pythagorean 
		return sqrtf(pow(pitchdis, 2) + pow(yawdis, 2) + pow(rolldis, 2));
	}
	bool QAngle::IsZero(float tolerance) const
	{
		return (pitch > -tolerance && pitch < tolerance &&
			yaw > -tolerance && yaw < tolerance &&
			roll > -tolerance && roll < tolerance);
	}

	QAngle QAngle::NormalizedTo(QAngle other)
	{
		QAngle newAngle;
		//follow steps in distanceTo


		//the above can be represented in this single `line`:
		newAngle.yaw = fminf(
			/*negative distance*/
			fmodf((yaw - (-180)) + (180 - other.yaw), 360),
			/*positive distance*/
			fmodf((180 - yaw) + (other.yaw - (-180)), 360)
		);


		//same idea for pitch
		//note that in csgo, pitch cannot loop, and this is overkill
		newAngle.pitch = fminf(
			/*negative distance*/
			fmodf((pitch - (-180)) + (180 - other.pitch), 360),
			/*positive distance*/
			fmodf((180 - pitch) + (other.pitch - (-180)), 360)
		);

		//same idea for roll
		//note that in csgo, roll is always 0, and this is unnecessary
		newAngle.roll = fminf(
			/*negative distance*/
			fmodf((roll - (-180)) + (180 - other.roll), 360),
			/*positive distance*/
			fmodf((180 - roll) + (other.roll - (-180)), 360)
		);

		return newAngle;
	}

	void QAngle::Normalize()
	{
		//try to normalize
		if (pitch > 89)
			pitch -= 179;
		if (pitch < -89)
			pitch += 179;
		if (yaw > 179.999f)
			yaw -= 360;
		if (yaw < -179.999f)
			yaw += 360;

		//try to normalize again
		if (pitch > 89)
			pitch -= 179;
		if (pitch < -89)
			pitch += 179;
		if (yaw > 179.999f)
			yaw -= 360;
		if (yaw < -179.999f)
			yaw += 360;

		//clamp
		if (pitch > 89)
			pitch = 89;
		if (pitch < -89)
			pitch = -89;
		if (yaw > 179.999f)
			yaw = 179.999f;
		if (yaw < -179.999f)
			yaw = 179.999f;
		roll = 0;
	}

	void QAngle::Unit()
	{
		float length = Length();
		if (length == 0.f)
		{
			yaw = 1.f;
			return;
		}
		pitch /= length;
		yaw /= length;
		roll /= length;
	}

	inline QAngle operator*(float lhs, const QAngle& rhs)
	{
		return rhs * lhs;
	}
	inline QAngle operator/(float lhs, const QAngle& rhs)
	{
		return rhs / lhs;
	}
}