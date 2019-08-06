#ifndef VECTOR3_H
#define VECTOR3_H

#include <cassert>
#include <cmath>

class vec3_m
{
public:
	/**
	\brief default constructor leaves data uninitialized.
	*/
	inline vec3_m() : x(0.0f), y(0.0f), z(0.0f)
	{
	}

	/**
	\brief Assigns scalar parameter to all elements.

	Useful to initialize to zero or one.

	\param[in] a Value to assign to elements.
	*/
	explicit inline vec3_m(float a) : x(a), y(a), z(a)
	{
	}

	/**
	\brief Initializes from 3 scalar parameters.

	\param[in] nx Value to initialize X component.
	\param[in] ny Value to initialize Y component.
	\param[in] nz Value to initialize Z component.
	*/
	inline vec3_m(float nx, float ny, float nz) : x(nx), y(ny), z(nz)
	{
	}

	/**
	\brief Copy ctor.
	*/
	inline vec3_m(const vec3_m& v) : x(v.x), y(v.y), z(v.z)
	{
	}

	// Operators

	/**
	\brief Assignment operator
	*/
	inline vec3_m& operator=(const vec3_m& p)
	{
		x = p.x;
		y = p.y;
		z = p.z;
		return *this;
	}

	/**
	\brief element access
	*/
	inline float& operator[](unsigned int index)
	{
		assert(index <= 2);

		return reinterpret_cast<float*>(this)[index];
	}

	/**
	\brief element access
	*/
	inline const float& operator[](unsigned int index) const
	{
		assert(index <= 2);

		return reinterpret_cast<const float*>(this)[index];
	}

	/**
	\brief returns true if the two vectors are exactly equal.
	*/
	inline bool operator==(const vec3_m& v) const
	{
		return x == v.x && y == v.y && z == v.z;
	}

	/**
	\brief returns true if the two vectors are not exactly equal.
	*/
	inline bool operator!=(const vec3_m& v) const
	{
		return x != v.x || y != v.y || z != v.z;
	}

	/**
	\brief tests for exact zero vector
	*/
	inline bool isZero() const
	{
		return x == 0.0f && y == 0.0f && z == 0.0f;
	}

	/**
	\brief returns the squared magnitude

	Avoids calling PxSqrt()!
	*/
	inline float magnitudeSquared() const
	{
		return x * x + y * y + z * z;
	}

	/**
	\brief returns the magnitude
	*/
	inline float magnitude() const
	{
		return sqrt(magnitudeSquared());
	}

	/**
	\brief negation
	*/
	inline vec3_m operator-() const
	{
		return vec3_m(-x, -y, -z);
	}

	/**
	\brief vector addition
	*/
	inline vec3_m operator+(const vec3_m& v) const
	{
		return vec3_m(x + v.x, y + v.y, z + v.z);
	}

	/**
	\brief vector difference
	*/
	inline vec3_m operator-(const vec3_m& v) const
	{
		return vec3_m(x - v.x, y - v.y, z - v.z);
	}

	/**
	\brief scalar post-multiplication
	*/
	inline vec3_m operator*(float f) const
	{
		return vec3_m(x * f, y * f, z * f);
	}

	/**
	\brief scalar division
	*/
	inline vec3_m operator/(float f) const
	{
		f = 1.0f / f;
		return vec3_m(x * f, y * f, z * f);
	}

	/**
	\brief vector addition
	*/
	inline vec3_m& operator+=(const vec3_m& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	/**
	\brief vector difference
	*/
	inline vec3_m& operator-=(const vec3_m& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	/**
	\brief scalar multiplication
	*/
	inline vec3_m& operator*=(float f)
	{
		x *= f;
		y *= f;
		z *= f;
		return *this;
	}
	/**
	\brief scalar division
	*/
	inline vec3_m& operator/=(float f)
	{
		f = 1.0f / f;
		x *= f;
		y *= f;
		z *= f;
		return *this;
	}

	/**
	\brief returns the scalar product of this and other.
	*/
	inline float dot(const vec3_m& v) const
	{
		return x * v.x + y * v.y + z * v.z;
	}

	/**
	\brief cross product
	*/
	inline vec3_m cross(const vec3_m& v) const
	{
		return vec3_m(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}

	/** return a unit vector */

	inline vec3_m getNormalized() const
	{
		const float m = magnitudeSquared();
		return m > 0.0f ? *this * 1.0f / sqrt(m) : vec3_m(0, 0, 0);
	}

	/**
	\brief normalizes the vector in place
	*/
	inline float normalize()
	{
		const float m = magnitude();
		if (m > 0.0f)
			*this /= m;
		return m;
	}

	/**
	\brief normalizes the vector in place. Does nothing if vector magnitude is under PX_NORMALIZATION_EPSILON.
	Returns vector magnitude if >= PX_NORMALIZATION_EPSILON and 0.0f otherwise.
	*/
	inline float normalizeSafe()
	{
		const float mag = magnitude();
		if (mag < float(1e-20f))
			return 0.0f;
		*this *= 1.0f / mag;
		return mag;
	}

	/**
	\brief normalizes the vector in place. Asserts if vector magnitude is under PX_NORMALIZATION_EPSILON.
	returns vector magnitude.
	*/
	inline float normalizeFast()
	{
		const float mag = magnitude();
		assert(mag >= float(1e-20f));
		*this *= 1.0f / mag;
		return mag;
	}

	/**
	\brief a[i] * b[i], for all i.
	*/
	inline vec3_m multiply(const vec3_m& a) const
	{
		return vec3_m(x * a.x, y * a.y, z * a.z);
	}

	float x, y, z;
};

static inline vec3_m operator*(float f, const vec3_m& v)
{
	return vec3_m(f * v.x, f * v.y, f * v.z);
}

static inline vec3_m operator*(const vec3_m& v1, const vec3_m& v2)
{
	return vec3_m(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
}



#endif
