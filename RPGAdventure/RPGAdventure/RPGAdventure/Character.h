#pragma once

#include "Position.h"
#include "Velocity.h"

class Character {

public:
    Position position = { 0 };
    Velocity velocity = { 0 };
    Vector2 facingDirection = { 0 };

    Rectangle attackSideOverlapRect;
    Rectangle attackUpOverlapRect;
    Rectangle attackDownOverlapRect;
    
    float attackTime = 1.0f;
    float nextAttackTime = 0.0f;

    float health = 1.0f;

    bool attackDealtWith = true;
};

class CharacterStates {

public:
    bool idle = false;
    bool running = false;
    bool attackingUp = false;
    bool attackingDown = false;
    bool attackingSide = false;
};

bool IsCharacterAttacking(CharacterStates& characterStates) {
    return characterStates.attackingUp || characterStates.attackingDown || characterStates.attackingSide;
}