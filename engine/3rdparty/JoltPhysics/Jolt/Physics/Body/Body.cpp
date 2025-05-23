// SPDX-FileCopyrightText: 2021 Jorrit Rouwe
// SPDX-License-Identifier: MIT

#include <Jolt/Jolt.h>

#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/StateRecorder.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Core/StringTools.h>
#include <Jolt/Core/Profiler.h>
#ifdef JPH_DEBUG_RENDERER
	#include <Jolt/Renderer/DebugRenderer.h>
#endif // JPH_DEBUG_RENDERER

JPH_NAMESPACE_BEGIN

Body Body::sFixedToWorld(false);

Body::Body(bool) :
	mPosition(Vec3::sZero()),
	mRotation(Quat::sIdentity()),
	mShape(new SphereShape(FLT_EPSILON)), // Dummy shape
	mFriction(0.0f),
	mRestitution(0.0f),
	mObjectLayer(cObjectLayerInvalid),
	mMotionType(EMotionType::Static)
{
}

void Body::SetMotionType(EMotionType inMotionType)
{
	if (mMotionType == inMotionType)
		return;

	JPH_ASSERT(inMotionType == EMotionType::Static || mMotionProperties != nullptr, "Body needs to be created with mAllowDynamicOrKinematic set tot true");
	JPH_ASSERT(inMotionType != EMotionType::Static || !IsActive(), "Deactivate body first");

	// Store new motion type
	mMotionType = inMotionType;

	if (mMotionProperties != nullptr)
	{
		// Update cache
		JPH_IF_ENABLE_ASSERTS(mMotionProperties->mCachedMotionType = inMotionType;)

		switch (inMotionType)
		{
		case EMotionType::Static:
			// Stop the object
			mMotionProperties->mLinearVelocity = Vec3::sZero();
			mMotionProperties->mAngularVelocity = Vec3::sZero();
			[[fallthrough]];

		case EMotionType::Kinematic:
			// Cancel forces
			mMotionProperties->mForce = Float3(0, 0, 0);
			mMotionProperties->mTorque = Float3(0, 0, 0);
			break;

		case EMotionType::Dynamic:
			break;
		}
	}
}

void Body::SetAllowSleeping(bool inAllow)									
{ 
	mMotionProperties->mAllowSleeping = inAllow; 
	if (inAllow) 
		ResetSleepTestSpheres();
}

void Body::MoveKinematic(Vec3Arg inTargetPosition, QuatArg inTargetRotation, float inDeltaTime)
{
	JPH_ASSERT(!IsStatic());
	JPH_ASSERT(BodyAccess::sCheckRights(BodyAccess::sPositionAccess, BodyAccess::EAccess::Read)); 

	// Calculate center of mass at end situation
	Vec3 new_com = inTargetPosition + inTargetRotation * mShape->GetCenterOfMass();

	// Calculate delta position and rotation
	Vec3 delta_pos = new_com - mPosition;
	Quat delta_rotation = inTargetRotation * mRotation.Conjugated();

	mMotionProperties->MoveKinematic(delta_pos, delta_rotation, inDeltaTime);
}

void Body::CalculateWorldSpaceBoundsInternal()
{
	mBounds = mShape->GetWorldSpaceBounds(GetCenterOfMassTransform(), Vec3::sReplicate(1.0f));
}

void Body::SetPositionAndRotationInternal(Vec3Arg inPosition, QuatArg inRotation) 
{ 
	JPH_ASSERT(BodyAccess::sCheckRights(BodyAccess::sPositionAccess, BodyAccess::EAccess::ReadWrite)); 

	mPosition = inPosition + inRotation * mShape->GetCenterOfMass();
	mRotation = inRotation; 

	// initialize bounding box
	CalculateWorldSpaceBoundsInternal();

	// Reset sleeping test
	if (mMotionProperties != nullptr)
		ResetSleepTestSpheres();
}

void Body::UpdateCenterOfMassInternal(Vec3Arg inPreviousCenterOfMass, bool inUpdateMassProperties)
{
	// Update center of mass position so the world position for this body stays the same
	mPosition += mRotation * (mShape->GetCenterOfMass() - inPreviousCenterOfMass);

	// Recalculate mass and inertia if requested
	if (inUpdateMassProperties && mMotionProperties != nullptr)
		mMotionProperties->SetMassProperties(mShape->GetMassProperties());
}

void Body::SetShapeInternal(const Shape *inShape, bool inUpdateMassProperties)
{
	JPH_ASSERT(BodyAccess::sCheckRights(BodyAccess::sPositionAccess, BodyAccess::EAccess::ReadWrite)); 

	// Get the old center of mass
	Vec3 old_com = mShape->GetCenterOfMass();

	// Update the shape
	mShape = inShape;

	// Update center of mass
	UpdateCenterOfMassInternal(old_com, inUpdateMassProperties);

	// Recalculate bounding box
	CalculateWorldSpaceBoundsInternal();
}

Body::ECanSleep Body::UpdateSleepStateInternal(float inDeltaTime, float inMaxMovement, float inTimeBeforeSleep)
{
	// Check override & sensors will never go to sleep (they would stop detecting collisions with sleeping bodies)
	if (!mMotionProperties->mAllowSleeping || IsSensor())
		return ECanSleep::CannotSleep;

	// Get the points to test
	Vec3 points[3];
	GetSleepTestPoints(points);

	for (int i = 0; i < 3; ++i)
	{
		Sphere &sphere = mMotionProperties->mSleepTestSpheres[i];

		// Encapsulate the point in a sphere
		sphere.EncapsulatePoint(points[i]);

		// Test if it exceeded the max movement
		if (sphere.GetRadius() > inMaxMovement)
		{
			// Body is not sleeping, reset test
			mMotionProperties->ResetSleepTestSpheres(points);
			return ECanSleep::CannotSleep;
		}
	}

	mMotionProperties->mSleepTestTimer += inDeltaTime;
	return mMotionProperties->mSleepTestTimer >= inTimeBeforeSleep? ECanSleep::CanSleep : ECanSleep::CannotSleep;
}

bool Body::ApplyBuoyancyImpulse(const Plane &inSurface, float inBuoyancy, float inLinearDrag, float inAngularDrag, Vec3Arg inFluidVelocity, Vec3Arg inGravity, float inDeltaTime)
{
	JPH_PROFILE_FUNCTION();

	// We follow the approach from 'Game Programming Gems 6' 2.5 Exact Buoyancy for Polyhedra
	// All quantities below are in world space

	// Calculate amount of volume that is submerged and what the center of buoyancy is
	float total_volume, submerged_volume;
	Vec3 center_of_buoyancy;
	mShape->GetSubmergedVolume(GetCenterOfMassTransform(), Vec3::sReplicate(1.0f), inSurface, total_volume, submerged_volume, center_of_buoyancy);
		
	// If we're not submerged, there's no point in doing the rest of the calculations
	if (submerged_volume > 0.0f)
	{
	#ifdef JPH_DEBUG_RENDERER
		// Draw submerged volume properties
		if (Shape::sDrawSubmergedVolumes)
		{
			DebugRenderer::sInstance->DrawMarker(center_of_buoyancy, Color::sWhite, 2.0f);
			DebugRenderer::sInstance->DrawText3D(center_of_buoyancy, StringFormat("%.3f / %.3f", (double)submerged_volume, (double)total_volume));
		}
	#endif // JPH_DEBUG_RENDERER

		// When buoyancy is 1 we want neutral buoyancy, this means that the density of the liquid is the same as the density of the body at that point.
		// Buoyancy > 1 should make the object float, < 1 should make it sink.
		float inverse_mass = mMotionProperties->GetInverseMass();
		float fluid_density = inBuoyancy / (total_volume * inverse_mass);

		// Buoyancy force = Density of Fluid * Submerged volume * Magnitude of gravity * Up direction (eq 2.5.1)
		// Impulse = Force * Delta time
		// We should apply this at the center of buoyancy (= center of mass of submerged volume)
		Vec3 buoyancy_impulse = -fluid_density * submerged_volume * mMotionProperties->GetGravityFactor() * inGravity * inDeltaTime;

		// Calculate the velocity of the center of buoyancy relative to the fluid
		Vec3 relative_center_of_buoyancy = center_of_buoyancy - mPosition;
		Vec3 linear_velocity = mMotionProperties->GetLinearVelocity();
		Vec3 angular_velocity = mMotionProperties->GetAngularVelocity();
		Vec3 center_of_buoyancy_velocity = linear_velocity + angular_velocity.Cross(relative_center_of_buoyancy);
		Vec3 relative_center_of_buoyancy_velocity = inFluidVelocity - center_of_buoyancy_velocity;

		// Here we deviate from the article, instead of eq 2.5.14 we use a quadratic drag formula: https://en.wikipedia.org/wiki/Drag_%28physics%29
		// Drag force = 0.5 * Fluid Density * (Velocity of fluid - Velocity of center of buoyancy)^2 * Linear Drag * Area Facing the Relative Fluid Velocity
		// Again Impulse = Force * Delta Time
		// We should apply this at the center of buoyancy (= center of mass for submerged volume with no center of mass offset)

		// Get size of local bounding box
		Vec3 size = mShape->GetLocalBounds().GetSize();

		// Determine area of the local space bounding box in the direction of the relative velocity between the fluid and the center of buoyancy
		float area = 0.0f;
		float relative_center_of_buoyancy_velocity_len_sq = relative_center_of_buoyancy_velocity.LengthSq();
		if (relative_center_of_buoyancy_velocity_len_sq > 1.0e-12f)
		{
			Vec3 local_relative_center_of_buoyancy_velocity = GetRotation().Conjugated() * relative_center_of_buoyancy_velocity;
			area = local_relative_center_of_buoyancy_velocity.Abs().Dot(size.Swizzle<SWIZZLE_Y, SWIZZLE_Z, SWIZZLE_X>() * size.Swizzle<SWIZZLE_Z, SWIZZLE_X, SWIZZLE_Y>()) / sqrt(relative_center_of_buoyancy_velocity_len_sq);
		}

		// Calculate the impulse
		Vec3 drag_impulse = (0.5f * fluid_density * inLinearDrag * area * inDeltaTime) * relative_center_of_buoyancy_velocity * relative_center_of_buoyancy_velocity.Length();

		// Clamp magnitude against current linear velocity to prevent overshoot
		float linear_velocity_len_sq = linear_velocity.LengthSq();
		float drag_delta_linear_velocity_len_sq = (drag_impulse * inverse_mass).LengthSq();
		if (drag_delta_linear_velocity_len_sq > linear_velocity_len_sq)
			drag_impulse *= sqrt(linear_velocity_len_sq / drag_delta_linear_velocity_len_sq);

		// Calculate the resulting delta linear velocity due to buoyancy and drag
		Vec3 delta_linear_velocity = (drag_impulse + buoyancy_impulse) * inverse_mass;
		mMotionProperties->AddLinearVelocityStep(delta_linear_velocity);

		// Determine average width of the body (across the three axis)
		float l = (size.GetX() + size.GetY() + size.GetZ()) / 3.0f;

		// Drag torque = -Angular Drag * Mass * Submerged volume / Total volume * (Average width of body)^2 * Angular velocity (eq 2.5.15)
		Vec3 drag_angular_impulse = (-inAngularDrag * submerged_volume / total_volume * inDeltaTime * Square(l) / inverse_mass) * angular_velocity;
		Mat44 inv_inertia = GetInverseInertia();
		Vec3 drag_delta_angular_velocity = inv_inertia * drag_angular_impulse;

		// Clamp magnitude against the current angular velocity to prevent overshoot
		float angular_velocity_len_sq = angular_velocity.LengthSq();
		float drag_delta_angular_velocity_len_sq = drag_delta_angular_velocity.LengthSq();
		if (drag_delta_angular_velocity_len_sq > angular_velocity_len_sq)
			drag_delta_angular_velocity *= sqrt(angular_velocity_len_sq / drag_delta_angular_velocity_len_sq);

		// Calculate total delta angular velocity due to drag and buoyancy
		Vec3 delta_angular_velocity = drag_delta_angular_velocity + inv_inertia * relative_center_of_buoyancy.Cross(buoyancy_impulse + drag_impulse);
		mMotionProperties->AddAngularVelocityStep(delta_angular_velocity);
		return true;
	}

	return false;
}

void Body::SaveState(StateRecorder &inStream) const
{
	// Only write properties that can change at runtime
	inStream.Write(mPosition);
	inStream.Write(mRotation);
	inStream.Write(mFriction);
	inStream.Write(mRestitution);
	mCollisionGroup.SaveBinaryState(inStream);
	inStream.Write(mMotionType);

	if (mMotionProperties != nullptr)
		mMotionProperties->SaveState(inStream);
}

void Body::RestoreState(StateRecorder &inStream)
{
	inStream.Read(mPosition);
	inStream.Read(mRotation);
	inStream.Read(mFriction);
	inStream.Read(mRestitution);
	mCollisionGroup.RestoreBinaryState(inStream);
	inStream.Read(mMotionType);

	if (mMotionProperties != nullptr)
	{
		mMotionProperties->RestoreState(inStream);
		JPH_IF_ENABLE_ASSERTS(mMotionProperties->mCachedMotionType = mMotionType);
	}

	// initialize bounding box
	CalculateWorldSpaceBoundsInternal();
}

BodyCreationSettings Body::GetBodyCreationSettings() const
{
	BodyCreationSettings result;

	result.mPosition = GetPosition();
	result.mRotation = GetRotation();
	result.mLinearVelocity = mMotionProperties != nullptr? mMotionProperties->GetLinearVelocity() : Vec3::sZero();
	result.mAngularVelocity = mMotionProperties != nullptr? mMotionProperties->GetAngularVelocity() : Vec3::sZero();
	result.mObjectLayer = GetObjectLayer();
	result.mCollisionGroup = GetCollisionGroup();
	result.mMotionType = GetMotionType();
	result.mAllowDynamicOrKinematic = mMotionProperties != nullptr;
	result.mIsSensor = IsSensor();
	result.mMotionQuality = mMotionProperties != nullptr? mMotionProperties->GetMotionQuality() : EMotionQuality::Discrete;
	result.mAllowSleeping = mMotionProperties != nullptr? GetAllowSleeping() : true;
	result.mFriction = GetFriction();
	result.mRestitution = GetRestitution();
	result.mLinearDamping = mMotionProperties != nullptr? mMotionProperties->GetLinearDamping() : 0.0f;
	result.mAngularDamping = mMotionProperties != nullptr? mMotionProperties->GetAngularDamping() : 0.0f;
	result.mMaxLinearVelocity = mMotionProperties != nullptr? mMotionProperties->GetMaxLinearVelocity() : 0.0f;
	result.mMaxAngularVelocity = mMotionProperties != nullptr? mMotionProperties->GetMaxAngularVelocity() : 0.0f;
	result.mGravityFactor = mMotionProperties != nullptr? mMotionProperties->GetGravityFactor() : 1.0f;
	result.mOverrideMassProperties = EOverrideMassProperties::MassAndInertiaProvided;
	result.mMassPropertiesOverride.mMass = mMotionProperties != nullptr? 1.0f / mMotionProperties->GetInverseMassUnchecked() : FLT_MAX;
	result.mMassPropertiesOverride.mInertia = mMotionProperties != nullptr? mMotionProperties->GetLocalSpaceInverseInertiaUnchecked().Inversed3x3() : Mat44::sIdentity();
	result.SetShape(GetShape());

	return result;
}

JPH_NAMESPACE_END
