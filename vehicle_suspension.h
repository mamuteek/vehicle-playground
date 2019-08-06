#pragma once

#include <physics_globals.h>

class physics_actor;


struct vehicle_suspension
{
	struct RaycastInfo
	{
		vec3_m	m_contactNormalWS;
		vec3_m	m_contactPointWS;
		float	m_suspensionLength;
		vec3_m	m_hardPointWS;
		vec3_m	m_wheelDirectionWS;
		vec3_m	m_wheelAxleWS;
		bool	m_isInContact;
		void*	m_groundObject;
	};

	RaycastInfo	m_raycastInfo;

	placement_m	m_worldTransform;

	vec3_m	m_chassisConnectionCS;
	vec3_m	m_wheelDirectionCS;
	vec3_m	m_wheelAxleCS;
	float	m_suspension_rest_length;
	float	m_wheel_radius;
	float	m_suspension_stiffness;
	float	m_suspension_damping;
	float	m_friction_slip;
	float	m_steering;
	float	m_rotation;
	float	m_delta_rotation;
	float	m_roll_influence;

	float	m_angular_velocity;
	float	m_engine_force;
	float	m_brake;
	bool	m_is_front;


	vehicle_suspension()
		: m_suspension_rest_length(0.6f),
		  m_wheel_radius(0.9f),
		  m_suspension_stiffness(5.88f),
		  m_suspension_damping(0.88f),
		  m_friction_slip(1.5f),
		  m_steering(0.0f),
		  m_rotation(0.0f),
		  m_delta_rotation(0.0f),
		  m_roll_influence(0.1f),
		  m_angular_velocity(0.0f),
		  m_engine_force(0.0f),
		  m_brake(0.0f),
		  m_is_front(false)
	{}


	void	updateWheel(physics_actor& chassis, RaycastInfo& raycastInfo);

	float	m_clippedInvContactDotSuspension;
	float	m_suspensionRelativeVelocity;
	//calculated by suspension
	float	m_wheelsSuspensionForce;
	float	m_skidInfo;

};

