#pragma once

class physics_scene;
class physics_shape;
class physics_actor;

struct joint_limit_m {
	float linear_x_min;
	float linear_x_max;

	float linear_y_min;
	float linear_y_max;

	float linear_z_min;
	float linear_z_max;

	float angular_x_min;
	float angular_x_max;

	float angular_y_min;
	float angular_y_max;

	float angular_z_min;
	float angular_z_max;

	joint_limit_m()
		: linear_x_min(0.0f),
		  linear_x_max(0.0f),
		  linear_y_min(0.0f),
		  linear_y_max(0.0f),
		  linear_z_min(0.0f),
		  linear_z_max(0.0f),
		  angular_x_min(0.0f),
		  angular_x_max(0.0f),
		  angular_y_min(0.0f),
		  angular_y_max(0.0f),
		  angular_z_min(0.0f),
		  angular_z_max(0.0f)
		{}
};

class physics_joint
{
public:
	physics_joint();
	~physics_joint();


	virtual void*	get_body(void) = 0;
	virtual void	set_body(void* the_body) = 0;

	virtual void insert(physics_scene* scene) = 0;
	virtual void set_limits(const joint_limit_m &limit) = 0;
};
