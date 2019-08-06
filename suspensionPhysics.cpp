#include <suspensionPhysics.h>



physics_suspension_t::physics_suspension_t(const float *const springing_lut, const float *const damping_lut, const float residual_travel)
{
	springing.res_travel = residual_travel;
	springing.springing_force = springing_lut;
	springing.inv_delta_sagging = static_cast<float>(SPRINGING_INDEX_MAX);
	springing.suspension_travel = residual_travel;

	// sagging is set negative to be able to recognize if it has been initialized by a suspension probe
	sagging = sagging_next = sagging_previous = sagging_filtered = -1.0f;

	damping.damping_force_compressive = damping_lut;
	damping.inv_delta_damping = static_cast<float>(DAMPING_INDEX_MAX);

	speed = 0.0f;
	supported_weight = 1.0f;
}




float physics_suspension_t::get_damping_force(const float speed) const
{
	if (speed < 0.0f) {
		return 0.0;// - damping.damping_force_rebound[DAMPING_INDEX_MAX];
	}

	if (speed > 0.999f) {
		return damping.damping_force_compressive[DAMPING_INDEX_MAX];
	}

	const float *damping_force = damping.damping_force_compressive;

	const float float_index = speed * damping.inv_delta_damping;
	const size_t index = static_cast<size_t>(float_index);
	const float s = float_index - index;

	const float interpolated_force = damping_force[index] * s + damping_force[index + 1] * (1 - s);
	return interpolated_force;
}


float physics_suspension_t::get_springing_force(const float sagging) const
{
	if (sagging < PHYSICS_THRESHOLD_N3) {
		return 0.0f;
	}
	if (sagging > 0.999f) {
		return springing.springing_force[SPRINGING_INDEX_MAX];
	}

	const float float_index = sagging * springing.inv_delta_sagging;
	const size_t index = static_cast<size_t>(float_index);
	const float s = float_index - index;

	const float interpolated_force = springing.springing_force[index] * s + springing.springing_force[index + 1] * (1 - s);
	//assert(interpolated_force > -PHYSICS_THRESHOLD_N6);
	return interpolated_force;
}
