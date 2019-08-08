#pragma once

#include <vehicle_suspension.h>
#include <vector>

class physics_actor;
class physics_scene;

class vehicle_physics
{
	std::vector<float>		m_longitudinal_force;
	std::vector<float>		m_lateral_force;

	physics_actor *m_chassisBody;
	physics_scene *m_scene;

	std::vector<vehicle_suspension*> m_wheels;

public:
	vehicle_physics(physics_actor* chassis, physics_scene *scene);
	virtual ~vehicle_physics() ;

	virtual void process_raycasts(void);
	virtual void update_suspension(const float timestep);
	virtual void update_friction(void);
	virtual void rotate_wheels(float timestep);

	virtual void update_vehicle(float timestep);

	float				get_steering	(int wheel);
	void				set_steering	(float steering, int wheel);
	void				apply_engine_force	(float force, int wheel);
	void				set_brake			(float brake, int wheelIndex);

	vehicle_suspension*	get_wheel	(int index);


	const placement_m	get_wheel_world_transform	(int wheelIndex) const;
	const placement_m	get_chassis_world_transform	(void) const;
	void				update_wheel_transform		(int wheelIndex);

	void add_wheel(	const vec3_m& connectionPointCS0, const vec3_m& wheelDirectionCS0,const vec3_m& wheelAxleCS,float suspensionRestLength,float wheelRadius, bool isFrontWheel);

	inline int get_wheel_count() const {
		return int (m_wheels.size());
	}
};
