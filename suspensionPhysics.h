#ifndef suspensionPhysics_h
#define suspensionPhysics_h

#include <vehicleSuspension.h>


const float PHYSICS_THRESHOLD_P3 = 1000.0f;
const float PHYSICS_THRESHOLD_P6 = 1000000.0f;
const float PHYSICS_THRESHOLD_N1 = 0.1f;
const float PHYSICS_THRESHOLD_N2 = 0.01f;
const float PHYSICS_THRESHOLD_N3 = 0.001f;
const float PHYSICS_THRESHOLD_N6 = 0.000001f;
const float PHYSICS_THRESHOLD_N7 = 0.0000001f;

const size_t DAMPING_INDEX_MAX = 10;
const size_t SPRINGING_INDEX_MAX = 20;

const float SPRINGING_FORCE_STEER_LUT[] = { 0.000f,	0.050f,	0.120f,	0.160f,	0.190f,	0.200f,	0.210f,	0.250f,	0.300f,	0.400f,	0.500f,	0.620f,	0.750f,	0.890f,	1.000f,	1.120f,	1.250f,	1.380f,	1.520f,	1.650f,	1.800f };
const float SPRINGING_FORCE_FIXED_LUT[] = { 0.000f,	0.050f,	0.120f,	0.160f,	0.190f,	0.200f,	0.210f,	0.250f,	0.300f,	0.400f,	0.500f,	0.700f,	0.950f,	1.150f,	1.320f,	1.470f,	1.600f,	1.740f,	1.860f,	1.950f,	2.000f };
const float SPRINGING_FORCE_TRAILER_LUT[] = { 0.000f, 0.050f,	0.120f,	0.160f,	0.190f,	0.200f,	0.210f,	0.250f,	0.300f,	0.400f,	0.500f,	0.600f,	0.700f,	0.800f,	0.900f,	1.000f,	1.100f,	1.200f,	1.300f,	1.400f,	1.500f };



struct suspension_damping_t
{
	const float* damping_force_compressive;
	float inv_delta_damping;
};


struct suspension_springing_t
{
	const float*	springing_force;
	float			inv_delta_sagging;
	float			res_travel;
	float			suspension_travel;
};


class physics_suspension_t
{
public:
	physics_suspension_t(const float *const springing_lut, const float *const damping_lut, const float residual_travel);

	float	get_damping_force(const float speed) const;
	float	get_springing_force(const float sagging) const;

private:
	suspension_damping_t			damping;
	suspension_springing_t			springing;
	float					sagging;
	float					sagging_next;
	float					sagging_previous;
	float					sagging_filtered;
	float					speed;
	float					supported_weight;
};

#endif
