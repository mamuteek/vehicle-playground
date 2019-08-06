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


void vehicle_physics::addWheel(const vec3_m& connectionPointCS, const vec3_m& wheelDirectionCS0, const vec3_m& wheelAxleCS,
										float suspensionRestLength, float wheelRadius, bool isFrontWheel)
{
	vehicle_suspension susp;

	susp.m_chassisConnectionCS = connectionPointCS;
	susp.m_wheelDirectionCS = wheelDirectionCS0;
	susp.m_wheelAxleCS = wheelAxleCS;
	susp.m_suspension_rest_length = suspensionRestLength;
	susp.m_wheel_radius = wheelRadius;
	susp.m_is_front = isFrontWheel;

	m_wheelInfo.push_back(susp);
	updateWheelTransform(getNumWheels() - 1);
}


const placement_m vehicle_physics::getWheelTransformWS(int wheelIndex) const
{
	const vehicle_suspension& wheel = m_wheelInfo[wheelIndex];
	return wheel.m_worldTransform;
}


void vehicle_physics::updateWheelTransform(int wheelIndex)
{
	vehicle_suspension& wheel = m_wheelInfo[wheelIndex];
	const placement_m chassis_trans = getChassisWorldTransform();

	wheel.m_raycastInfo.m_isInContact = false;
	wheel.m_raycastInfo.m_hardPointWS = chassis_trans.transform(wheel.m_chassisConnectionCS);
	wheel.m_raycastInfo.m_wheelDirectionWS = chassis_trans.q.rotate(wheel.m_wheelDirectionCS);
	wheel.m_raycastInfo.m_wheelAxleWS = chassis_trans.q.rotate(wheel.m_wheelAxleCS);

	const vec3_m up = -wheel.m_raycastInfo.m_wheelDirectionWS.getNormalized();
	const vec3_m right = wheel.m_raycastInfo.m_wheelAxleWS.getNormalized();
	//const vec3_m forward = up.cross(right).getNormalized();

	const quat_m rel_rotation(3.14159f / 2.0f, vec3_m(0.0f, 1.0f, 0.0f));

	const quat_m initial_transform = getChassisWorldTransform().q * rel_rotation;
	const quat_m steering_quat(wheel.m_steering, up);
	const quat_m rotating_quat(wheel.m_rotation, right);

	wheel.m_worldTransform.q = steering_quat * rotating_quat * initial_transform;
	wheel.m_worldTransform.p = wheel.m_raycastInfo.m_hardPointWS + wheel.m_raycastInfo.m_wheelDirectionWS * wheel.m_raycastInfo.m_suspensionLength;
}


void vehicle_physics::processRaycasts(void)
{
	for (int idx = 0; idx < m_wheelInfo.size(); idx++) {
		vehicle_suspension& wheel = m_wheelInfo[idx];

		const float raylen = wheel.m_suspension_rest_length + wheel.m_wheel_radius;
		const vec3_m rayvector = wheel.m_raycastInfo.m_wheelDirectionWS * raylen;
		const vec3_m source = wheel.m_raycastInfo.m_hardPointWS;
		wheel.m_raycastInfo.m_contactPointWS = source + rayvector;
		const vec3_m target = wheel.m_raycastInfo.m_contactPointWS;

		physics_actor *hit_actor = nullptr;
		vec3_m hit_point, hit_normal;
		float hit_fraction;

		m_scene->ray_test(source, target, &hit_actor, hit_point, hit_normal, hit_fraction);

		if (hit_actor && hit_fraction > 0.0f) {

			const vec3_m hitPointInWorld = hit_point;
			const vec3_m hitNormalInWorld = hit_normal.getNormalized();
			const float hitFraction = hit_fraction;
			const float hitDepth = raylen * hitFraction;

			wheel.m_raycastInfo.m_contactNormalWS = hitNormalInWorld;
			wheel.m_raycastInfo.m_isInContact = true;
			wheel.m_raycastInfo.m_groundObject = hit_actor->getBody();
			wheel.m_raycastInfo.m_suspensionLength = hitDepth - wheel.m_wheel_radius;

			// Calculate suspension velocity
			wheel.m_raycastInfo.m_contactPointWS = hitPointInWorld;

			const float denominator = wheel.m_raycastInfo.m_contactNormalWS.dot(wheel.m_raycastInfo.m_wheelDirectionWS);
			if (denominator >= float(-0.1f)) {
				wheel.m_suspensionRelativeVelocity = 0.0f;
				wheel.m_clippedInvContactDotSuspension = 10.0f;
			}
			else {
				const vec3_m rel_position = wheel.m_raycastInfo.m_contactPointWS - m_chassisBody->get_placement().p;
				const vec3_m rel_velocity = m_chassisBody->get_rel_velocity_at_rel_pos(rel_position);

				const float proj_velocity = wheel.m_raycastInfo.m_contactNormalWS.dot(rel_velocity);
				const float inv = float(-1.) / denominator;

				wheel.m_suspensionRelativeVelocity = proj_velocity * inv;
				wheel.m_clippedInvContactDotSuspension = inv;
			}
		}
		else {
			// put wheel info into a rest position
			wheel.m_raycastInfo.m_suspensionLength = wheel.m_suspension_rest_length;
			wheel.m_suspensionRelativeVelocity = 0.0f;
			wheel.m_raycastInfo.m_contactNormalWS = -wheel.m_raycastInfo.m_wheelDirectionWS;
			wheel.m_clippedInvContactDotSuspension = 0.0f;
			wheel.m_raycastInfo.m_groundObject = nullptr;
		}
	}
}


const placement_m vehicle_physics::getChassisWorldTransform() const
{
	return m_chassisBody->get_placement();
}


void vehicle_physics::rotateWheels(float timestep)
{
	for (int i = 0; i < m_wheelInfo.size(); i++) {
		vehicle_suspension& wheel = m_wheelInfo[i];
		if (wheel.m_raycastInfo.m_isInContact) {
			const vec3_m rel_position = wheel.m_raycastInfo.m_hardPointWS - m_chassisBody->get_placement().p;
			const vec3_m rel_velocity = m_chassisBody->get_velocity_at_rel_pos(rel_position);
			const vec3_m local_forward(0.0f, 1.0f, 0.0f);
			vec3_m global_forward = getChassisWorldTransform().transform(local_forward);

			const float proj = global_forward.dot(wheel.m_raycastInfo.m_contactNormalWS);
			global_forward -= wheel.m_raycastInfo.m_contactNormalWS * proj;
			const float proj2 = global_forward.dot(rel_velocity);

			wheel.m_delta_rotation = (proj2 * timestep) / (wheel.m_wheel_radius);
			wheel.m_rotation += wheel.m_delta_rotation;
		}
		else {
			// rotation damping when not in contact
			wheel.m_delta_rotation *= 0.99f;
			wheel.m_rotation += wheel.m_delta_rotation;
		}
	}
}



void vehicle_physics::updateVehicle(float timestep)
{
	for (int i = 0; i < getNumWheels(); ++i) {
		updateWheelTransform(i);
	}

	processRaycasts();
	updateSuspension();

	updateFriction();
	rotateWheels(timestep);
}


void vehicle_physics::setSteeringValue(float steering,int wheel)
{
	assert(wheel>=0 && wheel < getNumWheels());
	vehicle_suspension& wheelInfo = getWheelInfo(wheel);
	wheelInfo.m_steering = steering;
}


float vehicle_physics::getSteeringValue(int wheel) const
{
	return getWheelInfo(wheel).m_steering;
}


void vehicle_physics::applyEngineForce(float force, int wheel)
{
	assert(wheel>=0 && wheel < getNumWheels());
	vehicle_suspension& wheelInfo = getWheelInfo(wheel);
	wheelInfo.m_engine_force = force;
}


const vehicle_suspension& vehicle_physics::getWheelInfo(int index) const
{
	assert((index >= 0) && (index < getNumWheels()));
	return m_wheelInfo[index];
}

vehicle_suspension& vehicle_physics::getWheelInfo(int index)
{
	assert((index >= 0) && (index < getNumWheels()));
	return m_wheelInfo[index];
}

void vehicle_physics::setBrake(float brake,int wheelIndex)
{
	assert((wheelIndex >= 0) && (wheelIndex < getNumWheels()));
	getWheelInfo(wheelIndex).m_brake = brake;
	getWheelInfo(wheelIndex).m_engine_force = 0.0f;
}


void vehicle_physics::updateSuspension(void)
{
	for (int idx = 0; idx < getNumWheels(); ++idx) {
		vehicle_suspension &wheel_info = m_wheelInfo[idx];
		if (wheel_info.m_raycastInfo.m_isInContact) {
			// Springing
			const float susp_length = wheel_info.m_suspension_rest_length;
			const float current_length = wheel_info.m_raycastInfo.m_suspensionLength;
			const float diff = (susp_length - current_length);

			// TODO: calibrate the springing
			float suspension_force = wheel_info.m_suspension_stiffness * diff * wheel_info.m_clippedInvContactDotSuspension;

			// Damping
			const float rel_velocity = wheel_info.m_suspensionRelativeVelocity;
			const float suspension_damping = wheel_info.m_suspension_damping * rel_velocity;

			suspension_force -= suspension_damping;

			// Result
			const float mass = m_chassisBody->get_mass();
			suspension_force *= mass;
			if (suspension_force < 0.0f) {
				suspension_force = 0.0f;
			}

			const vec3_m force = wheel_info.m_raycastInfo.m_contactNormalWS * suspension_force;
			const vec3_m rel_position = wheel_info.m_raycastInfo.m_contactPointWS - m_chassisBody->get_placement().p;

			m_chassisBody->add_force_at_rel_pos(force, rel_position);
		}
	}
}



void vehicle_physics::updateFriction(void)
{
	const int wheel_count = getNumWheels();
	if (! wheel_count) {
		return;
	}

	m_forwardImpulse.resize(wheel_count);
	m_sideImpulse.resize(wheel_count);

	for (int i = 0; i < wheel_count; ++i) {
		m_sideImpulse[i] = 0.0f;
		m_forwardImpulse[i] = 0.0f;
	}

	for (int wheel = 0; wheel < wheel_count; wheel++) {
		vehicle_suspension& wheelInfo = m_wheelInfo[wheel];

		if (wheelInfo.m_raycastInfo.m_isInContact) {
			const vec3_m rel_velocity = m_chassisBody->get_velocity_at_rel_pos(m_wheelInfo[wheel].m_chassisConnectionCS);
			const vec3_m side_vector = -wheelInfo.m_raycastInfo.m_wheelAxleWS.getNormalized();
			const float side_rel_velocity = rel_velocity.dot(side_vector);
			m_sideImpulse[wheel] = 2.0f * side_rel_velocity;
		}
	}

	for (int idx = 0; idx < wheel_count; ++idx) {
		const vehicle_suspension *const wheel = &m_wheelInfo[idx];

		if (wheel->m_raycastInfo.m_isInContact) {
			const vec3_m rel_velocity = m_chassisBody->get_velocity_at_rel_pos(m_wheelInfo[idx].m_chassisConnectionCS);
			const vec3_m forward_vector = wheel->m_raycastInfo.m_wheelDirectionWS.getNormalized();;
			const float forward_rel_velocity = rel_velocity.dot(forward_vector);
			const float rotation_velocity = wheel->m_angular_velocity * wheel->m_wheel_radius;
			//const float rel_velocity = forward_rel_velocity;// -rotation_velocity;

			float engine = wheel->m_engine_force;

			
			m_forwardImpulse[idx] = forward_rel_velocity;
			m_forwardImpulse[idx] += wheel->m_engine_force * 10.0f;
		}
	}

	for (int wheel = 0; wheel < getNumWheels(); wheel++) {
		vehicle_suspension& wheelInfo = m_wheelInfo[wheel];
		const vec3_m rel_pos = wheelInfo.m_raycastInfo.m_contactPointWS - m_chassisBody->get_placement().p;

		if (m_forwardImpulse[wheel] != 0.0f) {
			const vec3_m lateral_vector = wheelInfo.m_raycastInfo.m_wheelAxleWS.getNormalized(); 
			const vec3_m up_vector = wheelInfo.m_raycastInfo.m_wheelDirectionWS.getNormalized();
			const vec3_m longitudinal_vector = -lateral_vector.cross(up_vector);
			const vec3_m longitudinal_force =  -m_forwardImpulse[wheel] * longitudinal_vector;
			m_chassisBody->add_force_at_rel_pos(longitudinal_force, rel_pos);
		}
		if (m_sideImpulse[wheel] != 0.0f) {
			const vec3_m lateral_vector = wheelInfo.m_raycastInfo.m_wheelAxleWS.getNormalized(); // vs m_axle[i]
			const vec3_m lateral_force = m_sideImpulse[wheel] * lateral_vector;
			m_chassisBody->add_force_at_rel_pos(lateral_force, rel_pos);
		}
	}
}