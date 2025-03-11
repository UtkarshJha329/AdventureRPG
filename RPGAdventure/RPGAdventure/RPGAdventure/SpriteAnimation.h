#pragma once

#include <iostream>

#include "SpriteSheet.h"
#include "raylib/raymath.h"

class SpriteAnimation {

public:

	unsigned int startFrame				= 0;
	unsigned int endFrame				= 0;

	unsigned int numFramesPerSecond		= 0;
	Vector2 frameStepSize				= Vector2{ 1, 0 };

	unsigned int currentFramePlaying	= 0;
	float timeSinceLastFrameUpdate		= 0.0f;

	float timeBetweenEachFrameUpdate	= 0.0f;

	bool loop							= true;

	Rectangle curFrameView				= { 0.0f, 0.0f, 0.0f, 0.0f };
};

void InitSpriteAnimation(SpriteAnimation& spriteAnimation, unsigned int _startFrame, unsigned int _endFrame, unsigned int _numFramesPerSecond, bool _loop, SpriteSheet& _curSpriteSheet) {
	spriteAnimation.startFrame = _startFrame;
	spriteAnimation.endFrame = _endFrame;
	spriteAnimation.numFramesPerSecond = _numFramesPerSecond;

	spriteAnimation.currentFramePlaying = spriteAnimation.startFrame;

	spriteAnimation.timeBetweenEachFrameUpdate = 1.0f / (float)spriteAnimation.numFramesPerSecond;
	spriteAnimation.timeSinceLastFrameUpdate = spriteAnimation.timeBetweenEachFrameUpdate + 0.1f;

	spriteAnimation.loop = _loop;

	spriteAnimation.curFrameView = _curSpriteSheet.cell;
}

void UpdateAnimation(SpriteAnimation& spriteAnimation) {

	if (spriteAnimation.timeSinceLastFrameUpdate >= spriteAnimation.timeBetweenEachFrameUpdate) {
		spriteAnimation.currentFramePlaying++;

		if (spriteAnimation.currentFramePlaying > spriteAnimation.endFrame) {
			spriteAnimation.currentFramePlaying = spriteAnimation.loop ? spriteAnimation.startFrame : spriteAnimation.endFrame;
		}

		spriteAnimation.curFrameView.x += spriteAnimation.currentFramePlaying * spriteAnimation.frameStepSize.x * spriteAnimation.curFrameView.width;
		spriteAnimation.curFrameView.y += spriteAnimation.currentFramePlaying * spriteAnimation.frameStepSize.y * spriteAnimation.curFrameView.height;

		spriteAnimation.timeSinceLastFrameUpdate = 0.0f;
	}
	else {
		spriteAnimation.timeSinceLastFrameUpdate += GetFrameTime();
	}

}
