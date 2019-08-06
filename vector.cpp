#include <vector.h>

void float3::reset(void)
{
	x = y = z = 0.0f;
}

float3 float3::operator=(float3 &p) {
	x = p.x;
	y = p.y;
	z = p.z;

	return *this;
}

void float3::operator+=(float3 &p)
{
	x += p.x;
	y += p.y;
	z += p.z;
}