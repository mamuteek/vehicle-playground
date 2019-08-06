#include <vehicle_suspension.h>
#include <physics_actor.h>


void vehicle_suspension::updateWheel(physics_actor& chassis,RaycastInfo& raycastInfo)
{
	(void)raycastInfo;

	if (m_raycastInfo.m_isInContact) {
		const float project = m_raycastInfo.m_contactNormalWS.dot(m_raycastInfo.m_wheelDirectionWS);
		if (project >= -0.1f) {
			m_suspensionRelativeVelocity = 0.0f;
			m_clippedInvContactDotSuspension = 10.0f;
		}
		else  {
			const vec3_m rel_pos = m_raycastInfo.m_contactPointWS - chassis.get_placement().p;
			const vec3_m chassis_velocity_at_contactPoint = chassis.get_velocity_at_rel_pos(rel_pos);
			const float proj_vel = m_raycastInfo.m_contactNormalWS.dot(chassis_velocity_at_contactPoint);
			const float inv_project = 1.0f / project;
			m_suspensionRelativeVelocity = proj_vel * inv_project;
			m_clippedInvContactDotSuspension = inv_project;
		}

	}
	else { // Not in contact : position wheel in a nice (rest length) position
		m_raycastInfo.m_suspensionLength = m_suspension_rest_length;
		m_suspensionRelativeVelocity = 0.0f;
		m_raycastInfo.m_contactNormalWS = -m_raycastInfo.m_wheelDirectionWS;
		m_clippedInvContactDotSuspension = 1.0f;
	}
}
