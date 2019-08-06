#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "quat.h"


// Class representing a rigid euclidean transform as a quat_m and a vector
class placement_m
{
public:
	quat_m q;
	vec3_m p;

	inline placement_m() : q(), p()
	{
	}

	inline explicit placement_m(const vec3_m& position) : q(), p(position)
	{
	}

	inline explicit placement_m(const quat_m& orientation) : q(orientation), p(0)
	{
	}

	inline placement_m(float x, float y, float z, quat_m aQ = quat_m())
		: q(aQ), p(x, y, z)
	{
	}

	inline placement_m(const vec3_m& p0, const quat_m& q0) : q(q0), p(p0)
	{
	}

	inline placement_m(const quat_m& q0, const vec3_m& p0) : q(q0), p(p0)
	{
	}

	// returns true if the two transforms are exactly equal
	inline bool operator==(const placement_m& t) const
	{
		return p == t.p && q == t.q;
	}

	inline placement_m operator*(const placement_m& x) const
	{
		return placement_m(x);
	}

	inline placement_m& operator*=(placement_m& other)
	{
		*this = *this * other;
		return *this;
	}

	inline placement_m get_inverse() const
	{
		return placement_m(q.inverse_rotate(-p), q.get_conjugate());
	}

	inline vec3_m transform(const vec3_m& input) const
	{
		return q.rotate(input) + p;
	}

	inline vec3_m inverse_transform(const vec3_m& input) const
	{
		return q.inverse_rotate(input - p);
	}

	//! Transform transform to parent (returns compound transform: first src, then *this)
	inline placement_m transform(const placement_m& src) const
	{
		// src = [srct, srcr] -> [r*srct + t, r*srcr]
		return placement_m(q.rotate(src.p) + p, q * src.q);
	}

	//! Transform transform from parent (returns compound transform: first src, then this->inverse)
	inline placement_m transformInv(const placement_m& src) const
	{
		// src = [srct, srcr] -> [r^-1*(srct-t), r^-1*srcr]
		quat_m qinv = q.get_conjugate();
		return placement_m(qinv.rotate(src.p - p), qinv * src.q);
	}
};

#endif // #ifndef TRANSFORM_H
