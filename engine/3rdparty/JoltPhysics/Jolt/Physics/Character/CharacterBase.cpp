// SPDX-FileCopyrightText: 2021 Jorrit Rouwe
// SPDX-License-Identifier: MIT

#include <Jolt/Jolt.h>

#include <Jolt/Physics/Character/CharacterBase.h>
#include <Jolt/Physics/StateRecorder.h>

JPH_NAMESPACE_BEGIN

CharacterBase::CharacterBase(const CharacterBaseSettings *inSettings, PhysicsSystem *inSystem) :
	mSystem(inSystem),
	mShape(inSettings->mShape)
{
	// initialize max slope angle
	SetMaxSlopeAngle(inSettings->mMaxSlopeAngle);
}

void CharacterBase::SaveState(StateRecorder &inStream) const
{
	inStream.Write(mGroundState);
	inStream.Write(mGroundBodyID);
	inStream.Write(mGroundBodySubShapeID);
	inStream.Write(mGroundPosition);
	inStream.Write(mGroundNormal);
	inStream.Write(mGroundVelocity);
	// Can't save or restore user data (may be a pointer) and material
}

void CharacterBase::RestoreState(StateRecorder &inStream)
{
	inStream.Read(mGroundState);
	inStream.Read(mGroundBodyID);
	inStream.Read(mGroundBodySubShapeID);
	inStream.Read(mGroundPosition);
	inStream.Read(mGroundNormal);
	inStream.Read(mGroundVelocity);
}

JPH_NAMESPACE_END
