#pragma once

#include <iostream>

#include "SpriteSheet.h"
#include "raylib/raymath.h"

class SpriteAnimation {

public:

	unsigned int startFrameX			= 0;
	unsigned int endFrameX				= 0;

	unsigned int curAnimationStateY		= 0;

	unsigned int numFramesPerSecond		= 0;
	Vector2 frameStepSize				= Vector2{ 1, 0 };

	unsigned int currentFramePlaying	= 0;
	float timeSinceLastFrameUpdate		= 0.0f;

	float timeBetweenEachFrameUpdate	= 0.0f;

	bool loop							= true;
	bool loopFinished					= false;

	Rectangle curFrameView				= { 0.0f, 0.0f, 0.0f, 0.0f };
};

void InitSpriteAnimation(SpriteAnimation& spriteAnimation, unsigned int _startFrameX, unsigned int _endFrameX, unsigned int _numFramesPerSecond, bool _loop, SpriteSheet& _curSpriteSheet) {
	spriteAnimation.startFrameX = _startFrameX;
	spriteAnimation.endFrameX = _endFrameX;

	//spriteAnimation.startFrameY = _startFrameY;

	spriteAnimation.numFramesPerSecond = _numFramesPerSecond;

	spriteAnimation.currentFramePlaying = spriteAnimation.startFrameX;

	spriteAnimation.timeBetweenEachFrameUpdate = 1.0f / (float)spriteAnimation.numFramesPerSecond;
	spriteAnimation.timeSinceLastFrameUpdate = spriteAnimation.timeBetweenEachFrameUpdate + 0.1f;

	spriteAnimation.loop = _loop;

	spriteAnimation.curFrameView = _curSpriteSheet.cell;
}

void UpdateAnimation(SpriteAnimation& spriteAnimation) {

	if (spriteAnimation.timeSinceLastFrameUpdate >= spriteAnimation.timeBetweenEachFrameUpdate) {

		//std::cout << "Updated animation." << std::endl;

		spriteAnimation.currentFramePlaying++;

		if (spriteAnimation.currentFramePlaying == spriteAnimation.startFrameX) {
			spriteAnimation.loopFinished = false;
		}
		else if (spriteAnimation.currentFramePlaying > spriteAnimation.endFrameX) {
			spriteAnimation.loopFinished = true;
			spriteAnimation.currentFramePlaying = spriteAnimation.loop ? spriteAnimation.startFrameX : spriteAnimation.endFrameX;
		}

		spriteAnimation.curFrameView.y = spriteAnimation.curAnimationStateY * spriteAnimation.curFrameView.height;

		spriteAnimation.curFrameView.x += spriteAnimation.currentFramePlaying * spriteAnimation.frameStepSize.x * spriteAnimation.curFrameView.width;
		spriteAnimation.curFrameView.y += spriteAnimation.currentFramePlaying * spriteAnimation.frameStepSize.y * spriteAnimation.curFrameView.height;

		spriteAnimation.timeSinceLastFrameUpdate = 0.0f;
	}
	else {
		spriteAnimation.timeSinceLastFrameUpdate += GetFrameTime();
		spriteAnimation.loopFinished = false;
	}

}
