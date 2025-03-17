#pragma once

#include <iostream>

#include "raylib/raylib.h"

#include "Character.h"
#include "AnimationGraph.h"

struct Goblin {

public:
	Vector2 movingDirection = Vector2{ 1.0f, 0.0f };
};

const unsigned int torchGoblin_CharacterSpriteSheet_idle_animation_yPos = 0;
const unsigned int torchGoblin_CharacterSpriteSheet_running_animation_yPos = 1;
const unsigned int torchGoblin_CharacterSpriteSheet_attack_side_animation_yPos = 2;
const unsigned int torchGoblin_CharacterSpriteSheet_attack_down_animation_yPos = 3;
const unsigned int torchGoblin_CharacterSpriteSheet_attack_up_animation_yPos = 4;

#pragma region Running And Idle Animation States
int TorchGoblinIdleToOthersAnimationChangeRule(std::any& parms)
{
	CharacterStates characterStates = std::any_cast<CharacterStates>(parms);
	bool isCharacterAttacking = IsCharacterAttacking(characterStates);

	if (!isCharacterAttacking && characterStates.running) {
		return torchGoblin_CharacterSpriteSheet_running_animation_yPos;
	}
	else if (isCharacterAttacking) {
		if (characterStates.attackingSide) {
			return torchGoblin_CharacterSpriteSheet_attack_side_animation_yPos;
		}
		else if (characterStates.attackingDown) {
			return torchGoblin_CharacterSpriteSheet_attack_down_animation_yPos;
		}
		else if (characterStates.attackingUp) {
			return torchGoblin_CharacterSpriteSheet_attack_up_animation_yPos;
		}
		else {
			return -1;
		}
	}
	else if (characterStates.idle) {
		//std::cout << "Set idle." << std::endl;
		return torchGoblin_CharacterSpriteSheet_idle_animation_yPos;
	}
	else {
		return -1;
	}
}

int TorchGoblinRunToOthersAnimationChangeRule(std::any& parms)
{
	CharacterStates characterStates = std::any_cast<CharacterStates>(parms);
	bool isCharacterAttacking = IsCharacterAttacking(characterStates);
	if (!isCharacterAttacking && characterStates.idle) {
		return torchGoblin_CharacterSpriteSheet_idle_animation_yPos;
	}
	else if (isCharacterAttacking) {
		if (characterStates.attackingSide) {
			return torchGoblin_CharacterSpriteSheet_attack_side_animation_yPos;
		}
		else if (characterStates.attackingDown) {
			return torchGoblin_CharacterSpriteSheet_attack_down_animation_yPos;
		}
		else if (characterStates.attackingUp) {
			return torchGoblin_CharacterSpriteSheet_attack_up_animation_yPos;
		}
		else {
			return -1;
		}
	}
	else if (characterStates.running) {
		return torchGoblin_CharacterSpriteSheet_running_animation_yPos;
	}
	else {
		return -1;
	}
}
#pragma endregion

#pragma region Attacking And Idle Animation States

int TorchGoblinIdleAndSideAttackAnimationChangeRule(std::any& parms)
{
	CharacterStates characterStates = std::any_cast<CharacterStates>(parms);
	if (characterStates.attackingSide) {
		//std::cout << "Keep attack animation playing." << std::endl;
		return torchGoblin_CharacterSpriteSheet_attack_side_animation_yPos;
	}
	else {
		return torchGoblin_CharacterSpriteSheet_idle_animation_yPos;
	}
}

int TorchGoblinIdleAndDownAttackAnimationChangeRule(std::any& parms)
{
	CharacterStates characterStates = std::any_cast<CharacterStates>(parms);
	if (characterStates.attackingDown) {
		return torchGoblin_CharacterSpriteSheet_attack_down_animation_yPos;
	}
	else {
		return torchGoblin_CharacterSpriteSheet_idle_animation_yPos;
	}
}

int TorchGoblinIdleAndUpAttackAnimationChangeRule(std::any& parms)
{
	CharacterStates characterStates = std::any_cast<CharacterStates>(parms);
	if (characterStates.attackingUp) {
		return torchGoblin_CharacterSpriteSheet_attack_up_animation_yPos;
	}
	else {
		return torchGoblin_CharacterSpriteSheet_idle_animation_yPos;
	}
}

#pragma endregion