/*
Copyright(c) 2016-2018 Panos Karabelas

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

//= INCLUDES =======
#include "Vector3.h"
//==================

namespace Directus::Math
{
	class ENGINE_CLASS Quaternion
	{
	public:
		// Constructs an identity quaternion
		Quaternion()
		{
			x = 0;
			y = 0;
			z = 0;
			w = 1;
		}

		// Constructs a new quaternion with the given components
		Quaternion(float x, float y, float z, float w)
		{
			this->x = x;
			this->y = y;
			this->z = z;
			this->w = w;
		}

		~Quaternion() {}

		//= FROM ====================================================================
		// Define from an angle (in degrees) and axis.
		static Quaternion FromAngleAxis(float angle, const Vector3& axis)
		{
			float half = angle * 0.5f;
		    float sin = sinf(half);
		    float cos = cosf(half);
			return Quaternion(axis.x * sin, axis.y * sin, axis.z * sin, cos);
		}
		void FromAxes(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis);

		// Euler angles to quaternion (input in degrees)
		static Quaternion FromEulerAngles(const Vector3& axes)						{ return FromYawPitchRoll(axes.y * DEG_TO_RAD, axes.x * DEG_TO_RAD, axes.z * DEG_TO_RAD); }
		static Quaternion FromEulerAngles(float xAxis, float yAxis, float zAxis)	{ return FromYawPitchRoll(yAxis * DEG_TO_RAD, xAxis * DEG_TO_RAD, zAxis * DEG_TO_RAD); }

		// Yaw - Y, Pitch - X, Roll - Z, Radians
		static Quaternion FromYawPitchRoll(float yaw, float pitch, float roll)
		{
			float halfRoll	= roll * 0.5f;
            float halfPitch = pitch * 0.5f;
            float halfYaw	= yaw * 0.5f;

            float sinRoll	= sin(halfRoll);
            float cosRoll	= cos(halfRoll);
            float sinPitch	= sin(halfPitch);
            float cosPitch	= cos(halfPitch);
            float sinYaw	= sin(halfYaw);
            float cosYaw	= cos(halfYaw);

			return Quaternion(
				cosYaw * sinPitch * cosRoll + sinYaw * cosPitch * sinRoll,
				sinYaw * cosPitch * cosRoll - cosYaw * sinPitch * sinRoll,
				cosYaw * cosPitch * sinRoll - sinYaw * sinPitch * cosRoll,
				cosYaw * cosPitch * cosRoll + sinYaw * sinPitch * sinRoll
			);
		}
		//===========================================================================

		//= TO ================================================================================
		// Returns euler angles in degrees
		Vector3 ToEulerAngles() const
		{
			// Derivation from http://www.geometrictools.com/Documentation/EulerAngles.pdf
			// Order of rotations: Z first, then X, then Y
			float check = 2.0f * (-y * z + w * x);

			if (check < -0.995f)
			{
				return Vector3
				(
					-90.0f,
					0.0f,
					-atan2f(2.0f * (x * z - w * y), 1.0f - 2.0f * (y * y + z * z)) * RAD_TO_DEG
				);
			}

			if (check > 0.995f)
			{
				return Vector3
				(
					90.0f,
					0.0f,
					atan2f(2.0f * (x * z - w * y), 1.0f - 2.0f * (y * y + z * z)) * RAD_TO_DEG
				);
			}

			return Vector3
			(
				asinf(check) * RAD_TO_DEG,
				atan2f(2.0f * (x * z + w * y), 1.0f - 2.0f * (x * x + y * y)) * RAD_TO_DEG,
				atan2f(2.0f * (x * y + w * z), 1.0f - 2.0f * (x * x + z * z)) * RAD_TO_DEG
			);
		}

		float Yaw() const	{ return ToEulerAngles().y; }
		float Pitch() const { return ToEulerAngles().x; }
		float Roll() const	{ return ToEulerAngles().z; }
		//=====================================================================================

		void FromRotationTo(const Vector3& start, const Vector3& end)
		{
			Vector3 normStart = start.Normalized();
			Vector3 normEnd = end.Normalized();
			float d = normStart.Dot(normEnd);

			if (d > -1.0f + M_EPSILON)
			{
				Vector3 c = normStart.Cross(normEnd);
				float s = sqrtf((1.0f + d) * 2.0f);
				float invS = 1.0f / s;

				x = c.x * invS;
				y = c.y * invS;
				z = c.z * invS;
				w = 0.5f * s;
			}
			else
			{
				Vector3 axis = Vector3::Right.Cross(normStart);
				if (axis.Length() < M_EPSILON)
				{
					axis = Vector3::Up.Cross(normStart);
				}

				FromAngleAxis(180.0f, axis);
			}
		}

		static Quaternion FromLookRotation(const Vector3& direction, const Vector3& upDirection = Vector3::Up)
		{
			Quaternion ret;
			Vector3 forward = direction.Normalized();

			Vector3 v = forward.Cross(upDirection);
			if (v.LengthSquared() >= M_EPSILON)
			{
				v.Normalize();
				Vector3 up = v.Cross(forward);
				Vector3 right = up.Cross(forward);
				ret.FromAxes(right, up, forward);
			}
			else
				ret.FromRotationTo(Vector3::Forward, forward);

			return ret;
		}

		static Quaternion FromToRotation(const Quaternion& start, const Quaternion& end)
		{
			return Inverse(start) * end;
		}

		Quaternion Conjugate() const { return Quaternion(w, -x, -y, -z); }
		float LengthSquared() const { return w * w + x * x + y * y + z * z; }

		//= NORMALIZATION =====================================
		void Normalize()
		{
			float lenSquared = LengthSquared();
			if (!Equals(lenSquared, 1.0f) && lenSquared > 0.0f)
			{
				float invLen = 1.0f / sqrtf(lenSquared);
				w *= invLen;
				x *= invLen;
				y *= invLen;
				z *= invLen;
			}
		}

		Quaternion Normalized() const
		{
			float lenSquared = LengthSquared();
			if (!Equals(lenSquared, 1.0f) && lenSquared > 0.0f)
			{
				float invLen = 1.0f / sqrtf(lenSquared);
				return *this * invLen;
			}

			return *this;
		}

		static Quaternion Inverse(const Quaternion& q);
		Quaternion Inverse() const { return Inverse(*this); }
		//=====================================================

		//= ASSIGNMENT ==============================
		Quaternion& operator =(const Quaternion& rhs)
		{
			w = rhs.w;
			x = rhs.x;
			y = rhs.y;
			z = rhs.z;

			return *this;
		}
		//===========================================

		//= MULTIPLICATION ==============================================================================
		Quaternion operator*(const Quaternion& rhs) const
		{
			return Quaternion(
				w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z,
				w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y,
				w * rhs.y + y * rhs.w + z * rhs.x - x* rhs.z,
				w * rhs.z + z * rhs.w + x * rhs.y - y * rhs.x
			);
		}

		void operator*=(const Quaternion& rhs)
		{
			x *= rhs.x;
			y *= rhs.y;
			z *= rhs.z;
			w *= rhs.w;
		}

		Vector3 operator*(const Vector3& rhs) const
		{
			Vector3 qVec(x, y, z);
			Vector3 cross1(qVec.Cross(rhs));
			Vector3 cross2(qVec.Cross(cross1));

			return rhs + 2.0f * (cross1 * w + cross2);
		}

		Quaternion& operator *=(float rhs)
		{
			w *= rhs;
			x *= rhs;
			y *= rhs;
			z *= rhs;

			return *this;
		}

		Quaternion operator *(float rhs) const { return Quaternion(w * rhs, x * rhs, y * rhs, z * rhs); }
		//===============================================================================================

		//= COMPARISON =========================================================================
		bool operator ==(const Quaternion& rhs) const
		{
			// Test for a quality while allowing for some error
			return Equals(w, rhs.w) && Equals(x, rhs.x) && Equals(y, rhs.y) && Equals(z, rhs.z);
		}
		bool operator !=(const Quaternion& rhs) const { return !(*this == rhs); }
		//======================================================================================

		std::string ToString() const;

		float x, y, z, w;

		static const Quaternion Identity;
	};

	// Reverse order operators
	inline ENGINE_CLASS Vector3 operator*(const Vector3& lhs, const Quaternion& rhs) { return rhs * lhs; }
	inline ENGINE_CLASS Quaternion operator*(float lhs, const Quaternion& rhs) { return rhs * lhs; }
}
