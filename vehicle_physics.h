#pragma once

#include <vehicle_suspension.h>
#include <vector>

class physics_actor;
class physics_scene;

class vehicle_physics
{
	std::vector<float>		m_forwardImpulse;
	std::vector<float>		m_sideImpulse;

	physics_actor *m_chassisBody;
	physics_scene *m_scene;


public:
	vehicle_physics(physics_actor* chassis, physics_scene *scene);
	virtual ~vehicle_physics() ;

	virtual void processRaycasts(void);
	virtual void updateSuspension(void);
	virtual void updateFriction(void);
	virtual void rotateWheels(float timestep);

	virtual void updateVehicle(float timestep);


	const placement_m	getChassisWorldTransform() const;
	float				getSteeringValue(int wheel) const;
	void				setSteeringValue(float steering, int wheel);
	void				applyEngineForce(float force, int wheel);
	const placement_m	getWheelTransformWS(int wheelIndex) const;
	void				updateWheelTransform(int wheelIndex);

	void addWheel(const vec3_m& connectionPointCS0, const vec3_m& wheelDirectionCS0,const vec3_m& wheelAxleCS,float suspensionRestLength,float wheelRadius, bool isFrontWheel);

	inline int		getNumWheels() const {
		return int (m_wheelInfo.size());
	}

	std::vector<vehicle_suspension>	m_wheelInfo;


	const vehicle_suspension&	getWheelInfo(int index) const;

	vehicle_suspension&	getWheelInfo(int index);

	void setBrake(float brake,int wheelIndex);

	vec3_m getForwardVector() const
	{
		const placement_m& chassisTrans = getChassisWorldTransform();

		const vec3_m forward_axis_local(0.0f, 1.0f, 0.0f);
		const vec3_m forward_axis_global = chassisTrans.transform(forward_axis_local);
		return forward_axis_global;
	}

};
