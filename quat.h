#ifndef quat_m_H
#define quat_m_H


#include "vector.h"
#include <cmath>
#include <cassert>


class quat_m
{
public:
	/**
	\brief Default constructor, does not do any initialization.
	*/
	inline quat_m() : x(0.0f), y(0.0f), z(0.0f), w(1.0f)
	{
	}

	/**
	\brief Constructor from a scalar: sets the real part w to the scalar value, and the imaginary parts (x,y,z) to zero
	*/
	explicit inline quat_m(float r) : x(0.0f), y(0.0f), z(0.0f), w(r)
	{
	}

	/**
	\brief Constructor.  Take note of the order of the elements!
	*/
	inline quat_m(float nx, float ny, float nz, float nw) : x(nx), y(ny), z(nz), w(nw)
	{
	}

	/**
	\brief Creates from angle-axis representation.

	Axis must be normalized!

	Angle is in radians!

	<b>Unit:</b> Radians
	*/
	inline quat_m(float angleRadians, const vec3_m& axis)
	{
		const vec3_m norm_axis = axis.getNormalized();
		const float a = angleRadians * 0.5f;
		const float s = sin(a);
		w = cos(a);
		x = norm_axis.x * s;
		y = norm_axis.y * s;
		z = norm_axis.z * s;
	}

	/**
	\brief Copy ctor.
	*/
	inline quat_m(const quat_m& v) : x(v.x), y(v.y), z(v.z), w(v.w)
	{
	}

	/**
	\brief Creates from orientation matrix.

	\param[in] m Rotation matrix to extract quat_m from.
	*/
	//explicit inline quat_m(const PxMat33& m); /* defined in PxMat33.h */

																  /**
																  \brief returns true if quat is identity
																  */
	inline bool isIdentity() const
	{
		return x == 0.0f && y == 0.0f && z == 0.0f && w == 1.0f;
	}

	/**
	\brief returns true if the two quat_ms are exactly equal
	*/
	inline bool operator==(const quat_m& q) const
	{
		return x == q.x && y == q.y && z == q.z && w == q.w;
	}

	/**
	\brief converts this quat_m to angle-axis representation
	*/
	inline void toRadiansAndUnitAxis(float& angle, vec3_m& axis) const
	{
		const float quatEpsilon = 1.0e-8f;
		const float s2 = x * x + y * y + z * z;
		if (s2 < quatEpsilon * quatEpsilon) // can't extract a sensible axis
		{
			angle = 0.0f;
			axis = vec3_m(1.0f, 0.0f, 0.0f);
		}
		else
		{
			const float s = 1.0f / sqrt(s2);
			axis = vec3_m(x, y, z) * s;
			angle = abs(w) < quatEpsilon ? 3.14159f : atan2(s2 * s, w) * 2.0f;
		}
	}

	/**
	\brief Gets the angle between this quat and the identity quat_m.

	<b>Unit:</b> Radians
	*/
	inline float getAngle() const
	{
		return acos(w) * 2.0f;
	}

	/**
	\brief Gets the angle between this quat and the argument

	<b>Unit:</b> Radians
	*/
	inline float getAngle(const quat_m& q) const
	{
		return acos(dot(q)) * 2.0f;
	}

	/**
	\brief This is the squared 4D vector length, should be 1 for unit quat_ms.
	*/
	inline float magnitudeSquared() const
	{
		return x * x + y * y + z * z + w * w;
	}

	/**
	\brief returns the scalar product of this and other.
	*/
	inline float dot(const quat_m& v) const
	{
		return x * v.x + y * v.y + z * v.z + w * v.w;
	}

	inline quat_m getNormalized() const
	{
		const float s = 1.0f / magnitude();
		return quat_m(x * s, y * s, z * s, w * s);
	}

	inline float magnitude() const
	{
		return sqrt(magnitudeSquared());
	}

	// modifiers:
	/**
	\brief maps to the closest unit quat_m.
	*/
	inline float normalize() // convert this PxQuat to a unit quat_m
	{
		const float mag = magnitude();
		if (mag != 0.0f)
		{
			const float imag = 1.0f / mag;

			x *= imag;
			y *= imag;
			z *= imag;
			w *= imag;
		}
		return mag;
	}

	/*
	\brief returns the conjugate.

	\note for unit quat_ms, this is the inverse.
	*/
	inline quat_m get_conjugate() const
	{
		return quat_m(-x, -y, -z, w);
	}

	/*
	\brief returns imaginary part.
	*/
	inline vec3_m get_imaginary_oart() const
	{
		return vec3_m(x, y, z);
	}

	/** brief computes rotation of x-axis */
	inline vec3_m get_x_rotation() const
	{
		const float x2 = x * 2.0f;
		const float w2 = w * 2.0f;
		return vec3_m((w * w2) - 1.0f + x * x2, (z * w2) + y * x2, (-y * w2) + z * x2);
	}

	/** brief computes rotation of y-axis */
	inline vec3_m get_y_rotation() const
	{
		const float y2 = y * 2.0f;
		const float w2 = w * 2.0f;
		return vec3_m((-z * w2) + x * y2, (w * w2) - 1.0f + y * y2, (x * w2) + z * y2);
	}

	/** brief computes rotation of z-axis */
	inline vec3_m get_z_rotation() const
	{
		const float z2 = z * 2.0f;
		const float w2 = w * 2.0f;
		return vec3_m((y * w2) + x * z2, (-x * w2) + y * z2, (w * w2) - 1.0f + z * z2);
	}

	/**
	rotates passed vec by this (assumed unitary)
	*/
	inline const vec3_m rotate(const vec3_m& v) const
	{
		const float vx = 2.0f * v.x;
		const float vy = 2.0f * v.y;
		const float vz = 2.0f * v.z;
		const float w2 = w * w - 0.5f;
		const float dot2 = (x * vx + y * vy + z * vz);
		return vec3_m((vx * w2 + (y * vz - z * vy) * w + x * dot2), (vy * w2 + (z * vx - x * vz) * w + y * dot2),
			(vz * w2 + (x * vy - y * vx) * w + z * dot2));
	}

	/**
	inverse rotates passed vec by this (assumed unitary)
	*/
	inline const vec3_m inverse_rotate(const vec3_m& v) const
	{
		const float vx = 2.0f * v.x;
		const float vy = 2.0f * v.y;
		const float vz = 2.0f * v.z;
		const float w2 = w * w - 0.5f;
		const float dot2 = (x * vx + y * vy + z * vz);
		return vec3_m((vx * w2 - (y * vz - z * vy) * w + x * dot2), (vy * w2 - (z * vx - x * vz) * w + y * dot2),
			(vz * w2 - (x * vy - y * vx) * w + z * dot2));
	}

	/**
	\brief Assignment operator
	*/
	inline quat_m& operator=(const quat_m& p)
	{
		x = p.x;
		y = p.y;
		z = p.z;
		w = p.w;
		return *this;
	}

	inline quat_m& operator*=(const quat_m& q)
	{
		const float tx = w * q.x + q.w * x + y * q.z - q.y * z;
		const float ty = w * q.y + q.w * y + z * q.x - q.z * x;
		const float tz = w * q.z + q.w * z + x * q.y - q.x * y;

		w = w * q.w - q.x * x - y * q.y - q.z * z;
		x = tx;
		y = ty;
		z = tz;

		return *this;
	}

	inline quat_m& operator+=(const quat_m& q)
	{
		x += q.x;
		y += q.y;
		z += q.z;
		w += q.w;
		return *this;
	}

	inline quat_m& operator-=(const quat_m& q)
	{
		x -= q.x;
		y -= q.y;
		z -= q.z;
		w -= q.w;
		return *this;
	}

	inline quat_m& operator*=(const float s)
	{
		x *= s;
		y *= s;
		z *= s;
		w *= s;
		return *this;
	}

	/** quat_m multiplication */
	inline quat_m operator*(const quat_m& q) const
	{
		return quat_m(w * q.x + q.w * x + y * q.z - q.y * z, w * q.y + q.w * y + z * q.x - q.z * x,
			w * q.z + q.w * z + x * q.y - q.x * y, w * q.w - x * q.x - y * q.y - z * q.z);
	}

	/** quat_m addition */
	inline quat_m operator+(const quat_m& q) const
	{
		return quat_m(x + q.x, y + q.y, z + q.z, w + q.w);
	}

	/** quat_m subtraction */
	inline quat_m operator-() const
	{
		return quat_m(-x, -y, -z, -w);
	}

	inline quat_m operator-(const quat_m& q) const
	{
		return quat_m(x - q.x, y - q.y, z - q.z, w - q.w);
	}

	inline quat_m operator*(float r) const
	{
		return quat_m(x * r, y * r, z * r, w * r);
	}

	/** the quat_m elements */
	float x, y, z, w;
};

#endif // #ifndef PXFOUNDATION_PXQUAT_H

