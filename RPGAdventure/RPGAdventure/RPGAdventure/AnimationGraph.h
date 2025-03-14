#pragma once

#include <any>
#include <vector>
#include <functional>

#include "SpriteAnimation.h"

class AnimationGraph {

public:

	int currentAnimationPlaying = -1;

	std::vector<SpriteAnimation> animations;
	std::vector<std::function<int(std::any&)>> transitionConditionFunctions;

};

int ShouldAnimationTransition(AnimationGraph& animationGraph, std::any& parms)
{
	return animationGraph.transitionConditionFunctions[animationGraph.currentAnimationPlaying](parms);
}

void UpdateAnimationGraphCurrentAnimation(AnimationGraph& animationGraph)
{
	UpdateAnimation(animationGraph.animations[animationGraph.currentAnimationPlaying]);
}