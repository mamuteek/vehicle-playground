#pragma once

#include <placement.h>
#include <vector.h>
#include <quat.h>

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include "PxPhysicsAPI.h"

enum collisiontypes {
	COL_NOTHING = 0,
	COL_GROUND	= 1 << 0,
	COL_TRUCK	= 1 << 1,
	COL_TRAILER = 1 << 2,
	COL_CABLE	= 1 << 3,
	COL_WHEEL	= 1 << 4,
	COL_ALL		= 31
};


// Vector transforms

static inline physx::PxVec3 to_pxVec3(const vec3_m &vec) {
	return physx::PxVec3(vec[0], vec[1], vec[2]);
}

static inline btVector3 to_btVector3(const vec3_m &vec) {
	return btVector3(vec[0], vec[1], vec[2]);
}

static inline vec3_m to_vec3_m(const physx::PxVec3 &vec) {
	return vec3_m(vec[0], vec[1], vec[2]);
}

static inline vec3_m to_vec3_m(const btVector3 &vec) {
	return vec3_m(vec[0], vec[1], vec[2]);
}

// Quaternion transforms

static inline physx::PxQuat to_pxQuat(const quat_m &quat) {
	return physx::PxQuat(quat.x, quat.y, quat.z, quat.w);
}

static inline btQuaternion to_btQuaternion(const quat_m &quat) {
	return btQuaternion(quat.x, quat.y, quat.z, quat.w);
}

static inline quat_m to_quat_m(const physx::PxQuat &quat) {
	return quat_m(quat.x, quat.y, quat.z, quat.w);
}

static inline quat_m to_quat_m(const btQuaternion &quat) {
	return quat_m(quat.getX(), quat.getY(), quat.getZ(), quat.getW());
}

// Placement transforms

static inline physx::PxTransform to_pxTransform(const placement_m &transform) {
	const physx::PxVec3 px_vec = to_pxVec3(transform.p);
	const physx::PxQuat px_quat = to_pxQuat(transform.q);
	const physx::PxTransform physx_transform(px_vec, px_quat);
	return physx_transform;
}

static inline btTransform to_btTransform(const placement_m &transform) {
	const btVector3 bt_vec = to_btVector3(transform.p);
	const btQuaternion bt_quat = to_btQuaternion(transform.q);
	const btTransform bt_transform(bt_quat, bt_vec);
	return bt_transform;
}

static inline placement_m to_placement_m(const physx::PxTransform &transform) {
	const vec3_m vec = to_vec3_m(transform.p);
	const quat_m quat = to_quat_m(transform.q);
	const placement_m placement(vec, quat);
	return placement;
}

static inline placement_m to_placement_m(const btTransform &transform) {
	const vec3_m vec = to_vec3_m(transform.getOrigin());
	const quat_m quat = to_quat_m(transform.getRotation());
	const placement_m placement(vec, quat);
	return placement;
}

