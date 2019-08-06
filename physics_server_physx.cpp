#include <physics_server_physx.h>
#include <physics_scene_physx.h>
#include <physics_actor_physx.h>
#include <physics_shape_physx.h>
#include <physics_joint_physx.h>

#include <physics_globals.h>


physics_server_physx::physics_server_physx(const float timestep)
	: physics_server(timestep)
{
	mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, mAllocator, mErrorCallback);
	mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, PxTolerancesScale(), true, NULL);
	mCooking = PxCreateCooking(PX_PHYSICS_VERSION, *mFoundation, PxCookingParams(PxTolerancesScale()));
}

physics_server_physx::~physics_server_physx(void)
{
	mCooking->release();
	mPhysics->release();
	mFoundation->release();
}

physics_joint *physics_server_physx::create_joint_6dof(physics_actor *const actor1, physics_actor *const actor2, const placement_m &frameInA, const placement_m &frameInB)
{
	const PxTransform px_frameInA = to_pxTransform(frameInA);
	const PxTransform px_frameInB = to_pxTransform(frameInB);
	PxRigidActor *const px_actor1 = static_cast<PxRigidActor*>(actor1->getBody());
	PxRigidActor *const px_actor2 = static_cast<PxRigidActor*>(actor2->getBody());
	PxD6Joint *joint_body = PxD6JointCreate(*mPhysics, px_actor1, px_frameInA, px_actor2, px_frameInB);
	physics_joint* joint = new physics_joint_physx();
	joint->set_body(joint_body);

	joint_body->setMotion(PxD6Axis::eSWING1, PxD6Motion::eFREE);
	joint_body->setMotion(PxD6Axis::eSWING2, PxD6Motion::eFREE);
	joint_body->setMotion(PxD6Axis::eTWIST, PxD6Motion::eFREE);
	joint_body->setDrive(PxD6Drive::eSLERP, PxD6JointDrive(0, 1000, FLT_MAX, true));
	joint_body->setConstraintFlag(PxConstraintFlag::eVISUALIZATION, true);

	return joint;
}

physics_actor* physics_server_physx::create_new_actor(const placement_m &transform)
{
	const PxTransform px_transform = to_pxTransform(transform);
	PxRigidDynamic *const actor_body = mPhysics->createRigidDynamic(px_transform);
	physics_actor *const actor = new physics_actor_physx();
	actor->setBody(actor_body);
	actor_body->userData = actor;
	actor_body->setMaxDepenetrationVelocity(1.0f);
	return actor;
}

physics_actor *physics_server_physx::new_actor_static(const placement_m &transform)
{
	const PxTransform physx_transform = to_pxTransform(transform);
	PxRigidStatic *const actor_body = mPhysics->createRigidStatic(physx_transform);
	physics_actor *const actor = new physics_actor_physx();
	actor->setBody(actor_body);
	actor_body->userData = actor;
	return actor;
}

physics_shape *physics_server_physx::create_plane(const vec3_m &normal, const float dist)
{
	const PxPlaneGeometry geom;
	
	const PxMaterial *const material = mPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	PxShape *const shape_body = mPhysics->createShape(geom, *material);
	physics_shape *const shape = new physics_shape_physx();
	shape->set_body(shape_body);
	return shape;
}

physics_shape* physics_server_physx::create_heightfield(const int width, const int length, void *const data, const float minHeight, const float maxHeight)
{
	PxHeightFieldDesc hfDesc;
	hfDesc.format = PxHeightFieldFormat::eS16_TM;
	hfDesc.nbColumns = width;
	hfDesc.nbRows = length;
	hfDesc.samples.data = data;
	hfDesc.samples.stride = sizeof(PxHeightFieldSample);
	PxHeightField *const hf = mCooking->createHeightField(hfDesc, mPhysics->getPhysicsInsertionCallback());

	const PxHeightFieldGeometry geom(hf, PxMeshGeometryFlags(), 1, 50, 50);
	const PxMaterial *const material = mPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	PxShape *const shape_body = mPhysics->createShape(geom, *material);
	physics_shape *const shape = new physics_shape_physx();
	shape->set_body(shape_body);

	return shape;
}

physics_shape *physics_server_physx::create_box(const vec3_m &halfExtents)
{
	const PxVec3 pxExtents = to_pxVec3(halfExtents);
	const PxBoxGeometry geom(pxExtents);
	const PxMaterial *const material = mPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	PxShape *const shape_body = mPhysics->createShape(geom, *material);
	physics_shape *const shape = new physics_shape_physx();
	shape->set_body(shape_body);
	return shape;
}

physics_shape *physics_server_physx::create_capsule(const float radius, const float width)
{
	const PxCapsuleGeometry geom(radius, width);
	const PxMaterial *const material = mPhysics->createMaterial(0.5f, 0.5f, 0.6f);
	const PxTransform relativePose(PxQuat(PxHalfPi, PxVec3(0, 0, 1)));

	PxShape *const shape_body = mPhysics->createShape(geom, *material);
	physics_shape *const shape = new physics_shape_physx();
	shape->set_body(shape_body);
	return shape;
}

physics_shape *physics_server_physx::create_sphere(const float radius)
{
	return nullptr;
}

physics_shape *physics_server_physx::create_compound(void)
{
	return nullptr;
}

void *physics_server_physx::get_body(void)
{
	void *const ret = static_cast<void*>(mPhysics);
	return ret;
}

physics_scene *physics_server_physx::create_scene(void)
{
	physics_scene* scene = new physics_scene_physx(this);
	scenes.push_back(scene);
	return scene;
}

void physics_server_physx::physics_tick(void)
{
	for (std::vector<physics_scene*>::iterator it = scenes.begin(); it != scenes.end(); ++it) {
		(*it)->physics_tick(timestep);
	}
}