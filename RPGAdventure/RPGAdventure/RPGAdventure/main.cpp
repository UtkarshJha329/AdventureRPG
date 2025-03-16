
#define GLSL_VERSION            430
#define GRAPHICS_API_OPENGL_43
#define RLGL_RENDER_TEXTURES_HINT

#include "raylib/raylib.h"
#include "raylib/rlgl.h"
#include "flecs/flecs.h"

#include "AssetResourcesData.h"
#include "TextureResource.h"

#include "SpriteSheet.h"
#include "SpriteAnimation.h"
#include "AnimationGraph.h"
#include "TileMap.h"

#include "Room.h"

#include "KnightAnimationGraphTransitionFunctions.h"
#include "TorchGoblinAnimationGraphTransitionFunctions.h"

#include <string>
#include <vector>

const int screenWidth = 1280;
const int screenHeight = 720;

struct Player{public:};
struct Goblin{public:};

const int worldSizeX = 100;
const int worldSizeY = 100;

const int attackingTime = 1.0f;

const int roomSizeX = 1920.0f;
const int roomSizeY = 1200.0f;

const int numRooms = 10.0f;

int main(void)
{
    flecs::world world;

    auto e_Player = world.entity("Player");
    e_Player.add<Player>();
    e_Player.add<Character>();
    e_Player.add<CharacterStates>();
    e_Player.add<SpriteSheet>();
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

    auto e_Goblin = world.entity("Goblin");
    e_Goblin.add<Goblin>();
    e_Goblin.add<Character>();
    e_Goblin.add<CharacterStates>();
    e_Goblin.add<SpriteSheet>();
    e_Goblin.add<TextureResource>();
    e_Goblin.add<AnimationGraph>();

    e_Goblin.set<TextureResource>({ torchGoblin_CharacterSpriteSheetLocation, torchGoblin_CharacterSpriteSheet_numSpriteCellsX, torchGoblin_CharacterSpriteSheet_numSpriteCellsY, torchGoblin_CharacterSpriteSheet_paddingX, torchGoblin_CharacterSpriteSheet_paddingY});

    AnimationGraph* goblinAnimationGraph = e_Goblin.get_mut<AnimationGraph>();
    goblinAnimationGraph->transitionConditionFunctions.push_back(TorchGoblinIdleToOthersAnimationChangeRule);
    goblinAnimationGraph->transitionConditionFunctions.push_back(TorchGoblinRunToOthersAnimationChangeRule);
    goblinAnimationGraph->transitionConditionFunctions.push_back(TorchGoblinIdleAndSideAttackAnimationChangeRule);
    goblinAnimationGraph->transitionConditionFunctions.push_back(TorchGoblinIdleAndDownAttackAnimationChangeRule);
    goblinAnimationGraph->transitionConditionFunctions.push_back(TorchGoblinIdleAndUpAttackAnimationChangeRule);

    Character* goblin = e_Goblin.get_mut<Character>();
    goblin->position.pos = Vector2{ 10.0f, 20.0f };
    goblin->facingDirection = Vector2{ 1.0f, 1.0f };

    auto e_Camera2D = world.entity("Camera2D");
    e_Camera2D.add<Camera2D>();

    Camera2D* camera = e_Camera2D.get_mut<Camera2D>();

    camera->target = Vector2{ 0.0f, 0.0f };
    camera->offset = Vector2{ screenWidth / 2.0f, screenHeight / 2.0f };
    camera->rotation = 0.0f;
    camera->zoom = 1.0f;

    auto e_tileMapGround = world.entity("Ground Tiles");
    e_tileMapGround.add<TextureResource>();
    e_tileMapGround.add<TileMap>();
    e_tileMapGround.add<Vector3>();

    e_tileMapGround.set<TextureResource>({ terrainTopFlatTileMap, terrainTopFlatTileMap_numSpriteCellsX, terrainTopFlatTileMap_numSpriteCellsY, terrainTopFlatTileMap_paddingX, terrainTopFlatTileMap_paddingY });
    e_tileMapGround.set<Vector3>({ Vector3 {0.0f, 0.0f, 0.0f} });

    std::vector<Vector2> tileTextureIndexData(worldSizeX * worldSizeY, { 0.0f, 0.0f });

    unsigned int curRoomIndex = 0;
    std::vector<flecs::entity> roomEntities(numRooms);

    for (int i = 0; i < numRooms; i++)
    {
        std::string roomName = "Room " + std::to_string(i);
        auto e_room = world.entity(roomName.c_str());
        e_room.add<Position>();
        e_room.add<Room>();
        float pos = 0.0f;
        e_room.set<Position>({ pos, i * roomSizeY * -1.0f });

        roomEntities[i] = e_room;
    }

    auto InitSpriteSheetSystem = world.system<SpriteSheet, TextureResource>()
        .kind(flecs::OnStart)
        .each([](flecs::iter& it, size_t, SpriteSheet& ss, TextureResource& tr) {
            //std::cout << "Init Sprite Sheet System." << std::endl;
            InitSpriteSheet(ss, tr.texSrc, tr.numSpriteCellsX, tr.numSpriteCellsY, tr.paddingX, tr.paddingY);
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

    auto InitTileMapSystem = world.system<TextureResource, TileMap>()
        .kind(flecs::OnStart)
        .each([&tileTextureIndexData](flecs::iter& it, size_t, TextureResource& tr, TileMap& tm) {

            //std::cout << "Init Tile Map Sprites." << std::endl;
            InitSpriteSheet(tm.tilePallet, tr.texSrc, tr.numSpriteCellsX, tr.numSpriteCellsY, tr.paddingX, tr.paddingY);

            tm.sizeX = worldSizeX;
            tm.sizeY = worldSizeY;

            tm.tiles.reserve(tm.sizeX);
            for (int x = 0; x < tm.sizeX; x++)
            {
                tm.tiles.push_back(std::vector<Tile>(tm.sizeY));
                //tm.tiles[x].reserve(tm.sizeY);
                for (int y = 0; y < tm.sizeY; y++)
                {
                    tm.tiles[x][y].tileInSpriteSheet = tm.tilePallet.cell;

                    tm.tiles[x][y].tileInSpriteSheet.x = GetRandomValue(0, tm.tilePallet.numSpriteCellsX) * tm.tilePallet.cell.width;
                    tm.tiles[x][y].tileInSpriteSheet.y = GetRandomValue(0, tm.tilePallet.numSpriteCellsY) * tm.tilePallet.cell.height;

                    tileTextureIndexData[y * worldSizeX + x].x = tm.tiles[x][y].tileInSpriteSheet.x;
                    tileTextureIndexData[y * worldSizeX + x].x = tm.tiles[x][y].tileInSpriteSheet.y;

                    //std::cout << "tile[" << x << "][" << y << "] := " << tm.tiles[x][y].tileInSpriteSheet.x << ", " << tm.tiles[x][y].tileInSpriteSheet.y << ", " << tm.tiles[x][y].tileInSpriteSheet.width << ", " << tm.tiles[x][y].tileInSpriteSheet.height << std::endl;
                }
            }

        });

    auto InitEnemyCharacterStatesSystem = world.system<CharacterStates, Goblin>()
        .kind(flecs::OnStart)
        .each([](flecs::iter& it, size_t, CharacterStates& characterStates, Goblin) {
            characterStates.idle = true;
        });

    auto UpdateSpriteSheetAnimationSystem = world.system<SpriteAnimation>()
        .kind(flecs::OnUpdate)
        .each([](flecs::iter& it, size_t, SpriteAnimation& spriteAnimation) {
            //std::cout << "Update Sprite Sheet Animation System." << std::endl;
            UpdateAnimation(spriteAnimation);
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

    auto SpriteSheetAnimationDrawingSystem = world.system<SpriteSheet, SpriteAnimation, Position>()
        .kind(flecs::OnUpdate)
        .each([](flecs::iter& it, size_t, SpriteSheet& ss, SpriteAnimation& spriteAnimation, Position& position) {
            //std::cout << "Update Sprite Sheet Animation Drawing System." << std::endl;
            //spriteAnimation.curAnimationStateY = 5;
            DrawTextureRec(ss.spriteSheetTexture, spriteAnimation.curFrameView, position.pos - Vector2{ ss.cell.width * 0.5f, ss.cell.height * 0.5f }, WHITE);
        });

    auto AnimationGraphDrawingSystem = world.system<SpriteSheet, AnimationGraph, Character>()
        .kind(flecs::OnUpdate)
        .each([](flecs::iter& it, size_t, SpriteSheet& ss, AnimationGraph& animationGraph, Character& character) {
            //std::cout << "Update Sprite Sheet Animation Drawing System." << std::endl;
            //spriteAnimation.curAnimationStateY = 5;
            Rectangle view = animationGraph.animations[animationGraph.currentAnimationPlaying].curFrameView;
            view.width *= character.facingDirection.x;
            DrawTextureRec(ss.spriteSheetTexture, view, character.position.pos - Vector2{ss.cell.width * 0.5f, ss.cell.height * 0.5f}, WHITE);
        });

    auto TileMapDrawingSystem = world.system<TileMap>()
        .kind(flecs::OnUpdate)
        .each([e_Camera2D](flecs::iter& it, size_t, TileMap& tm) {

            Vector2 windowStart = GetScreenToWorld2D(Vector2{ 0.0f, 0.0f }, *e_Camera2D.get<Camera2D>());
            Vector2 windowEnd = GetScreenToWorld2D(Vector2{ (float)GetScreenWidth(), (float)GetScreenHeight()}, *e_Camera2D.get<Camera2D>());

            //std::cout << "Drawing tiles." << std::endl;
            BeginMode2D(*e_Camera2D.get<Camera2D>());
                for (int x = 0; x < tm.sizeX; x++)
                {
                    float xPos = (float)x * tm.tiles[x][0].tileInSpriteSheet.width;
                    if (xPos >= windowStart.x && xPos <= windowEnd.x) {

                        for (int y = 0; y < tm.sizeY; y++)
                        {
                            float yPos = (float)y * tm.tiles[x][y].tileInSpriteSheet.height;
                            if (yPos >= windowStart.y && yPos <= windowEnd.y)
                            {
                                DrawTextureRec(tm.tilePallet.spriteSheetTexture, tm.tiles[x][y].tileInSpriteSheet, Vector2{ xPos , yPos }, WHITE);
                            }
                        }
                    }
                }
            EndMode2D();
        });

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");


    world.progress(GetFrameTime());

    //std::cout << "Starting Game Loop..." << std::endl;

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
    float worldSizeOnX = (float)worldSizeX;
    SetShaderValue(simpleTileMapRenderingShader, tileMapSizeXInSimpletileMapRenderingShader, &worldSizeOnX, SHADER_UNIFORM_FLOAT);

    int tileMapSizeYInSimpletileMapRenderingShader = GetShaderLocation(simpleTileMapRenderingShader, "totalTilesY");
    float worldSizeOnY = (float)worldSizeY;
    SetShaderValue(simpleTileMapRenderingShader, tileMapSizeYInSimpletileMapRenderingShader, &worldSizeOnY, SHADER_UNIFORM_FLOAT);

    unsigned int tileMapTextureSpriteSheetDataSSBO = rlLoadShaderBuffer(tileTextureIndexData.size() * sizeof(Vector2), tileTextureIndexData.data(), RL_STATIC_DRAW);


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

    bool attackClosed = true;
    float attackCloseTime = 0.0f;

    Vector2 previousMovementDirection = Vector2Zeros;

    while (!WindowShouldClose())
    {
        Character* playerCharacter_mut = e_Player.get_mut<Character>();
        CharacterStates* playerCharacterStates_mut = e_Player.get_mut<CharacterStates>();

        Character* goblinCharacter_mut = e_Goblin.get_mut<Character>();
        CharacterStates* goblinCharacterStates_mut = e_Goblin.get_mut<CharacterStates>();

        playerCharacter_mut->velocity.vel = Vector2Zeros;

        Vector2 curFrameMovementDirection = Vector2Zeros;

        Vector2 curFrameMovement = Vector2Zeros;

        float speed = 100.0f;
        float deltaTime = GetFrameTime();


        if (!attackClosed) {
            if (attackCloseTime <= GetTime()) {
                //std::cout << "Stopped Attack!" << std::endl;

                playerCharacterStates_mut->attackingSide = false;
                playerCharacterStates_mut->attackingDown = false;
                playerCharacterStates_mut->attackingUp = false;
                attackClosed = true;
            }
        }



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

        if (IsKeyPressed(KEY_SPACE) && !IsCharacterAttacking(*playerCharacterStates_mut)) {
            //std::cout << "Attacking!" << std::endl;

            if (curFrameMovementDirection.x != 0.0f/* && curDirection.y == 0.0f*/) {
                playerCharacterStates_mut->attackingSide = true;
            }
            else if (curFrameMovementDirection.x == 0.0f) {
                if (curFrameMovementDirection.y == -1.0f) {
                    playerCharacterStates_mut->attackingDown = true;
                }
                else if (curFrameMovementDirection.y == 1.0f) {
                    playerCharacterStates_mut->attackingUp = true;
                }
            }

            attackCloseTime = GetTime() + attackingTime;
            attackClosed = false;
        }

        if (!Vector2Equals(curFrameMovementDirection, previousMovementDirection)) {
            //std::cout << "Reset facing direction." << std::endl;
            if (curFrameMovementDirection.x != 0) {
                playerCharacter_mut->facingDirection.x = curFrameMovementDirection.x;
            }
            playerCharacter_mut->facingDirection.y = 1.0f;
        }

        Vector2 curFrameVel = curFrameMovementDirection * Vector2{ 1.0f, -1.0f } * speed;
        curFrameMovement = curFrameVel * deltaTime;

        if (IsCharacterAttacking(*playerCharacterStates_mut)) {
            //std::cout << "Stop moving while attacking." << std::endl;
            curFrameMovement = Vector2Zeros;
        }

        playerCharacter_mut->position.pos += curFrameMovement;

        playerCharacter_mut->velocity.vel = curFrameVel;
        playerCharacterStates_mut->running = Vector2Length(playerCharacter_mut->velocity.vel) != 0.0f;
        playerCharacterStates_mut->idle = Vector2Length(playerCharacter_mut->velocity.vel) == 0.0f;

        Vector2 goblinDirToPlayer = Vector2Subtract(playerCharacter_mut->position.pos, goblinCharacter_mut->position.pos);
        float distanceToPlayer = Vector2Length(goblinDirToPlayer);
        goblinDirToPlayer = Vector2Normalize(goblinDirToPlayer);

        goblinCharacter_mut->facingDirection = goblinDirToPlayer * Vector2{ 1 / abs(goblinDirToPlayer.x), 1 / abs(goblinDirToPlayer.y) };

        if (distanceToPlayer >= 200.0f && !IsCharacterAttacking(*goblinCharacterStates_mut)) {
            goblinCharacter_mut->velocity.vel = goblinDirToPlayer * speed;
            goblinCharacter_mut->position.pos = goblinCharacter_mut->position.pos + goblinCharacter_mut->velocity.vel * deltaTime;

            goblinCharacterStates_mut->attackingSide = false;
            goblinCharacterStates_mut->running = true;
            goblinCharacterStates_mut->idle = false;
        }
        else if(distanceToPlayer <= 150.0f) {
            //std::cout << "goblin attacking." << GetTime() << std::endl;
            //std::cout << "goblin attacking." << GetTime() << std::endl;
            goblinCharacterStates_mut->attackingSide = true;
            goblinCharacterStates_mut->running = false;
            goblinCharacterStates_mut->idle = false;
        }
        else {
            //std::cout << "goblin stopped attacking." << GetTime() << std::endl;
            goblinCharacterStates_mut->attackingSide = false;
            goblinCharacterStates_mut->running = false;
            goblinCharacterStates_mut->idle = true;
        }

        UpdatePlayerAnimationGraphSystem.run();

        //std::cout << playerCharacter_mut->position.pos.x << ", " << playerCharacter_mut->position.pos.y << std::endl;

        Camera2D* camera = e_Camera2D.get_mut<Camera2D>();
        Vector2 posX = Vector2{ playerCharacter_mut->position.pos.x, camera->target.y };
        Vector2 posY = Vector2{ camera->target.x, playerCharacter_mut->position.pos.y };

        auto e_curRoom = roomEntities[curRoomIndex];

        if (CanCameraMoveInRoom(e_curRoom.get_mut<Position>()->pos, posX, *camera)) {
            //std::cout << "Inside : " << GetTime() << std::endl;
            camera->target.x = posX.x;
        }
        if (CanCameraMoveInRoom(e_curRoom.get_mut<Position>()->pos, posY, *camera)) {
            //std::cout << "Inside : " << GetTime() << std::endl;
            camera->target.y = posY.y;
        }

        camera->zoom += ((float)GetMouseWheelMove() * 0.05f);


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

        AnimationGraphDrawingSystem.run();
        //std::cout << "After draw: " << playerCharacterStates_mut->attackingSide << std::endl;


        float resolution = screenWidth / screenHeight;
        float width = screenWidth / 12;
        float height = width / resolution;
        //Vector2 attackFacingDebugPos = curPlayerPos->pos + (curDirection * Vector2{ width, height });
        //DrawRectangle(attackFacingDebugPos.x, attackFacingDebugPos.y, (int)width, (int)height, RED);

        //DrawCircle(curPlayerPos->pos.x, curPlayerPos->pos.y, 10.0f, RED);
        DrawRectangleLines(playerCharacter_mut->position.pos.x - (width * 0.5f), playerCharacter_mut->position.pos.y - (height * 0.5f), width, height, RED);

        EndMode2D();

        DrawFPS(40, 40);

        EndDrawing();

        previousMovementDirection = curFrameMovementDirection;
    }

    CloseWindow();        // Close window and OpenGL context

    return 0;
}
