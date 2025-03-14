#pragma once

#include "Position.h"
#include "Velocity.h"

class Character {

public:
    Position position = { 0 };
    Velocity velocity = { 0 };
    Vector2 facingDirection = { 0 };
};

class CharacterStates {

public:
    bool idle = false;
    bool running = false;
    bool attackingUp = false;
    bool attackingDown = false;
    bool attackingSide = false;
};