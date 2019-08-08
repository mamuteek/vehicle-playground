#include <vehicle_suspension.h>
#include <vehicle_physics.h>

#include <cassert>
#include <iostream>

#include <physics_actor.h>
#include <physics_shape.h>
#include <physics_scene.h>

#include <physics_globals.h>


vehicle_physics::vehicle_physics(physics_actor* chassis, physics_scene *scene)
	: m_chassisBody(chassis),
	  m_scene(scene)
{
}


vehicle_physics::~vehicle_physics()
{
}


void vehicle_physics::add_wheel(const vec3_m& connectionPointCS, const vec3_m& wheelDirectionCS0, const vec3_m& wheelAxleCS,
										float suspensionRestLength, float wheelRadius, bool isFrontWheel)
{
	vehicle_suspension *susp = new vehicle_suspension;

	susp->m_chassisConnectionCS = connectionPointCS;
	susp->m_wheelDirectionCS = wheelDirectionCS0;
	susp->m_wheelAxleCS = wheelAxleCS;
	susp->m_suspension_rest_length = suspensionRestLength;
	susp->m_wheel_radius = wheelRadius;
	susp->m_is_front = isFrontWheel;

	m_wheels.push_back(susp);
	update_wheel_transform(get_wheel_count() - 1);
}


const placement_m vehicle_physics::get_wheel_world_transform(int wheelIndex) const
{
	const vehicle_suspension* wheel = m_wheels[wheelIndex];
	return wheel->m_worldTransform;
}


void vehicle_physics::update_wheel_transform(int wheelIndex)
{
	vehicle_suspension *wheel = m_wheels[wheelIndex];
	const placement_m chassis_trans = get_chassis_world_transform();
	const quat_m rel_rotation(3.14159f / 2.0f, vec3_m(0.0f, 1.0f, 0.0f));

	const vec3_m up = -wheel->m_wheelDirectionCS.getNormalized();
	const vec3_m right = wheel->m_wheelAxleCS.getNormalized();
	const vec3_m forward = up.cross(right).getNormalized();

	const quat_m initial_transform = chassis_trans.q * rel_rotation;
	const quat_m steering_quat(wheel->m_steering, up);
	const quat_m rotating_quat(wheel->m_rotation * 0.0f, right);

	wheel->m_raycastInfo.m_isInContact = false;
	wheel->m_raycastInfo.m_hardPointWS = chassis_trans.transform(wheel->m_chassisConnectionCS);
	wheel->m_raycastInfo.m_wheelDirectionWS = chassis_trans.q.rotate(wheel->m_wheelDirectionCS);
	wheel->m_raycastInfo.m_wheelAxleWS = chassis_trans.q.rotate(steering_quat.rotate(wheel->m_wheelAxleCS));
	wheel->m_raycastInfo.m_wheelForwardWS = chassis_trans.q.rotate(wheel->m_wheelDirectionCS.cross(wheel->m_wheelAxleCS));

	wheel->m_worldTransform.q = steering_quat * rotating_quat * initial_transform;
	wheel->m_worldTransform.p = wheel->m_raycastInfo.m_hardPointWS + wheel->m_raycastInfo.m_wheelDirectionWS * wheel->m_raycastInfo.m_suspension_length;
}


void vehicle_physics::process_raycasts(void)
{
	for (int idx = 0; idx < m_wheels.size(); idx++) {
		vehicle_suspension *wheel = m_wheels[idx];

		const float raylen = wheel->m_suspension_rest_length + wheel->m_wheel_radius;
		const vec3_m rayvector = wheel->m_raycastInfo.m_wheelDirectionWS * raylen;
		const vec3_m source = wheel->m_raycastInfo.m_hardPointWS;
		wheel->m_raycastInfo.m_contactPointWS = source + rayvector;
		const vec3_m target = wheel->m_raycastInfo.m_contactPointWS;

		physics_actor *hit_actor = nullptr;
		vec3_m hit_point, hit_normal;
		float hit_fraction;

		m_scene->ray_test(source, target, &hit_actor, hit_point, hit_normal, hit_fraction);

		if (hit_actor && hit_fraction > 0.0f) {

			const vec3_m hitPointInWorld = hit_point;
			const vec3_m hitNormalInWorld = hit_normal.getNormalized();
			const float hitFraction = hit_fraction;
			const float hitDepth = raylen * hitFraction;

			wheel->m_raycastInfo.m_contactNormalWS = hitNormalInWorld;
			wheel->m_raycastInfo.m_isInContact = true;
			wheel->m_raycastInfo.m_groundObject = hit_actor->getBody();
			wheel->m_raycastInfo.m_suspension_length_last = wheel->m_raycastInfo.m_suspension_length;
			wheel->m_raycastInfo.m_suspension_length = hitDepth - wheel->m_wheel_radius;

			// Calculate suspension velocity
			wheel->m_raycastInfo.m_contactPointWS = hitPointInWorld;
		}
		else {
			// put wheel info into a rest position
			wheel->m_raycastInfo.m_suspension_length = wheel->m_suspension_rest_length;
			wheel->m_raycastInfo.m_contactNormalWS = -wheel->m_raycastInfo.m_wheelDirectionWS;
			wheel->m_raycastInfo.m_groundObject = nullptr;
		}
	}
}


const placement_m vehicle_physics::get_chassis_world_transform(void) const
{
	return m_chassisBody->get_placement();
}


void vehicle_physics::rotate_wheels(float timestep)
{
	for (int i = 0; i < m_wheels.size(); i++) {
		vehicle_suspension *wheel = m_wheels[i];
		if (wheel->m_raycastInfo.m_isInContact) {
			const vec3_m rel_position = wheel->m_raycastInfo.m_hardPointWS - m_chassisBody->get_placement().p;
			const vec3_m rel_velocity = m_chassisBody->get_velocity_at_rel_pos(rel_position);
			const vec3_m local_forward(0.0f, 1.0f, 0.0f);
			vec3_m global_forward = get_chassis_world_transform().transform(local_forward);

			const float proj = global_forward.dot(wheel->m_raycastInfo.m_contactNormalWS);
			global_forward -= wheel->m_raycastInfo.m_contactNormalWS * proj;
			const float proj2 = global_forward.dot(rel_velocity);

			wheel->m_delta_rotation = (proj2 * timestep) / (wheel->m_wheel_radius);
			wheel->m_rotation += wheel->m_delta_rotation;
		}
		else {
			// rotation damping when not in contact
			wheel->m_delta_rotation *= 0.99f;
			wheel->m_rotation += wheel->m_delta_rotation;
		}
	}
}



void vehicle_physics::update_vehicle(float timestep)
{
	for (int i = 0; i < m_wheels.size(); ++i) {
		update_wheel_transform(i);
	}

	process_raycasts();
	update_suspension(timestep);

	update_friction();
	rotate_wheels(timestep);
}


void vehicle_physics::set_steering(float steering,int wheel_idx)
{
	assert(wheel_idx >= 0 && wheel_idx < m_wheels.size());
	vehicle_suspension *wheel = get_wheel(wheel_idx);
	wheel->m_steering = steering;
}


float vehicle_physics::get_steering(int wheel_idx)
{
	vehicle_suspension *wheel = get_wheel(wheel_idx);
	return wheel->m_steering;
}


void vehicle_physics::apply_engine_force(float force, int wheel_idx)
{
	assert(wheel_idx >= 0 && wheel_idx < m_wheels.size());
	vehicle_suspension *wheel = get_wheel(wheel_idx);
	wheel->m_engine_force = force;
}

vehicle_suspension *vehicle_physics::get_wheel(int index)
{
	assert((index >= 0) && (index < m_wheels.size()));
	return m_wheels[index];
}

void vehicle_physics::set_brake(float brake,int wheel_idx)
{
	assert((wheel_idx >= 0) && (wheel_idx < m_wheels.size()));
	get_wheel(wheel_idx)->m_brake = brake;
}


void vehicle_physics::update_suspension(const float timestep)
{
	for (int idx = 0; idx < m_wheels.size(); ++idx) {
		vehicle_suspension *wheel = m_wheels[idx];
		if (wheel->m_raycastInfo.m_isInContact) {
			// Springing
			const float rest_length = wheel->m_suspension_rest_length;
			const float previous_length = wheel->m_raycastInfo.m_suspension_length_last;
			const float current_length = wheel->m_raycastInfo.m_suspension_length;
			const float diff = (rest_length - current_length);

			// TODO: calibrate the springing
			float suspension_force = wheel->m_suspension_stiffness * diff;

			// Damping
			const float rel_velocity = (current_length - previous_length) / timestep;
			const float suspension_damping = wheel->m_suspension_damping * rel_velocity;

			suspension_force -= suspension_damping;

			// Result
			const float mass = m_chassisBody->get_mass();
			suspension_force *= mass;
			if (suspension_force < 0.0f) {
				suspension_force = 0.0f;
			}

			const vec3_m force = wheel->m_raycastInfo.m_contactNormalWS * suspension_force;
			const vec3_m rel_position = wheel->m_raycastInfo.m_contactPointWS - m_chassisBody->get_placement().p;

			m_chassisBody->add_force_at_rel_pos(force, rel_position);
		}
	}
}



void vehicle_physics::update_friction(void)
{
	const size_t wheel_count = m_wheels.size();
	if (! wheel_count) {
		return;
	}

	m_longitudinal_force.resize(wheel_count);
	m_lateral_force.resize(wheel_count);

	for (int idx = 0; idx < wheel_count; ++idx) {
		m_lateral_force[idx] = 0.0f;
		m_longitudinal_force[idx] = 0.0f;
	}

	for (int idx = 0; idx < wheel_count; ++idx) {
		const vehicle_suspension *const wheel = m_wheels[idx];

		if (wheel->m_raycastInfo.m_isInContact) {
			const vec3_m rel_velocity = m_chassisBody->get_velocity_at_rel_pos(wheel->m_chassisConnectionCS);
			const vec3_m side_vector = -wheel->m_raycastInfo.m_wheelAxleWS.getNormalized();
			const float side_rel_velocity = rel_velocity.dot(side_vector);
			m_lateral_force[idx] = 1.0f * side_rel_velocity;
		}
	}

	for (int idx = 0; idx < wheel_count; ++idx) {
		const vehicle_suspension *const wheel = m_wheels[idx];

		if (wheel->m_raycastInfo.m_isInContact) {
			const vec3_m rel_velocity = m_chassisBody->get_velocity_at_rel_pos(wheel->m_chassisConnectionCS);
			const vec3_m forward_vector = m_chassisBody->get_placement().q.rotate(vec3_m(1.0f, 0.0f, 0.0f));
			const float forward_rel_velocity = -rel_velocity.dot(forward_vector);

			const float coeff = 1.0f / (1.0f + 2.0f * forward_rel_velocity);
			const int sgn = (wheel->m_engine_force > 0.0f ? 1 : ((wheel->m_engine_force < 0.0f) ? -1 : 0));
			if (idx == 2 && (wheel->m_engine_force != 0.0f || wheel->m_brake != 0.0f)) {
				m_longitudinal_force[idx] = abs(wheel->m_engine_force - wheel->m_brake) * 5.0f * coeff;
			}
		}
	}

	for (int idx = 0; idx < m_wheels.size(); ++idx) {
		const vehicle_suspension *const wheel = m_wheels[idx];
		const vec3_m rel_pos = m_chassisBody->get_placement().inverse_transform(wheel->m_raycastInfo.m_contactPointWS);

		if (m_longitudinal_force[idx] != 0.0f && idx == 2) {
			const float vel_magn = m_chassisBody->get_velocity_at_rel_pos(vec3_m(0.0f, 0.0f, 0.0f)).magnitude();
			const float coeff = vel_magn > 5.0f ? 0.0f : 1.0f;
			const vec3_m longitudinal_vector2 = wheel->m_raycastInfo.m_wheelForwardWS;
			const vec3_m forward_vector = m_chassisBody->get_placement().q.rotate(vec3_m(1.0f, 0.0f, 0.0f));
			const vec3_m longitudinal_force = (-coeff * wheel->m_engine_force * 10.0f + coeff * wheel->m_brake * 1.0f) * vec3_m(1.0f, 0.0f, 0.0f);// m_longitudinal_force[idx] * forward_vector;
			//m_chassisBody->add_force_at_rel_pos(longitudinal_force, rel_pos);
			m_chassisBody->add_rel_force_at_rel_pos(longitudinal_force, vec3_m(0.0f, 0.0f, 0.0f));
		}
		if (m_lateral_force[idx] != 0.0f) {
			const vec3_m lateral_vector = wheel->m_raycastInfo.m_wheelAxleWS.getNormalized();
			const vec3_m lateral_force = m_lateral_force[idx] * lateral_vector;
			m_chassisBody->add_force_at_rel_pos(lateral_force, rel_pos);
		}
	}
}