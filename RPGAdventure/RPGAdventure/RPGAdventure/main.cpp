
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
#include "TileMap.h"

#include <string>
#include <vector>

const int screenWidth = 1280;
const int screenHeight = 720;

class Position {

public:
    Vector2 pos;
};

struct Player{public:};

const int worldSizeX = 100;
const int worldSizeY = 100;

int main(void)
{
    flecs::world world;

    auto e_Player = world.entity("Player");
    e_Player.add<Player>();
    e_Player.add<Position>();
    e_Player.add<SpriteSheet>();
    e_Player.add<TextureResource>();
    e_Player.add<SpriteAnimation>();

    e_Player.set<TextureResource>({ knightCharacterSpriteSheet, knightCharacterSpriteSheet_numSpriteCellsX, knightCharacterSpriteSheet_numSpriteCellsY });
    e_Player.set<Position>({ Vector2{0.0f, 0.0f } });

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

    e_tileMapGround.set<TextureResource>({ terrainTopFlatTileMap, terrainTopFlatTileMap_numSpriteCellsX, terrainTopFlatTileMap_numSpriteCellsY });
    e_tileMapGround.set<Vector3>({ Vector3 {0.0f, 0.0f, 0.0f} });

    std::vector<Vector2> tileTextureIndexData(worldSizeX * worldSizeY, { 0.0f, 0.0f });

    auto InitSpriteSheetSystem = world.system<SpriteSheet, TextureResource>()
        .kind(flecs::OnStart)
        .each([](flecs::iter& it, size_t, SpriteSheet& ss, TextureResource& tr) {
            std::cout << "Init Sprite Sheet System." << std::endl;
            InitSpriteSheet(ss, tr.texSrc, tr.numSpriteCellsX, tr.numSpriteCellsY);
        });

    auto InitSpriteSheetAnimationSystem = world.system<SpriteSheet, SpriteAnimation>()
        .kind(flecs::OnStart)
        .each([](flecs::iter& it, size_t, SpriteSheet& ss, SpriteAnimation& spriteAnimation) {
            std::cout << "Init Sprite Sheet Animation System." << std::endl;
            InitSpriteAnimation(spriteAnimation, 0, ss.numSpriteCellsX, ss.numSpriteCellsX, true, ss);
        });

    auto InitTileMapSystem = world.system<TextureResource, TileMap>()
        .kind(flecs::OnStart)
        .each([&tileTextureIndexData](flecs::iter& it, size_t, TextureResource& tr, TileMap& tm) {
            InitSpriteSheet(tm.tilePallet, tr.texSrc, tr.numSpriteCellsX, tr.numSpriteCellsY);

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

    //auto PlayerMovementSystem = world.system<Player, Position>()
    //    .kind(flecs::PreUpdate)
    //    .each([](flecs::iter& it, size_t, Position& position) {
    //        if (IsKeyPressed(KEY_RIGHT)) {
    //            position.pos.x += 1 * it.delta_time();
    //        }
    //        if (IsKeyPressed(KEY_LEFT)) {
    //            position.pos.x -= 1 * it.delta_time();
    //        }
    //        if (IsKeyPressed(KEY_UP)) {
    //            position.pos.y += 1 * it.delta_time();
    //        }
    //        if (IsKeyPressed(KEY_DOWN)) {
    //            position.pos.y -= 1 * it.delta_time();
    //        }
    //    });

    //auto CameraFollowSystem = world.system<Camera2D>()
    //    .kind(flecs::OnUpdate)
    //    .each([e_Player](flecs::iter& it, size_t, Camera2D& camera)
    //    {
    //            const Position* playerPos = e_Player.get<Position>();
    //            camera.target = playerPos->pos;
    //    });

    auto UpdateSpriteSheetAnimationSystem = world.system<SpriteAnimation>()
        .kind(flecs::OnUpdate)
        .each([](flecs::iter& it, size_t, SpriteAnimation& spriteAnimation) {
            //std::cout << "Update Sprite Sheet Animation System." << std::endl;
            UpdateAnimation(spriteAnimation);
        });

    auto SpriteSheetAnimationDrawingSystem = world.system<SpriteSheet, SpriteAnimation, Position>()
        .kind(flecs::OnUpdate)
        .each([](flecs::iter& it, size_t, SpriteSheet& ss, SpriteAnimation& spriteAnimation, Position& position) {
            //std::cout << "Update Sprite Sheet Animation Drawing System." << std::endl;
            DrawTextureRec(ss.spriteSheetTexture, spriteAnimation.curFrameView, position.pos, WHITE);
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

    //auto e = world.entity("Test Sprite");
    //e.add<TextureResource>();
    //e.add<SpriteSheet>();
    //e.add<SpriteAnimation>();
    //e.add<Vector2>();

    //e.set<TextureResource>({ terrainTopFlatTileMap, terrainTopFlatTileMap_numSpriteCellsX, terrainTopFlatTileMap_numSpriteCellsY });
    //e.set<Vector2>({ Vector2{350.0f, 280.0f} });

    InitSpriteSheetSystem.run();
    InitSpriteSheetAnimationSystem.run();

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


    while (!WindowShouldClose())
    {
        UpdateSpriteSheetAnimationSystem.run();

        //PlayerMovementSystem.run();
        //CameraFollowSystem.run();

        Vector2 curFrameMovement = Vector2Zeros;

        float speed = 100.0f;
        float deltaTime = GetFrameTime();
        Position* curPlayerPos = e_Player.get_mut<Position>();
        if (IsKeyDown(KEY_RIGHT)) {
            curPlayerPos->pos.x += speed * deltaTime;
            //camera->target.x += speed * deltaTime;
            curFrameMovement.x -= speed * deltaTime;
        }
        if (IsKeyDown(KEY_LEFT)) {
            curPlayerPos->pos.x -= speed * deltaTime;
            //camera->target.x -= speed * deltaTime;
            curFrameMovement.x += speed * deltaTime;
        }
        if (IsKeyDown(KEY_UP)) {
            curPlayerPos->pos.y -= speed * deltaTime;
            //camera->target.y -= speed * deltaTime;
            curFrameMovement.y -= speed * deltaTime;
        }
        if (IsKeyDown(KEY_DOWN)) {
            curPlayerPos->pos.y += speed * deltaTime;
            //camera->target.y += speed * deltaTime;
            curFrameMovement.y += speed * deltaTime;
        }

        const Position* playerPos = e_Player.get<Position>();
        Camera2D* camera = e_Camera2D.get_mut<Camera2D>();
        camera->target = playerPos->pos;
        camera->zoom += ((float)GetMouseWheelMove() * 0.05f);
        //e_Camera2D.get_mut<Camera2D>()->offset = playerPos->pos;

        //std::cout << curPlayerPos->pos.x << ", " << curPlayerPos->pos.y << std::endl;
        //std::cout << camera->target.x << ", " << camera->target.y << std::endl;

        BeginDrawing();

        ClearBackground(RAYWHITE);

        //TileMapDrawingSystem.run();

        //const TileMap* tm = e_tileMapGround.get<TileMap>();

        //BeginMode2D(camera);
        //for (int x = 0; x < 10; x++)
        //{
        //    for (int y = 0; y < 10; y++)
        //    {
        //        DrawTextureRec(tm->tilePallet.spriteSheetTexture, tm->tiles[x][y].tileInSpriteSheet, Vector2{ (float)x, (float)y }, WHITE);
        //    }
        //}
        //EndMode2D();

        if(true)
        {
            rlEnableShader(simpleTileMapRenderingShader.id);

            Vector2 cameraScreenPos = GetWorldToScreen2D(camera->target, *camera);
            Vector2 cameraPos = camera->target;
            Vector2 curFrameMovementInPixels = GetWorldToScreen2D(curFrameMovement, *camera);

            //Vector2 diffPixels = cameraScreenPos - curFrameMovementInPixels;

            //curFrameMovementInPixels.x = curFrameMovementInPixels.x * movementX;
            //std::cout << curFrameMovementInPixels.x << ", " << curFrameMovementInPixels.y << std::endl;
            //std::cout << curFrameMovement.x << ", " << curFrameMovement.y << std::endl;
            //std::cout << diffPixels.x << ", " << diffPixels.y << std::endl;

            SetShaderValue(simpleTileMapRenderingShader, cameraScreenPosInSimpleTileMapRenderingShader, &cameraScreenPos, SHADER_UNIFORM_VEC2);
            SetShaderValue(simpleTileMapRenderingShader, cameraPosInSimpleTileMapRenderingShader, &cameraPos, SHADER_UNIFORM_VEC2);
            SetShaderValue(simpleTileMapRenderingShader, movementInPixelsInSimpleTileMapRenderingShader, &curFrameMovementInPixels, SHADER_UNIFORM_VEC2);


            rlActiveTextureSlot(0);
            rlEnableTexture(e_tileMapGround.get<TileMap>()->tilePallet.spriteSheetTexture.id);
            //rlEnableTexture(textureLoad.id);

            rlEnableVertexArray(quadVAO);
            //rlDrawVertexArray(0, 4);
            rlDrawVertexArrayElements(0, 6, 0);

            rlEnableVertexArray(0);

            rlDisableShader();
        }

        BeginMode2D(*camera);

        SpriteSheetAnimationDrawingSystem.run();

        EndMode2D();

        DrawFPS(40, 40);

        EndDrawing();
    }

    CloseWindow();        // Close window and OpenGL context

    return 0;
}
