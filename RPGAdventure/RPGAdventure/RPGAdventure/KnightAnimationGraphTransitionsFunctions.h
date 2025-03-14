#pragma once

#include "raylib/raylib.h"

#include "Velocity.h"
#include "AnimationGraph.h"

int KnightIdleToRunAnimationChangeRule(std::any& parms)
{
	Velocity velocity = std::any_cast<Velocity>(parms);
	if (Vector2Length(velocity.vel) > 0.0f) {
		return 1;
	}
	else {
		return 0;
	}
}

int KnightRunToIdleAnimationChangeRule(std::any& parms)
{
	Velocity velocity = std::any_cast<Velocity>(parms);
	if (Vector2Length(velocity.vel) == 0.0f) {
		return 0;
	}
	else {
		return 1;
	}
}