#include <shared_vehicle.h>
#include <cassert>

#include <physics_actor.h>
#include <physics_scene.h>
#include <physics_server.h>
#include <physics_shape.h>


shared_vehicle::shared_vehicle(physics_scene *scene, physics_server *physics, const placement_m& transform, const float mass, const vehicle_type type) :
	m_chassisActor(nullptr),
	m_suspension(nullptr),
	m_wheelShape(),
	m_wheelActor(),
	m_scene(scene),
	m_physics(physics)
{
	m_chassisActor = m_physics->create_new_actor(transform);
	m_chassisActor->set_mass(mass);
	m_chassisActor->set_inertia(vec3_m(1.0f, 1.0f, 1.0f));
	unsigned col_group, col_mask;
	vec3_m wheel_extents;

	if (type == vehicle_type_TRUCK) {
		m_chassisActor->set_inertia(vec3_m(4.4f, 7.9f, 81.4f));
		const vec3_m cabin_size(2.5f, 2.5f, 3.0f);
		const placement_m cabin_transform(vec3_m(-2.5f, 3.2f, 0.0f));
		physics_shape *const cabinShape = m_physics->create_box(cabin_size);
		m_chassisActor->attach_shape(cabinShape, cabin_transform);

		const vec3_m chassis_size(5.0f, 0.6f, 3.0f);
		const placement_m truck_chassis_transform(vec3_m(0.0f, 0.0f, 0.0f));
		physics_shape *const chassisShape = m_physics->create_box(chassis_size);
		m_chassisActor->attach_shape(chassisShape, truck_chassis_transform);

		m_chassisActor->set_collision_group(COL_TRUCK);
		col_group = COL_TRUCK;
		col_mask = COL_ALL;
		wheel_extents = vec3_m(4.0f, 0.7f, 3.0f);
	}
	else if (type == vehicle_type_TRAILER) {

		m_chassisActor->set_inertia(vec3_m(12.0f, 35.9f, 249.4f));
		const float TRAILER_WIDTH = 4.0f;
		const vec3_m cargo_size(8.0f, 1.8f, TRAILER_WIDTH);
		const placement_m cargo_transform(vec3_m(-4.0f, 3.0f, 0.0f));
		physics_shape *const cargoShape = m_physics->create_box(cargo_size);
		m_chassisActor->attach_shape(cargoShape, cargo_transform);

		const vec3_m trailerChassisSize(4.0f, 1.0f, TRAILER_WIDTH);
		const placement_m trailerChassisTransform(vec3_m(0.0f, 0.0f, 0.0f));
		physics_shape *const trailerChassisShape = m_physics->create_box(trailerChassisSize);
		m_chassisActor->attach_shape(trailerChassisShape, trailerChassisTransform);

		const vec3_m testSize1(4.0f, 1.8f, 0.5f);
		const placement_m testTransform1(quat_m(0.0f, 0.2f, 0.0f, 1.0f).getNormalized(), vec3_m(-8.125f, 3.0f, TRAILER_WIDTH - 1.1f));
		physics_shape *const testShape1 = m_physics->create_box(testSize1);
		m_chassisActor->attach_shape(testShape1, testTransform1);

		const vec3_m testSize2(4.0f, 1.8f, 0.5f);
		const placement_m testTransform2(quat_m(0.0f, -0.2f, 0.0f, 1.0f).getNormalized(), vec3_m(-8.125f, 3.0f, -(TRAILER_WIDTH - 1.1f)));
		physics_shape *const testShape2 = m_physics->create_box(testSize2);
		m_chassisActor->attach_shape(testShape2, testTransform2);

		m_chassisActor->set_collision_group(COL_TRAILER);
		col_group = COL_TRAILER;
		col_mask = COL_ALL;
		wheel_extents = vec3_m(4.0f, 0.7f, 3.0f);
	}

	m_chassisActor->set_placement(transform);
	m_chassisActor->auto_set_inertia();
	m_chassisActor->set_sleep_thresholds(0.01f, 0.01f);
	m_scene->insert(m_chassisActor, col_group, col_mask);

	m_suspension = new vehicle_physics(m_chassisActor, m_scene);
	addWheels(wheel_extents);
}


shared_vehicle::~shared_vehicle()
{
	delete m_suspension;
	delete m_chassisActor;
}


void shared_vehicle::physics_update()
{
}


void shared_vehicle::physics_post_update()
{
	updateWheels();
	m_suspension->update_vehicle(1.f / 60.f);
}


physics_actor *shared_vehicle::get_actor(void)
{
	return m_chassisActor;
}


vehicle_physics* shared_vehicle::getSuspension(void)
{
	return m_suspension;
}



void shared_vehicle::addWheels(const vec3_m& extents)
{
	// Wheel local frame.
	const vec3_m wheelDirectionCS(0.0f, -1.0f, 0.0f);
	const vec3_m wheelAxleCS(0.0f, 0.0f, -1.0f);
	const vec3_m wheelForwardCS(1.0f, 0.0f, 0.0f);

	const float suspension_rest_length = 0.6f;
	const float wheel_width = 0.4f;
	const float wheel_radius = 0.9f;

	// This assumes that the vehicle is centered at the origin and a right handed coordinate system is used.
	const vec3_m wheel_connection_point(-(extents[0] - wheel_radius), -suspension_rest_length, (extents[2] - wheel_width));
	m_suspension->add_wheel(wheel_connection_point, wheelDirectionCS, wheelAxleCS, suspension_rest_length, wheel_radius, true);
	m_suspension->add_wheel(wheel_connection_point * vec3_m(1.0f, 1.0f, -1.0f), wheelDirectionCS, wheelAxleCS, suspension_rest_length, wheel_radius, true);
	m_suspension->add_wheel(wheel_connection_point * vec3_m(-1.0f, 1.0f, 1.0f), wheelDirectionCS, wheelAxleCS, suspension_rest_length, wheel_radius, false);
	m_suspension->add_wheel(wheel_connection_point * vec3_m(-1.0f, 1.0f, -1.0f), wheelDirectionCS, wheelAxleCS, suspension_rest_length, wheel_radius, false);

	for (int i = 0; i < m_suspension->get_wheel_count(); ++i) {
		// shapes
		m_wheelShape[i] = m_physics->create_capsule(wheel_radius, wheel_width);

		// raycasts
		vehicle_suspension *wheel = m_suspension->get_wheel(i);
		wheel->m_suspension_stiffness = 7.0f;
		wheel->m_suspension_damping = 0.6f * btSqrt(wheel->m_suspension_stiffness);
		wheel->m_friction_slip = 1.2f;
		wheel->m_roll_influence = 1.0f;
		wheel->m_wheel_radius = wheel_radius;

		// rigid bodies
		placement_m trans; // identity
		m_wheelActor[i] = m_physics->create_new_actor(trans);
		m_wheelActor[i]->attach_shape(m_wheelShape[i], trans);
		m_wheelActor[i]->set_kinematic();
		m_scene->insert(m_wheelActor[i], COL_WHEEL, COL_NOTHING);
	}
}




void shared_vehicle::updateWheels(void)
{
	for (int i = 0; i < m_suspension->get_wheel_count(); ++i) {
		if (m_wheelActor[i]) {
			const vehicle_suspension *wheel = m_suspension->get_wheel(i);
			placement_m trans = wheel->m_worldTransform;
			trans.p -= vec3_m(0.0f, wheel->m_wheel_radius / 2.0f, 0.0f);
			m_wheelActor[i]->set_placement(trans);
		}
	}
}
