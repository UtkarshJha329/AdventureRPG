#pragma once

#include "flecs/flecs.h"

#include "BoundingBox2D.h"
#include "TileMap.h"
#include "TileMapData.h"

#include "TorchGoblinAnimationGraphTransitionFunctions.h"

bool IsMovementDirectionArrow(char c) {
    return c == '^' || c == 'V' || c == '>' || c == '<';
}

Vector2 EnemyPartolDirection(TileMapData& tmd, int enemyTileMapDataX, int enemyTileMapDataY) {

    // REMEMBER WE ADD 2.0f on the X AXIS BECAUSE THERE IS SPACE IN BETWEEN TWO TILES ON THE X AXIS!!!!!

    Vector2 up = Vector2{ (float)enemyTileMapDataX, (float)enemyTileMapDataY + 1.0f };
    Vector2 down = Vector2{ (float)enemyTileMapDataX, (float)enemyTileMapDataY - 1.0f };
    Vector2 right = Vector2{ (float)enemyTileMapDataX + 2.0f, (float)enemyTileMapDataY };
    Vector2 left = Vector2{ (float)enemyTileMapDataX - 2.0f, (float)enemyTileMapDataY };

    if (tmd.enemyTileMapData[up.y][up.x] == 'V') {
        return Vector2{ 0.0f, 1.0f };
    }
    else if (tmd.enemyTileMapData[down.y][down.x] == '^') {
        return Vector2{ 0.0f, -1.0f };
    }
    else if (tmd.enemyTileMapData[right.y][right.x] == '>') {
        return Vector2{ 1.0f, 0.0f };
    }
    else if (tmd.enemyTileMapData[left.y][left.x] == '<') {
        return Vector2{ -1.0f, 0.0f };
    }
}

void SpawnGoblins(flecs::world& world, TileMapData& tmd, TileMap& tm, Camera2D& camera) {

    int tilesY = 7;
    int totalY = 70;
    //tm.tileTextureIndexData = std::vector<float2>(totalTilesX * totalY, { 0.0f, 0.0f });

    int torchGoblin = 0;
    for (int y = 0; y < totalY; y++)
    {
        //tm.tiles[x].reserve(tm.sizeY);
        for (int x = 0; x < totalTilesX; x++)
        {

            int tileMapDataReadingX = x * 2;
            int tileMapDataReadingY = y;

            if (tmd.enemyTileMapData[tileMapDataReadingY][tileMapDataReadingX] == 'T') {

                //std::cout << "Added torch goblin." << std::endl;

                int roomIndexX = x / numTilesX;
                int roomIndexY = y / tilesY;

                std::string goblinName = "Torch Goblin (" + std::to_string(torchGoblin) + ")";
                auto e_torchGoblinEntity = world.entity(goblinName.c_str());
                e_torchGoblinEntity.add<Goblin>();
                e_torchGoblinEntity.add<Character>();
                e_torchGoblinEntity.add<CharacterStates>();
                e_torchGoblinEntity.add<SpriteSheet>();
                e_torchGoblinEntity.add<BoundingBox2D>();
                e_torchGoblinEntity.add<TextureResource>();
                e_torchGoblinEntity.add<AnimationGraph>();

                e_torchGoblinEntity.set<TextureResource>({ torchGoblin_CharacterSpriteSheetLocation, torchGoblin_CharacterSpriteSheet_numSpriteCellsX, torchGoblin_CharacterSpriteSheet_numSpriteCellsY, torchGoblin_CharacterSpriteSheet_paddingX, torchGoblin_CharacterSpriteSheet_paddingY });

                AnimationGraph* goblinAnimationGraph = e_torchGoblinEntity.get_mut<AnimationGraph>();
                goblinAnimationGraph->transitionConditionFunctions.push_back(TorchGoblinIdleToOthersAnimationChangeRule);
                goblinAnimationGraph->transitionConditionFunctions.push_back(TorchGoblinRunToOthersAnimationChangeRule);
                goblinAnimationGraph->transitionConditionFunctions.push_back(TorchGoblinIdleAndSideAttackAnimationChangeRule);
                goblinAnimationGraph->transitionConditionFunctions.push_back(TorchGoblinIdleAndDownAttackAnimationChangeRule);
                goblinAnimationGraph->transitionConditionFunctions.push_back(TorchGoblinIdleAndUpAttackAnimationChangeRule);

                Character* goblin = e_torchGoblinEntity.get_mut<Character>();

                goblin->attackTime = 2.0f;
                goblin->health = 2.0f;

                int numTilesYRounded = 7;
                Vector2 tileRoomIndex = Vector2{ x / numTilesX, y / (float)numTilesYRounded };
                //Vector2 offsetByTiles = Vector2{ ((tileRoomIndex.x * numTilesX) + x) * tileScaleX, ((tileRoomIndex.y * numTilesYRounded) + y) * tileScaleY } + Vector2{ tileScaleX * 0.5f, tileScaleY * 0.25f };
                Vector2 offsetByTiles = Vector2{ (x) * tileScaleX, (y) * tileScaleY } + Vector2{ tileScaleX * 0.5f, tileScaleY * 0.25f };
                Vector2 worldDistanceToOffsetBy = GetScreenToWorld2D(offsetByTiles, camera);

                //std::cout << worldDistanceToOffsetBy.x << ", " << worldDistanceToOffsetBy.y << std::endl;

                goblin->position.pos = Vector2{ worldDistanceToOffsetBy.x, worldDistanceToOffsetBy.y };
                goblin->facingDirection = Vector2{ 1.0f, 1.0f };

                Goblin* gob_mut = e_torchGoblinEntity.get_mut<Goblin>();
                //gob_mut->movingDirection = Vector2{ (float)GetRandomValue(-1, 1), (float)GetRandomValue(-1, 1) };
                gob_mut->movingDirection = EnemyPartolDirection(tmd, tileMapDataReadingX, tileMapDataReadingY);
                //std::cout << tileMapDataReadingX << ", " << tileMapDataReadingY << " :=  " << gob_mut->movingDirection.x << ", " << gob_mut->movingDirection.y << std::endl;
                goblin->facingDirection = gob_mut->movingDirection;
                //int setRandToZero = GetRandomValue(0, 1);
                //if (setRandToZero == 0) {
                //    gob_mut->movingDirection.x = 0;
                //}
                //else {
                //    gob_mut->movingDirection.y = 0;
                //}

                //if (gob_mut->movingDirection.x == 0 && gob_mut->movingDirection.y == 0) {
                //    gob_mut->movingDirection = Vector2{ 1.0f, 0.0f };
                //}

                tm.roomsData[roomIndexY][roomIndexX].torchGoblinEntitiesInThisRoom.push_back(e_torchGoblinEntity);
                torchGoblin++;
            }
        }
    }
}

void MakeGoblinsMoveIt(flecs::world& world, TileMap& tm, Vector2 roomIndex, Vector2 playerPos, float speed, float deltaTime, Camera2D& camera) {

    int numTorchGoblinsInThisRoom = tm.roomsData[roomIndex.y][roomIndex.x].torchGoblinEntitiesInThisRoom.size();
    for (int i = 0; i < numTorchGoblinsInThisRoom; i++)
    {
        Goblin* goblin_mut = tm.roomsData[roomIndex.y][roomIndex.x].torchGoblinEntitiesInThisRoom[i].get_mut<Goblin>();
        Character* goblinCharacter_mut = tm.roomsData[roomIndex.y][roomIndex.x].torchGoblinEntitiesInThisRoom[i].get_mut<Character>();
        CharacterStates* goblinCharacterStates_mut = tm.roomsData[roomIndex.y][roomIndex.x].torchGoblinEntitiesInThisRoom[i].get_mut<CharacterStates>();
        AnimationGraph* goblinAnimationGraph_mut = tm.roomsData[roomIndex.y][roomIndex.x].torchGoblinEntitiesInThisRoom[i].get_mut<AnimationGraph>();

        Vector2 goblinDirToPlayer = Vector2Subtract(playerPos, goblinCharacter_mut->position.pos);
        float distanceToPlayer = Vector2Length(goblinDirToPlayer);
        goblinDirToPlayer = Vector2Normalize(goblinDirToPlayer);

        goblinCharacter_mut->facingDirection = goblinDirToPlayer * Vector2{ 1 / abs(goblinDirToPlayer.x), 1 / abs(goblinDirToPlayer.y) };

        bool finishedPlayingAttackAnimaiton = IsCharacterAttacking(*goblinCharacterStates_mut)
                                            && (goblinAnimationGraph_mut->animations[torchGoblin_CharacterSpriteSheet_attack_side_animation_yPos].loopFinished);
        //bool finishedPlayingAttackAnimaiton = IsCharacterAttacking(*goblinCharacterStates_mut)
        //                                    && (goblinAnimationGraph_mut->animations[torchGoblin_CharacterSpriteSheet_attack_side_animation_yPos].currentFramePlaying
        //                                        == goblinAnimationGraph_mut->animations[torchGoblin_CharacterSpriteSheet_attack_side_animation_yPos].endFrameX);

        //if (finishedPlayingAttackAnimaiton) {
        //    std::cout << "FINISHED PLAYING ANIMATION!!!" << std::endl;
        //}
        //else {
        //    std::cout << "playing animation." << std::endl;
        //}

        if (distanceToPlayer >= 200.0f && !IsCharacterAttacking(*goblinCharacterStates_mut)) {

            Vector2 goblinVel = goblin_mut->movingDirection * speed;
            Vector2 curFrameMovement = goblinVel * deltaTime;
            Vector2 goblinNextPos = goblinCharacter_mut->position.pos + curFrameMovement * 4.0f;

            Vector2 cameraScreenPos = GetWorldToScreen2D(camera.target, camera);
            Vector2 curFrameMovementInPixels = GetWorldToScreen2D(curFrameMovement, camera);
            Vector2 curRoomIndex = CurrentRoomIndex(GetWorldToScreen2D(goblinNextPos, camera), cameraScreenPos, curFrameMovementInPixels);

            Vector2 nextGoblinTileCoordIndex = CurrentTileCoordIndex(GetWorldToScreen2D(goblinNextPos, camera), cameraScreenPos, curFrameMovementInPixels);

            int nextTileIndex = nextGoblinTileCoordIndex.y * totalTilesX + nextGoblinTileCoordIndex.x;

            //Stop player from moving 
            if (IsTileFilledWithCollider(tm, nextGoblinTileCoordIndex, totalTilesX, true)) {
                //std::cout << "Moving into an empty tile." << std::endl;
                curFrameMovement = curFrameMovement * -1.0f;
                goblinVel = goblinVel * -1.0f;

                goblinNextPos = goblinCharacter_mut->position.pos + curFrameMovement * 4.0f;
                goblin_mut->movingDirection *= -1.0f;
            }

            goblinCharacter_mut->facingDirection.x = curFrameMovement.x == 0.0f ? 1.0f : curFrameMovement.x / abs(curFrameMovement.x);
            goblinCharacter_mut->facingDirection.y = curFrameMovement.y == 0.0f ? 1.0f : curFrameMovement.y / abs(curFrameMovement.y);
            if (goblinCharacter_mut->facingDirection.y < 0.0) {
                goblinCharacter_mut->facingDirection.x = -1.0f;
            }

            goblinCharacter_mut->velocity.vel = goblinVel;
            goblinCharacter_mut->position.pos = goblinNextPos;

            goblinCharacterStates_mut->running = Vector2Length(goblinCharacter_mut->velocity.vel) != 0.0f;
            goblinCharacterStates_mut->idle = Vector2Length(goblinCharacter_mut->velocity.vel) == 0.0f;
            goblinCharacterStates_mut->attackingSide = false;

            goblinCharacter_mut->attackDealtWith = true;

        }
        else if (distanceToPlayer <= 200.0f && goblinCharacter_mut->nextAttackTime <= GetTime()) {
            //std::cout << "goblin attacking." << GetTime() << std::endl;
            //std::cout << "goblin attacking." << GetTime() << std::endl;
            //std::cout << tm.roomsData[roomIndex.y][roomIndex.x].torchGoblinEntitiesInThisRoom[i].name() << "IS ATTACKING!!!!" << std::endl;
            goblinCharacterStates_mut->attackingSide = true;
            goblinCharacterStates_mut->running = false;
            goblinCharacterStates_mut->idle = false;

            goblinCharacter_mut->attackDealtWith = false;

            goblinCharacter_mut->nextAttackTime = GetTime() + goblinCharacter_mut->attackTime;
        }
        else if(finishedPlayingAttackAnimaiton) {
            //std::cout << "goblin stopped attacking." << GetTime() << std::endl;
            goblinCharacterStates_mut->attackingSide = false;
            //goblinCharacterStates_mut->running = true;
            //goblinCharacterStates_mut->idle = false;
            goblinCharacterStates_mut->running = false;
            goblinCharacterStates_mut->idle = true;

            goblinCharacter_mut->attackDealtWith = true;
        }
    }
}

void ApplyKnockBackToGoblin(flecs::world& world, TileMap& tm, Vector2 roomIndex, int goblinIndex, Vector2 knockBackAmount, Camera2D& camera) {

    Goblin* goblin_mut = tm.roomsData[roomIndex.y][roomIndex.x].torchGoblinEntitiesInThisRoom[goblinIndex].get_mut<Goblin>();
    Character* goblinCharacter_mut = tm.roomsData[roomIndex.y][roomIndex.x].torchGoblinEntitiesInThisRoom[goblinIndex].get_mut<Character>();
    CharacterStates* goblinCharacterStates_mut = tm.roomsData[roomIndex.y][roomIndex.x].torchGoblinEntitiesInThisRoom[goblinIndex].get_mut<CharacterStates>();

    Vector2 curFrameMovement = knockBackAmount;
    Vector2 goblinNextPos = goblinCharacter_mut->position.pos + curFrameMovement * 4.0f;

    Vector2 cameraScreenPos = GetWorldToScreen2D(camera.target, camera);
    Vector2 curFrameMovementInPixels = GetWorldToScreen2D(curFrameMovement, camera);
    Vector2 curRoomIndex = CurrentRoomIndex(GetWorldToScreen2D(goblinNextPos, camera), cameraScreenPos, curFrameMovementInPixels);

    Vector2 nextGoblinTileCoordIndex = CurrentTileCoordIndex(GetWorldToScreen2D(goblinNextPos, camera), cameraScreenPos, curFrameMovementInPixels);

    int nextTileIndex = nextGoblinTileCoordIndex.y * totalTilesX + nextGoblinTileCoordIndex.x;

    //Stop player from moving 
    if (IsTileFilledWithCollider(tm, nextGoblinTileCoordIndex, totalTilesX, true)) {
        //std::cout << "Moving into an empty tile." << std::endl;
        curFrameMovement = curFrameMovement * -1.0f;

        goblinNextPos = goblinCharacter_mut->position.pos + curFrameMovement * 4.0f;
        goblin_mut->movingDirection *= -1.0f;
    }

    goblinCharacter_mut->facingDirection.x = curFrameMovement.x == 0.0f ? 1.0f : curFrameMovement.x / abs(curFrameMovement.x);
    goblinCharacter_mut->facingDirection.y = curFrameMovement.y == 0.0f ? 1.0f : curFrameMovement.y / abs(curFrameMovement.y);
    if (goblinCharacter_mut->facingDirection.y < 0.0) {
        goblinCharacter_mut->facingDirection.x = -1.0f;
    }

    goblinCharacter_mut->velocity.vel = Vector2Zeros;
    goblinCharacter_mut->position.pos = goblinNextPos;

    goblinCharacterStates_mut->running = false;
    goblinCharacterStates_mut->idle = true;
    goblinCharacterStates_mut->attackingSide = false;

}

void LoseHealth(flecs::world& world, TileMap& tm, Vector2 roomIndex, int goblinIndex) {

    Character* goblinCharacter_mut = tm.roomsData[roomIndex.y][roomIndex.x].torchGoblinEntitiesInThisRoom[goblinIndex].get_mut<Character>();
    goblinCharacter_mut->health--;

    if (goblinCharacter_mut->health <= 0.0f) {
        std::cout << tm.roomsData[roomIndex.y][roomIndex.x].torchGoblinEntitiesInThisRoom[goblinIndex].name() << " is DEAD!!!" << std::endl;

        tm.roomsData[roomIndex.y][roomIndex.x].torchGoblinEntitiesInThisRoom[goblinIndex].destruct();
        tm.roomsData[roomIndex.y][roomIndex.x].torchGoblinEntitiesInThisRoom.erase(tm.roomsData[roomIndex.y][roomIndex.x].torchGoblinEntitiesInThisRoom.begin() + goblinIndex);
    }

}

