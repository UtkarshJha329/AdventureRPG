
#define GLSL_VERSION            430
#define GRAPHICS_API_OPENGL_43
#define RLGL_RENDER_TEXTURES_HINT

#include "raylib/raylib.h"
#include "raylib/rlgl.h"
#include "flecs/flecs.h"

#include "AssetResourcesData.h"
#include "TextureResource.h"

#include "SpriteSheet.h"
#include "BoundingBox2D.h"
#include "SpriteAnimation.h"
#include "AnimationGraph.h"
#include "TileMap.h"
#include "TileMapData.h"

#include "Room.h"

#include "KnightAnimationGraphTransitionFunctions.h"
#include "TorchGoblinAnimationGraphTransitionFunctions.h"

#include "EnemyHandling.h"

#include <string>
#include <vector>
#include <fstream>

const int screenWidth = 1280;
const int screenHeight = 720;

struct Player{public:};

const int attackingTime = 1.0f;
const int timeOutForTouchingEnemy = 1.0f;
float timeToMoveAfterEnemyTouch = 0.0f;
float speed = 1000.0f;

int main(void)
{
    flecs::world world;

    auto e_Player = world.entity("Player");
    e_Player.add<Player>();
    e_Player.add<Character>();
    e_Player.add<CharacterStates>();
    e_Player.add<SpriteSheet>();
    e_Player.add<BoundingBox2D>();
    e_Player.add<TextureResource>();
    //e_Player.add<SpriteAnimation>();
    e_Player.add<AnimationGraph>();

    e_Player.set<TextureResource>({ knightCharacterSpriteSheetLocation, knightCharacterSpriteSheet_numSpriteCellsX, knightCharacterSpriteSheet_numSpriteCellsY, knightCharacterSpriteSheet_paddingX, knightCharacterSpriteSheet_paddingY });
    //e_Player.set<Character>(Position{ Vector2{0.0f, 0.0f } }, Velocity{ Vector2{0.0f, 0.0f} }, Direction{ Vector2{0.0f, 0.0f} });
    //e_Player.set<Velocity>({ Vector2{0.0f, 0.0f } });

    AnimationGraph* playerAnimationGraph = e_Player.get_mut<AnimationGraph>();
    playerAnimationGraph->transitionConditionFunctions.push_back(KnightIdleToRunAnimationChangeRule);
    playerAnimationGraph->transitionConditionFunctions.push_back(KnightRunToIdleAnimationChangeRule);

    playerAnimationGraph->transitionConditionFunctions.push_back(KnightIdleToSideAttackAnimationChangeRule);
    playerAnimationGraph->transitionConditionFunctions.push_back(KnightSideAttackToIdleAnimationChangeRule);

    playerAnimationGraph->transitionConditionFunctions.push_back(KnightIdleToDownAttackAnimationChangeRule);
    playerAnimationGraph->transitionConditionFunctions.push_back(KnightDownAttackToIdleAnimationChangeRule);

    playerAnimationGraph->transitionConditionFunctions.push_back(KnightIdleToUpAttackAnimationChangeRule);
    playerAnimationGraph->transitionConditionFunctions.push_back(KnightUpAttackToIdleAnimationChangeRule);

    auto e_Camera2D = world.entity("Camera2D");
    e_Camera2D.add<Camera2D>();

    Camera2D* camera = e_Camera2D.get_mut<Camera2D>();

    camera->target = Vector2{ 0.0f, 0.0f };
    camera->offset = Vector2{ screenWidth / 2.0f, screenHeight / 2.0f };
    camera->rotation = 0.0f;
    camera->zoom = 1.0f;

    auto e_MeasurementCamera2D = world.entity("Measurement Camera 2D");
    e_MeasurementCamera2D.add<Camera2D>();

    Camera2D* measurementCamera = e_MeasurementCamera2D.get_mut<Camera2D>();

    measurementCamera->target = Vector2{ 0.0f, 0.0f };
    measurementCamera->offset = Vector2{ screenWidth / 2.0f, screenHeight / 2.0f };
    measurementCamera->rotation = 0.0f;
    measurementCamera->zoom = 1.0f;

    auto e_tileMapGround = world.entity("Ground Tiles");
    e_tileMapGround.add<TextureResource>();
    e_tileMapGround.add<TileMapData>();
    e_tileMapGround.add<TileMap>();
    e_tileMapGround.add<Vector3>();

    e_tileMapGround.set<TextureResource>({ terrainTopFlatTileMap, terrainTopFlatTileMap_numSpriteCellsX, terrainTopFlatTileMap_numSpriteCellsY, terrainTopFlatTileMap_paddingX, terrainTopFlatTileMap_paddingY });
    e_tileMapGround.set<Vector3>({ Vector3 {0.0f, 0.0f, 0.0f} });

    const TileMap* tm = e_tileMapGround.get<TileMap>();
    TileMap* mut_tm = e_tileMapGround.get_mut<TileMap>();
    TileMapData* mut_tmd = e_tileMapGround.get_mut<TileMapData>();

    //auto e_roomTileMapData = world.entity("Tilemap Data");
    //e_roomTileMapData.add<TileMapData>();

    SpawnRooms(*mut_tm);
    FillTileMapDataFromFile(*mut_tmd);
    SpawnGoblins(world, *mut_tmd, *mut_tm, *measurementCamera);

#pragma region Systems

    auto InitSpriteSheetSystem = world.system<SpriteSheet, TextureResource>()
        .kind(flecs::OnStart)
        .each([](flecs::iter& it, size_t, SpriteSheet& ss, TextureResource& tr) {
            //std::cout << "Init Sprite Sheet System." << std::endl;
            InitSpriteSheet(ss, tr.texSrc, tr.numSpriteCellsX, tr.numSpriteCellsY, tr.paddingX, tr.paddingY);
        });

    auto InitBoundingBoxSystem = world.system<SpriteSheet, BoundingBox2D, Character>()
        .kind(flecs::OnStart)
        .each([](flecs::iter& it, size_t, SpriteSheet& ss, BoundingBox2D& bb, Character& character) {

            float width = ss.cell.width * 0.5f;
            float height = ss.cell.height * 0.5f;
            bb.boundingBoxForSprite = Rectangle{ -(width * 0.5f), -(height * 0.5f), width, height };
            bb.reducedBoundingBoxForSprite = Rectangle{ -(width * 0.5f * 0.5f), -(height * 0.5f * 0.5f), width * 0.5f, height * 0.5f };

            character.attackSideOverlapRect = Rectangle{ bb.reducedBoundingBoxForSprite.x + bb.reducedBoundingBoxForSprite.width, bb.reducedBoundingBoxForSprite.y - bb.reducedBoundingBoxForSprite.height
                                                        ,bb.reducedBoundingBoxForSprite.width * 1.5f, bb.reducedBoundingBoxForSprite.height * 3.0f };
            character.attackUpOverlapRect = Rectangle{ bb.reducedBoundingBoxForSprite.x - bb.reducedBoundingBoxForSprite.width, bb.reducedBoundingBoxForSprite.y - bb.reducedBoundingBoxForSprite.height
                                                      ,bb.reducedBoundingBoxForSprite.width * 3.0f, bb.reducedBoundingBoxForSprite.height * 1.0f };
            character.attackDownOverlapRect = Rectangle{ bb.reducedBoundingBoxForSprite.x - bb.reducedBoundingBoxForSprite.width, bb.reducedBoundingBoxForSprite.y + bb.reducedBoundingBoxForSprite.height
                                                        ,bb.reducedBoundingBoxForSprite.width * 3.0f, bb.reducedBoundingBoxForSprite.height * 1.0f };
        });

    auto InitSpriteSheetAnimationSystem = world.system<SpriteSheet, SpriteAnimation>()
        .kind(flecs::OnStart)
        .each([](flecs::iter& it, size_t, SpriteSheet& ss, SpriteAnimation& spriteAnimation) {
            //std::cout << "Init Sprite Sheet Animation System." << std::endl;
            InitSpriteAnimation(spriteAnimation, 0, ss.numSpriteCellsX, ss.numSpriteCellsX, true, ss);
        });

    auto InitAnimationGraphSystem = world.system<SpriteSheet, AnimationGraph>()
        .kind(flecs::OnStart)
        .each([](flecs::iter& it, size_t, SpriteSheet& ss, AnimationGraph& animationGraph) {
            //std::cout << "Init Sprite Sheet Animation System." << std::endl;
    
            animationGraph.currentAnimationPlaying = 0;
            for (int i = 0; i < ss.numSpriteCellsY; i++)
            {
                SpriteAnimation curSpriteAnimation = { 0 };
                curSpriteAnimation.curAnimationStateY = i;
                InitSpriteAnimation(curSpriteAnimation, 0, ss.numSpriteCellsX, ss.numSpriteCellsX, true, ss);

                animationGraph.animations.push_back(curSpriteAnimation);
            }
        });

    auto InitTileMapSystem = world.system<TextureResource, TileMapData, TileMap>()
        .kind(flecs::OnStart)
        .each([&world](flecs::iter& it, size_t, TextureResource& tr, TileMapData& tmd, TileMap& tm) {

            //std::cout << "Init Tile Map Sprites." << std::endl;
            InitSpriteSheet(tm.tilePallet, tr.texSrc, tr.numSpriteCellsX, tr.numSpriteCellsY, tr.paddingX, tr.paddingY);

            //std::cout << tm.tilePallet.cell.height << std::endl;

            int tilesY = 7;
            int totalY = 70;
            tm.tileTextureIndexData = std::vector<float2>(totalTilesX * totalY, { 0.0f, 0.0f });

            for (int y = 0; y < totalY; y++)
            {
                //tm.tiles[x].reserve(tm.sizeY);
                for (int x = 0; x < totalTilesX; x++)
                {
                    //tm.tileTextureIndexData[y * totalTilesX + x].v[0] = (float)(GetRandomValue(0, (int)(tm.tilePallet.numSpriteCellsX - 1)));
                    //tm.tileTextureIndexData[y * totalTilesX + x].v[1] = (float)(GetRandomValue(0, (int)(tm.tilePallet.numSpriteCellsY - 1)));

                    int tileMapDataReadingX = x * 2;
                    int tileMapDataReadingY = y;

                    if (tmd.tileMapData[tileMapDataReadingY][tileMapDataReadingX] == '#') {
                        tm.tileTextureIndexData[y * totalTilesX + x].v[0] = 8;
                        tm.tileTextureIndexData[y * totalTilesX + x].v[1] = 0;
                    }
                    else if (tmd.tileMapData[tileMapDataReadingY][tileMapDataReadingX] == 'x') {
                        tm.tileTextureIndexData[y * totalTilesX + x].v[0] = 1;
                        tm.tileTextureIndexData[y * totalTilesX + x].v[1] = 2;
                    }
                    else if (tmd.tileMapData[tileMapDataReadingY][tileMapDataReadingX] == 'o') {
                        tm.tileTextureIndexData[y * totalTilesX + x].v[0] = 4;
                        tm.tileTextureIndexData[y * totalTilesX + x].v[1] = 0;
                    }

                    //std::cout << "tile[" << y << "][" << x << "] := " << tileTextureIndexData[y * worldSizeX + x].v[0] << ", " << tileTextureIndexData[y * worldSizeX + x].v[1] << std::endl;
                }
            }

        });

    auto InitEnemyCharacterStatesSystem = world.system<CharacterStates, Goblin>()
        .kind(flecs::OnStart)
        .each([](flecs::iter& it, size_t, CharacterStates& characterStates, Goblin) {
            characterStates.idle = true;
        });

    auto UpdatePlayerAnimationGraphSystem = world.system<AnimationGraph, CharacterStates>()
        .kind(flecs::OnUpdate)
        .each([](flecs::iter& it, size_t, AnimationGraph& animationGraph, CharacterStates& characterStates) {
            //std::cout << "Update Sprite Sheet Animation System." << std::endl;
            //UpdateAnimation(animationGraph.animations[animationGraph.currentAnimationPlaying]);

            std::any parms = characterStates;
            int animationIndex = ShouldAnimationTransition(animationGraph, parms);

            //std::cout << animationIndex << ", " << animationGraph.currentAnimationPlaying << std::endl;

            if (animationIndex != -1 && animationIndex != animationGraph.currentAnimationPlaying) {
                animationGraph.animations[animationGraph.currentAnimationPlaying].timeSinceLastFrameUpdate = 0;
                animationGraph.currentAnimationPlaying = animationIndex;
                //std::cout << "Change Animation." << animationIndex << std::endl;
            }
            else if(animationIndex == -1) {
                animationGraph.animations[animationGraph.currentAnimationPlaying].timeSinceLastFrameUpdate = 0;
                animationGraph.currentAnimationPlaying = 0;
                //std::cout << "Reset Animation." << animationIndex << std::endl;
            }

            UpdateAnimationGraphCurrentAnimation(animationGraph);
        });

    auto GoblinAnimationGraphDrawingSystem = world.system<SpriteSheet, AnimationGraph, Character, Goblin>()
        .kind(flecs::OnUpdate)
        .each([](flecs::iter& it, size_t, SpriteSheet& ss, AnimationGraph& animationGraph, Character& character, Goblin) {
            //std::cout << "Update Sprite Sheet Animation Drawing System." << std::endl;
            //spriteAnimation.curAnimationStateY = 5;
            Rectangle view = animationGraph.animations[animationGraph.currentAnimationPlaying].curFrameView;
            view.width *= character.facingDirection.x;
            DrawTextureRec(ss.spriteSheetTexture, view, character.position.pos - Vector2{ss.cell.width * 0.5f, ss.cell.height * 0.5f}, WHITE);
        });

    auto PlayerAnimationGraphDrawingSystem = world.system<SpriteSheet, AnimationGraph, Character, Player>()
        .kind(flecs::OnUpdate)
        .each([](flecs::iter& it, size_t, SpriteSheet& ss, AnimationGraph& animationGraph, Character& character, Player) {
            //std::cout << "Update Sprite Sheet Animation Drawing System." << std::endl;
            //spriteAnimation.curAnimationStateY = 5;
            Rectangle view = animationGraph.animations[animationGraph.currentAnimationPlaying].curFrameView;
            view.width *= character.facingDirection.x;

            if (timeToMoveAfterEnemyTouch < GetTime()) {
                DrawTextureRec(ss.spriteSheetTexture, view, character.position.pos - Vector2{ss.cell.width * 0.5f, ss.cell.height * 0.5f}, WHITE);
            }
            else {
                DrawTextureRec(ss.spriteSheetTexture, view, character.position.pos - Vector2{ss.cell.width * 0.5f, ss.cell.height * 0.5f}, RED);
            }
        });

#pragma endregion

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

    world.progress(GetFrameTime());

    //std::cout << "Starting Game Loop..." << std::endl;

#pragma region Shader Creation and Initialisations

    Shader simpleTileMapRenderingShader = LoadShader(TextFormat("Shaders/SimpleTileMapRenderingShader.vert", GLSL_VERSION),
        TextFormat("Shaders/SimpleTileMapRenderingShader.frag", GLSL_VERSION));

    int cameraScreenPosInSimpleTileMapRenderingShader = GetShaderLocation(simpleTileMapRenderingShader, "cameraPosOnScreen");
    Vector2 cameraScreenPos = GetWorldToScreen2D(camera->target, *camera);
    SetShaderValue(simpleTileMapRenderingShader, cameraScreenPosInSimpleTileMapRenderingShader, &cameraScreenPos, SHADER_UNIFORM_VEC2);    

    int cameraPosInSimpleTileMapRenderingShader = GetShaderLocation(simpleTileMapRenderingShader, "cameraPos");
    Vector2 cameraPos = camera->target;
    SetShaderValue(simpleTileMapRenderingShader, cameraPosInSimpleTileMapRenderingShader, &cameraPos, SHADER_UNIFORM_VEC2);

    int movementInPixelsInSimpleTileMapRenderingShader = GetShaderLocation(simpleTileMapRenderingShader, "pixelMovement");
    Vector2 movement = Vector2Zeros;
    SetShaderValue(simpleTileMapRenderingShader, movementInPixelsInSimpleTileMapRenderingShader, &movement, SHADER_UNIFORM_VEC2);

    int tileMapSizeXInSimpletileMapRenderingShader = GetShaderLocation(simpleTileMapRenderingShader, "totalTilesX");
    float worldSizeOnX = (float)totalTilesX;
    SetShaderValue(simpleTileMapRenderingShader, tileMapSizeXInSimpletileMapRenderingShader, &worldSizeOnX, SHADER_UNIFORM_FLOAT);

    int tileMapSizeYInSimpletileMapRenderingShader = GetShaderLocation(simpleTileMapRenderingShader, "totalTilesY");
    float worldSizeOnY = (float)totalTilesY;
    SetShaderValue(simpleTileMapRenderingShader, tileMapSizeYInSimpletileMapRenderingShader, &worldSizeOnY, SHADER_UNIFORM_FLOAT);

    int tileMapCellWidthInSimpletileMapRenderingShader = GetShaderLocation(simpleTileMapRenderingShader, "cellWidth");
    float cellWidth = (float)e_tileMapGround.get<TileMap>()->tilePallet.cell.width;
    SetShaderValue(simpleTileMapRenderingShader, tileMapCellWidthInSimpletileMapRenderingShader, &cellWidth, SHADER_UNIFORM_FLOAT);

    int tileMapCellHeightInSimpletileMapRenderingShader = GetShaderLocation(simpleTileMapRenderingShader, "cellHeight");
    float cellHeight = (float)e_tileMapGround.get<TileMap>()->tilePallet.cell.height;
    SetShaderValue(simpleTileMapRenderingShader, tileMapCellHeightInSimpletileMapRenderingShader, &cellHeight, SHADER_UNIFORM_FLOAT);

    unsigned int tileMapTextureSpriteSheetDataSSBO = rlLoadShaderBuffer(tm->tileTextureIndexData.size() * sizeof(float2), tm->tileTextureIndexData.data(), RL_STATIC_DRAW);

#pragma endregion

#pragma region Screen Render Quad

    unsigned int quadVAO = 0;
    unsigned int quadVBO = 0;
    unsigned int quadEBO = 0;

    float vertices[] = {
        // positions         // texture coords
         1.0f,  1.0f, 0.0f,  1.0f, 1.0f, // top right
         1.0f, -1.0f, 0.0f,  1.0f, 0.0f, // bottom right
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, // bottom left
        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f  // top left 
    };

    unsigned short indices[] = {
        3, 1, 0, // first triangle
        3, 2, 1  // second triangle
    };

    // Gen VAO to contain VBO
    quadVAO = rlLoadVertexArray();
    rlEnableVertexArray(quadVAO);

    // Gen and fill vertex buffer (VBO)
    quadVBO = rlLoadVertexBuffer(&vertices, sizeof(vertices), false);
    quadEBO = rlLoadVertexBufferElement(&indices, sizeof(indices), false);

    // Bind vertex attributes (position, texcoords)
    rlSetVertexAttribute(0, 3, RL_FLOAT, 0, 5 * sizeof(float), 0);
    rlEnableVertexAttribute(0);
    rlSetVertexAttribute(1, 2, RL_FLOAT, 0, 5 * sizeof(float), (3 * sizeof(float)));
    rlEnableVertexAttribute(1);

    rlEnableVertexArray(0);

#pragma endregion

    bool attackClosed = true;
    float attackCloseTime = 0.0f;

    Vector2 previousMovementDirection = Vector2Zeros;
    Vector2 prioritizeAxis = Vector2Zeros;

    // VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV GAME LOOP VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV
    while (!WindowShouldClose())
    {
        bool doDebugPrint = false;
        //std::cout << totalTilesX << ", " << totalTilesY << std::endl;

        if (IsKeyPressed(KEY_ONE)) {
            doDebugPrint = true;
        }

#pragma region Get Component References.
        //Variables.
        Camera2D* camera = e_Camera2D.get_mut<Camera2D>();

        SpriteSheet* playerSS = e_Player.get_mut<SpriteSheet>();
        BoundingBox2D* playerBB2D = e_Player.get_mut<BoundingBox2D>();
        Character* playerCharacter_mut = e_Player.get_mut<Character>();
        CharacterStates* playerCharacterStates_mut = e_Player.get_mut<CharacterStates>();

        playerCharacter_mut->velocity.vel = Vector2Zeros;

        Vector2 curFrameMovementDirection = Vector2Zeros;

        Vector2 curFrameMovement = Vector2Zeros;

        float deltaTime = GetFrameTime();
#pragma endregion

#pragma region Handle Open Attack States For Player

        //Handle open attack states for player.
        if (!attackClosed) {
            if (attackCloseTime <= GetTime()) {
                //std::cout << "Stopped Attack!" << std::endl;

                playerCharacterStates_mut->attackingSide = false;
                playerCharacterStates_mut->attackingDown = false;
                playerCharacterStates_mut->attackingUp = false;
                attackClosed = true;
            }
        }
#pragma endregion

#pragma region Player Movement Input.
        //Handle player movement input.

        if (timeToMoveAfterEnemyTouch < GetTime()) {

            if (IsKeyDown(KEY_RIGHT)) {
                curFrameMovementDirection.x = 1.0;
            }
            if (IsKeyDown(KEY_LEFT)) {
                curFrameMovementDirection.x = -1.0;
            }
            if (IsKeyDown(KEY_UP)) {
                curFrameMovementDirection.y = 1.0;
            }
            if (IsKeyDown(KEY_DOWN)) {
                curFrameMovementDirection.y = -1.0;
            }
        }

#pragma endregion
        
#pragma region Player Facing Direction.
        //Handle player facing direction.

        if (!Vector2Equals(curFrameMovementDirection, previousMovementDirection)) {
            //std::cout << "Reset facing direction." << std::endl;
            if (curFrameMovementDirection.x != 0) {
                playerCharacter_mut->facingDirection.x = curFrameMovementDirection.x == 0.0f ? playerCharacter_mut->facingDirection.x : curFrameMovementDirection.x / abs(curFrameMovementDirection.x);
            }

            playerCharacter_mut->facingDirection.y = curFrameMovementDirection.y == 0.0f ? playerCharacter_mut->facingDirection.y : curFrameMovementDirection.y / abs(curFrameMovementDirection.y);
            if (curFrameMovementDirection.y != 0.0) {
                prioritizeAxis.y = 1.0f;
                prioritizeAxis.x = 0.0f;
            }
            if (curFrameMovementDirection.x != 0.0f) {
                prioritizeAxis.x = 1.0f;
                prioritizeAxis.y = 0.0f;
            }
        }
#pragma endregion

#pragma region Player Attaccking Input.
        //Handle player attacking input.
        bool calculateAttack = false;

        if (IsKeyPressed(KEY_SPACE) && !IsCharacterAttacking(*playerCharacterStates_mut)) {
            //std::cout << "Attacking!" << std::endl;

            if (curFrameMovementDirection.x != 0.0f && playerCharacter_mut->facingDirection.x != 0.0f/* && curDirection.y == 0.0f*/) {
                playerCharacterStates_mut->attackingSide = true;
            }
            else if (prioritizeAxis.x == 1.0f) {
                playerCharacterStates_mut->attackingSide = true;
            }
            else if (playerCharacter_mut->facingDirection.y == -1.0f) {
                playerCharacterStates_mut->attackingDown = true;
            }
            else if (playerCharacter_mut->facingDirection.y == 1.0f) {
                playerCharacterStates_mut->attackingUp = true;
            }

            attackCloseTime = GetTime() + attackingTime;
            attackClosed = false;
            calculateAttack = true;
        }
#pragma endregion

#pragma region Handle Player Movement And Animations.

        Vector2 curFrameVel = curFrameMovementDirection * Vector2{ 1.0f, -1.0f } * speed;
        curFrameMovement = curFrameVel * deltaTime;

        if (IsCharacterAttacking(*playerCharacterStates_mut)) {
            //std::cout << "Stop moving while attacking." << std::endl;
            curFrameMovement = Vector2Zeros;
            curFrameVel = Vector2Zeros;
        }

        Vector2 nextPlayerPosX = playerCharacter_mut->position.pos + Vector2{ curFrameMovement.x, 0.0f } *4.0f;
        Vector2 nextPlayerPosY = playerCharacter_mut->position.pos + Vector2{ 0.0f, curFrameMovement.y } *4.0f;

        Vector2 cameraScreenPosNextPlayerPos = GetWorldToScreen2D(camera->target, *camera);
        Vector2 curFrameMovementInPixelsNextPlayerPos = GetWorldToScreen2D(curFrameMovement, *camera);

        Vector2 curRoomIndexNextPlayerPos = CurrentRoomIndex(GetWorldToScreen2D(playerCharacter_mut->position.pos, *camera), cameraScreenPosNextPlayerPos, curFrameMovementInPixelsNextPlayerPos) * Vector2 { 1.0f, 1.0 };

        bool touchedEnemy = false;
        for (int i = 0; i < tm->roomsData[curRoomIndexNextPlayerPos.y][curRoomIndexNextPlayerPos.x].torchGoblinEntitiesInThisRoom.size(); i++) {

            auto e_CurrentGoblin = tm->roomsData[curRoomIndexNextPlayerPos.y][curRoomIndexNextPlayerPos.x].torchGoblinEntitiesInThisRoom[i];
            BoundingBox2D* curGoblinBB2D = e_CurrentGoblin.get_mut<BoundingBox2D>();
            Character* curGoblinCharacter = e_CurrentGoblin.get_mut<Character>();

            Rectangle playerWorldBB = Rectangle{ nextPlayerPosX.x + playerBB2D->reducedBoundingBoxForSprite.width, nextPlayerPosY.y + playerBB2D->reducedBoundingBoxForSprite.height
                                                , playerBB2D->reducedBoundingBoxForSprite.width, playerBB2D->reducedBoundingBoxForSprite.height };

            Rectangle curGoblinWorldBB = Rectangle{ curGoblinCharacter->position.pos.x + curGoblinBB2D->reducedBoundingBoxForSprite.width, curGoblinCharacter->position.pos.y + curGoblinBB2D->reducedBoundingBoxForSprite.height
                                                    , curGoblinBB2D->reducedBoundingBoxForSprite.width, curGoblinBB2D->reducedBoundingBoxForSprite.height };

            if (doDebugPrint) {
                std::cout << "1 := " << curGoblinWorldBB.x << ", " << curGoblinWorldBB.y << ", " << curGoblinWorldBB.width << ", " << curGoblinWorldBB.height << std::endl;
            }

            if (CheckCollisionRecs(playerWorldBB, curGoblinWorldBB)) {
                //std::cout << "Player and " << e_CurrentGoblin.name() << " collided!!!!" << std::endl;
                Vector2 knockBackDir = Vector2{ nextPlayerPosX.x, nextPlayerPosY.y } - curGoblinCharacter->position.pos;
                knockBackDir = Vector2Normalize(knockBackDir);

                float knockBackStrength = 50.0f;

                curFrameMovement = knockBackDir * knockBackStrength;
                touchedEnemy = true;
            }
        }

        if (touchedEnemy) {
            timeToMoveAfterEnemyTouch = GetTime() + timeOutForTouchingEnemy;
        }

        nextPlayerPosX = playerCharacter_mut->position.pos + Vector2{ curFrameMovement.x, 0.0f } *4.0f;
        nextPlayerPosY = playerCharacter_mut->position.pos + Vector2{ 0.0f, curFrameMovement.y } *4.0f;
        
        //cameraScreenPosNextPlayerPos = GetWorldToScreen2D(camera->target, *camera);
        curFrameMovementInPixelsNextPlayerPos = GetWorldToScreen2D(curFrameMovement, *camera);

        Vector2 nextRoomIndexNextPlayerPosX = CurrentRoomIndex(GetWorldToScreen2D(nextPlayerPosX, *camera), cameraScreenPosNextPlayerPos, curFrameMovementInPixelsNextPlayerPos) * Vector2 { 1.0f, 1.0 };
        Vector2 nextRoomIndexNextPlayerPosY = CurrentRoomIndex(GetWorldToScreen2D(nextPlayerPosY, *camera), cameraScreenPosNextPlayerPos, curFrameMovementInPixelsNextPlayerPos) * Vector2 { 1.0f, 1.0 };

        Vector2 nextPlayerTileCoordIndexX = CurrentTileCoordIndex(GetWorldToScreen2D(nextPlayerPosX, *camera), cameraScreenPosNextPlayerPos, curFrameMovementInPixelsNextPlayerPos);
        Vector2 nextPlayerTileCoordIndexY = CurrentTileCoordIndex(GetWorldToScreen2D(nextPlayerPosY, *camera), cameraScreenPosNextPlayerPos, curFrameMovementInPixelsNextPlayerPos);

        int nextTileIndexX = nextPlayerTileCoordIndexX.y * totalTilesX + nextPlayerTileCoordIndexX.x;
        int nextTileIndexY = nextPlayerTileCoordIndexY.y * totalTilesX + nextPlayerTileCoordIndexY.x;

        //Stop player from moving 
        if (IsTileFilledWithCollider(*mut_tm, nextPlayerTileCoordIndexX, totalTilesX)) {
            //std::cout << "Moving into an empty tile." << std::endl;
            curFrameMovement = Vector2{ curFrameMovement.x, -curFrameMovement.y } * -1.0f;
            curFrameVel = Vector2{ curFrameVel.x, -curFrameVel.y } * -1.0f;
        }
        if (IsTileFilledWithCollider(*mut_tm, nextPlayerTileCoordIndexY, totalTilesX)) {
            //std::cout << "Moving into an empty tile." << std::endl;
            curFrameMovement = Vector2{ -curFrameMovement.x, curFrameMovement.y } * -1.0f;
            curFrameVel = Vector2{ -curFrameVel.x, curFrameVel.y } * -1.0f;
        }

        playerCharacter_mut->position.pos += curFrameMovement;
        playerCharacter_mut->velocity.vel = curFrameVel;

        playerCharacterStates_mut->running = Vector2Length(playerCharacter_mut->velocity.vel) != 0.0f;
        playerCharacterStates_mut->idle = Vector2Length(playerCharacter_mut->velocity.vel) == 0.0f;

#pragma endregion

#pragma region Handle Player Attack

        if (calculateAttack) {

            for (int i = 0; i < tm->roomsData[curRoomIndexNextPlayerPos.y][curRoomIndexNextPlayerPos.x].torchGoblinEntitiesInThisRoom.size(); i++) {

                auto e_CurrentGoblin = tm->roomsData[curRoomIndexNextPlayerPos.y][curRoomIndexNextPlayerPos.x].torchGoblinEntitiesInThisRoom[i];
                BoundingBox2D* curGoblinBB2D = e_CurrentGoblin.get_mut<BoundingBox2D>();
                Character* curGoblinCharacter = e_CurrentGoblin.get_mut<Character>();

                Rectangle curGoblinWorldBB = Rectangle{ curGoblinCharacter->position.pos.x - curGoblinBB2D->reducedBoundingBoxForSprite.width * 0.5f, curGoblinCharacter->position.pos.y - curGoblinBB2D->reducedBoundingBoxForSprite.height * 0.5f
                                                        , curGoblinBB2D->reducedBoundingBoxForSprite.width, curGoblinBB2D->reducedBoundingBoxForSprite.height };

                if (doDebugPrint) {
                    std::cout << "2 := " << curGoblinWorldBB.x << ", " << curGoblinWorldBB.y << ", " << curGoblinWorldBB.width << ", " << curGoblinWorldBB.height << std::endl;
                }

                Rectangle playerWorldAttackBB = Rectangle{ playerCharacter_mut->position.pos.x , playerCharacter_mut->position.pos.y , 0.0f, 0.0f };

                if (playerCharacterStates_mut->attackingSide) {
                    //std::cout << "Attacked side" << std::endl;
                    if (playerCharacter_mut->facingDirection.x < 0.0f) {
                        playerWorldAttackBB.x += playerCharacter_mut->attackSideOverlapRect.x * -4.0f;
                    }
                    else {
                        playerWorldAttackBB.x += playerCharacter_mut->attackSideOverlapRect.x;
                    }
                    playerWorldAttackBB.y += playerCharacter_mut->attackSideOverlapRect.y;
                    playerWorldAttackBB.width = playerCharacter_mut->attackSideOverlapRect.width;
                    playerWorldAttackBB.height = playerCharacter_mut->attackSideOverlapRect.height;
                }
                else if (playerCharacterStates_mut->attackingDown) {
                    //std::cout << "Attacked down" << std::endl;
                    playerWorldAttackBB.x += playerCharacter_mut->attackDownOverlapRect.x;
                    playerWorldAttackBB.y += playerCharacter_mut->attackDownOverlapRect.y;
                    playerWorldAttackBB.width = playerCharacter_mut->attackDownOverlapRect.width;
                    playerWorldAttackBB.height = playerCharacter_mut->attackDownOverlapRect.height;

                }
                else if (playerCharacterStates_mut->attackingUp) {
                    //std::cout << "Attacked up" << std::endl;
                    playerWorldAttackBB.x += playerCharacter_mut->attackUpOverlapRect.x;
                    playerWorldAttackBB.y += playerCharacter_mut->attackUpOverlapRect.y;
                    playerWorldAttackBB.width = playerCharacter_mut->attackUpOverlapRect.width;
                    playerWorldAttackBB.height = playerCharacter_mut->attackUpOverlapRect.height;

                }

                if (CheckCollisionRecs(playerWorldAttackBB, curGoblinWorldBB)) {
                    //std::cout << "Player and " << e_CurrentGoblin.name() << " collided!!!!" << std::endl;
                    Vector2 knockBackDir = curGoblinCharacter->position.pos - Vector2{ nextPlayerPosX.x, nextPlayerPosY.y };
                    knockBackDir = Vector2Normalize(knockBackDir);

                    float knockBackStrength = 10.0f;

                    curGoblinCharacter->knockBack = knockBackDir * knockBackStrength;

                    ApplyKnockBackToGoblin(world, *mut_tm, curRoomIndexNextPlayerPos, i, *camera);
                    //curFrameMovement = knockBackDir * knockBackStrength;
                    //std::cout << "Hit " << e_CurrentGoblin.name() << std::endl;
                }
            }
        }


#pragma endregion

#pragma region Goblin Stuff

        curRoomIndexNextPlayerPos = CurrentRoomIndex(GetWorldToScreen2D(playerCharacter_mut->position.pos, *camera), cameraScreenPosNextPlayerPos, curFrameMovementInPixelsNextPlayerPos) * Vector2 { 1.0f, 1.0 };
        MakeGoblinsMoveIt(world, *mut_tm, curRoomIndexNextPlayerPos, playerCharacter_mut->position.pos, Goblin::goblinSpeed, deltaTime, *camera);

#pragma endregion

#pragma region Update Animations Based On Current Data
        UpdatePlayerAnimationGraphSystem.run();
#pragma endregion

#pragma region Camera Room Movement.

        int numTilesYRounded = 7;
        Vector2 offsetByTiles = Vector2{ numTilesX * tileScaleX, (numTilesYRounded * round(tileScaleY) ) + 30.0f } * -0.5f;
        Vector2 worldDistanceToOffsetBy = GetScreenToWorld2D(offsetByTiles, *measurementCamera);

        Vector2 cameraScreenPos = GetWorldToScreen2D(camera->target, *camera);
        Vector2 curFrameMovementInPixels = GetWorldToScreen2D(curFrameMovement, *camera);
        Vector2 curRoomIndex = CurrentRoomIndex(GetWorldToScreen2D(playerCharacter_mut->position.pos, *camera), cameraScreenPos, curFrameMovementInPixels) * Vector2 { 1.0f, 1.0 };
        Vector2 cameraCurRoomIndex = CurrentRoomIndex(GetWorldToScreen2D(camera->target, *camera), cameraScreenPos, curFrameMovementInPixels);

        //std::cout << cameraCurRoomIndex.x << ", " << cameraCurRoomIndex.y << std::endl;

        if (!Vector2Equals(curRoomIndex, cameraCurRoomIndex)) {

            Vector2 curRoomIndexDifference = Vector2{ (curRoomIndex.x - cameraCurRoomIndex.x) * -1.0f, curRoomIndex.y - cameraCurRoomIndex.y };
            Vector2 worldDistanceToOffsetCameraBy = curRoomIndexDifference * worldDistanceToOffsetBy;

            camera->target = camera->target + worldDistanceToOffsetCameraBy;
        }

        //camera->target = playerCharacter_mut->position.pos;
        camera->zoom += ((float)GetMouseWheelMove() * 0.05f);

#pragma endregion

#pragma region Draw To Screen.

        BeginDrawing();

        ClearBackground(RAYWHITE);


        if(true)
        {
            rlEnableShader(simpleTileMapRenderingShader.id);

            Vector2 cameraScreenPos = GetWorldToScreen2D(camera->target, *camera);
            Vector2 cameraPos = camera->target;
            Vector2 curFrameMovementInPixels = GetWorldToScreen2D(curFrameMovement, *camera);

            SetShaderValue(simpleTileMapRenderingShader, cameraScreenPosInSimpleTileMapRenderingShader, &cameraScreenPos, SHADER_UNIFORM_VEC2);
            SetShaderValue(simpleTileMapRenderingShader, cameraPosInSimpleTileMapRenderingShader, &cameraPos, SHADER_UNIFORM_VEC2);
            SetShaderValue(simpleTileMapRenderingShader, movementInPixelsInSimpleTileMapRenderingShader, &curFrameMovementInPixels, SHADER_UNIFORM_VEC2);

            rlBindShaderBuffer(tileMapTextureSpriteSheetDataSSBO, 3);

            rlActiveTextureSlot(0);
            rlEnableTexture(e_tileMapGround.get<TileMap>()->tilePallet.spriteSheetTexture.id);

            rlEnableVertexArray(quadVAO);
            rlDrawVertexArrayElements(0, 6, 0);

            rlEnableVertexArray(0);

            rlDisableShader();
        }

        BeginMode2D(*camera);

        //SpriteSheetAnimationDrawingSystem.run();
        //std::cout << "Before draw: " << playerCharacterStates_mut->attackingSide << std::endl;

        GoblinAnimationGraphDrawingSystem.run();
        PlayerAnimationGraphDrawingSystem.run();
        //std::cout << "After draw: " << playerCharacterStates_mut->attackingSide << std::endl;


        //float resolution = screenWidth / screenHeight;
        //float width = screenWidth / 12;
        //float height = width / resolution;
        //Vector2 attackFacingDebugPos = curPlayerPos->pos + (curDirection * Vector2{ width, height });
        //DrawRectangle(attackFacingDebugPos.x, attackFacingDebugPos.y, (int)width, (int)height, RED);

        //DrawCircle(curPlayerPos->pos.x, curPlayerPos->pos.y, 10.0f, RED);
        //DrawRectangleLines(playerCharacter_mut->position.pos.x - (width * 0.5f), playerCharacter_mut->position.pos.y - (height * 0.5f), width, height, RED);
        float width = playerBB2D->reducedBoundingBoxForSprite.width;
        float height = playerBB2D->reducedBoundingBoxForSprite.height;

        float posX = playerCharacter_mut->position.pos.x + playerBB2D->reducedBoundingBoxForSprite.x;
        float posY = playerCharacter_mut->position.pos.y + playerBB2D->reducedBoundingBoxForSprite.y;

        DrawRectangleLines(posX, posY, width, height, RED);

        //posY -= height;
        //height *= 3.0f;
        //posX += width;
        //width *= 1.5f;

        //DrawRectangleLines(posX, posY, width, height, RED);

        Rectangle playerWorldAttackBB = Rectangle{ playerCharacter_mut->position.pos.x , playerCharacter_mut->position.pos.y , 0.0f, 0.0f };

        if (playerCharacterStates_mut->attackingSide) {
            //std::cout << "Attacked side" << std::endl;
            //std::cout << playerCharacter_mut->facingDirection.x << std::endl;
            if (playerCharacter_mut->facingDirection.x < 0.0f) {
                playerWorldAttackBB.x += playerCharacter_mut->attackSideOverlapRect.x * -4.0f;
            }
            else {
                playerWorldAttackBB.x += playerCharacter_mut->attackSideOverlapRect.x;
            }
            playerWorldAttackBB.y += playerCharacter_mut->attackSideOverlapRect.y;
            playerWorldAttackBB.width = playerCharacter_mut->attackSideOverlapRect.width;
            playerWorldAttackBB.height = playerCharacter_mut->attackSideOverlapRect.height;
        }
        else if (playerCharacterStates_mut->attackingDown) {
            //std::cout << "Attacked down" << std::endl;
            playerWorldAttackBB.x += playerCharacter_mut->attackDownOverlapRect.x;
            playerWorldAttackBB.y += playerCharacter_mut->attackDownOverlapRect.y;
            playerWorldAttackBB.width = playerCharacter_mut->attackDownOverlapRect.width;
            playerWorldAttackBB.height = playerCharacter_mut->attackDownOverlapRect.height;

        }
        else if (playerCharacterStates_mut->attackingUp) {
            //std::cout << "Attacked up" << std::endl;
            playerWorldAttackBB.x += playerCharacter_mut->attackUpOverlapRect.x;
            playerWorldAttackBB.y += playerCharacter_mut->attackUpOverlapRect.y;
            playerWorldAttackBB.width = playerCharacter_mut->attackUpOverlapRect.width;
            playerWorldAttackBB.height = playerCharacter_mut->attackUpOverlapRect.height;

        }

        DrawRectangleLines(playerWorldAttackBB.x, playerWorldAttackBB.y, playerWorldAttackBB.width, playerWorldAttackBB.height, RED);


        for (int i = 0; i < tm->roomsData[curRoomIndexNextPlayerPos.y][curRoomIndexNextPlayerPos.x].torchGoblinEntitiesInThisRoom.size(); i++) {

            auto e_CurrentGoblin = tm->roomsData[curRoomIndexNextPlayerPos.y][curRoomIndexNextPlayerPos.x].torchGoblinEntitiesInThisRoom[i];
            BoundingBox2D* curGoblinBB2D = e_CurrentGoblin.get_mut<BoundingBox2D>();
            Character* curGoblinCharacter = e_CurrentGoblin.get_mut<Character>();

            Rectangle curGoblinWorldBB = Rectangle{ curGoblinCharacter->position.pos.x - curGoblinBB2D->reducedBoundingBoxForSprite.width * 0.5f, curGoblinCharacter->position.pos.y - curGoblinBB2D->reducedBoundingBoxForSprite.height * 0.5f
                                                    , curGoblinBB2D->reducedBoundingBoxForSprite.width, curGoblinBB2D->reducedBoundingBoxForSprite.height };
            if (doDebugPrint) {
                std::cout << "3 := " << curGoblinWorldBB.x << ", " << curGoblinWorldBB.y << ", " << curGoblinWorldBB.width << ", " << curGoblinWorldBB.height << std::endl;
            }

            DrawRectangleLines(curGoblinWorldBB.x, curGoblinWorldBB.y, curGoblinWorldBB.width, curGoblinWorldBB.height, RED);

        }

        EndMode2D();

        DrawFPS(40, 40);

        EndDrawing();

#pragma endregion

        previousMovementDirection = curFrameMovementDirection;
    }

    CloseWindow();        // Close window and OpenGL context

    return 0;
}
