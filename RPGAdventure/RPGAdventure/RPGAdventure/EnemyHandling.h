#pragma once

#include "flecs/flecs.h"

#include "TileMap.h"
#include "TileMapData.h"

#include "TorchGoblinAnimationGraphTransitionFunctions.h"

void SpawnGoblins(flecs::world& world, TileMapData& tmd, TileMap& tm) {

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

                std::string goblinName = "Torch Goblin" + std::to_string(torchGoblin);
                auto e_torchGoblinEntity = world.entity(goblinName.c_str());
                e_torchGoblinEntity.add<Goblin>();
                e_torchGoblinEntity.add<Character>();
                e_torchGoblinEntity.add<CharacterStates>();
                e_torchGoblinEntity.add<SpriteSheet>();
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
                goblin->position.pos = Vector2{ x * 20.0f, y * 20.0f };
                goblin->facingDirection = Vector2{ 1.0f, 1.0f };

                Goblin* gob_mut = e_torchGoblinEntity.get_mut<Goblin>();
                gob_mut->movingDirection = Vector2{ (float)GetRandomValue(-1, 1), (float)GetRandomValue(-1, 1) };

                int setRandToZero = GetRandomValue(0, 1);
                if (setRandToZero == 0) {
                    gob_mut->movingDirection.x = 0;
                }
                else {
                    gob_mut->movingDirection.y = 0;
                }

                if (gob_mut->movingDirection.x == 0 && gob_mut->movingDirection.y == 0) {
                    gob_mut->movingDirection = Vector2{ 1.0f, 0.0f };
                }

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

        Vector2 goblinDirToPlayer = Vector2Subtract(playerPos, goblinCharacter_mut->position.pos);
        float distanceToPlayer = Vector2Length(goblinDirToPlayer);
        goblinDirToPlayer = Vector2Normalize(goblinDirToPlayer);

        goblinCharacter_mut->facingDirection = goblinDirToPlayer * Vector2{ 1 / abs(goblinDirToPlayer.x), 1 / abs(goblinDirToPlayer.y) };

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
            if (IsTileFilledWithCollider(tm, nextGoblinTileCoordIndex, totalTilesX)) {
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

        }
        else if (distanceToPlayer <= 150.0f) {
            //std::cout << "goblin attacking." << GetTime() << std::endl;
            //std::cout << "goblin attacking." << GetTime() << std::endl;
            //std::cout << tm.roomsData[roomIndex.y][roomIndex.x].torchGoblinEntitiesInThisRoom[i].name() << "IS ATTACKING!!!!" << std::endl;
            goblinCharacterStates_mut->attackingSide = true;
            goblinCharacterStates_mut->running = false;
            goblinCharacterStates_mut->idle = false;
        }
        else {
            //std::cout << "goblin stopped attacking." << GetTime() << std::endl;
            goblinCharacterStates_mut->attackingSide = false;
            //goblinCharacterStates_mut->running = true;
            //goblinCharacterStates_mut->idle = false;
            goblinCharacterStates_mut->running = false;
            goblinCharacterStates_mut->idle = true;
        }
    }
}