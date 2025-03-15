#pragma once

#include "raylib/raylib.h"

#include "Character.h"
#include "AnimationGraph.h"

#pragma region Running And Idle Animation States
int KnightIdleToRunAnimationChangeRule(std::any& parms)
{
	CharacterStates characterStates = std::any_cast<CharacterStates>(parms);
	bool isCharacterAttacking = IsCharacterAttacking(characterStates);

	if (!isCharacterAttacking && characterStates.running) {
		return 1;
	}
	else if (isCharacterAttacking) {
		if (characterStates.attackingSide) {
			return 2;
		}
		else if (characterStates.attackingDown) {
			return 4;
		}
		else if (characterStates.attackingUp) {
			return 6;
		}
		else {
			return -1;
		}
	}
	else if (characterStates.idle) {
		return 0;
	}
	else{
		return -1;
	}
}

int KnightRunToIdleAnimationChangeRule(std::any& parms)
{
	CharacterStates characterStates = std::any_cast<CharacterStates>(parms);
	bool isCharacterAttacking = IsCharacterAttacking(characterStates);
	if (!isCharacterAttacking && characterStates.idle) {
		return 0;
	}
	else if (isCharacterAttacking) {
		if (characterStates.attackingSide) {
			return 2;
		}
		else if (characterStates.attackingDown) {
			return 4;
		}
		else if (characterStates.attackingUp) {
			return 6;
		}
		else {
			return -1;
		}
	}
	else if (characterStates.running) {
		return 1;
	}
	else {
		return -1;
	}
}
#pragma endregion

#pragma region Attacking And Idle Animation States

int KnightIdleToSideAttackAnimationChangeRule(std::any& parms)
{
	CharacterStates characterStates = std::any_cast<CharacterStates>(parms);
	if (characterStates.attackingSide) {
		//std::cout << "Keep attack animation playing." << std::endl;
		return 2;
	}
	else {
		return -1;
	}
}

int KnightIdleToDownAttackAnimationChangeRule(std::any& parms)
{
	CharacterStates characterStates = std::any_cast<CharacterStates>(parms);
	if (characterStates.attackingDown) {
		return 4;
	}
	else {
		return -1;
	}
}

int KnightIdleToUpAttackAnimationChangeRule(std::any& parms)
{
	CharacterStates characterStates = std::any_cast<CharacterStates>(parms);
	if (characterStates.attackingUp) {
		return 6;
	}
	else {
		return -1;
	}
}

int KnightSideAttackToIdleAnimationChangeRule(std::any& parms)
{
	CharacterStates characterStates = std::any_cast<CharacterStates>(parms);
	if (!characterStates.attackingSide) {
		return 0;
	}
	else {
		return -1;
	}
}

int KnightUpAttackToIdleAnimationChangeRule(std::any& parms)
{
	CharacterStates characterStates = std::any_cast<CharacterStates>(parms);
	if (!characterStates.attackingSide) {
		return 0;
	}
	else {
		return -1;
	}
}

int KnightDownAttackToIdleAnimationChangeRule(std::any& parms)
{
	CharacterStates characterStates = std::any_cast<CharacterStates>(parms);
	if (!characterStates.attackingSide) {
		return 0;
	}
	else {
		return -1;
	}
}
#pragma endregion