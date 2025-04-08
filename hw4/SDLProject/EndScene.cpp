/**
* Author: Kevin Chen
* Assignment: Rise of the AI
* Date due: 2025-04-05, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "endScene.h"
#include "Utility.h"
#include <string>
#include <vector>

#define LEVEL_WIDTH 14
#define LEVEL_HEIGHT 8

constexpr char SPRITESHEET_FILEPATH_END[] = "assets/keqing.png";
constexpr char PLATFORM_FILEPATH_END[]    = "assets/greenland.png";
constexpr char ENEMY_FILEPATH_END[]       = "assets/enemy.png";
constexpr char FONT_FILEPATH[]            = "assets/font1.png";

GLuint g_font_texture_id_END;

unsigned int LEVEL_DATA_END[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

EndScene::~EndScene()
{
    delete [] m_game_state.enemies;
    delete    m_game_state.player;
    delete    m_game_state.map;

    Mix_FreeChunk(m_game_state.jump_sfx);
    Mix_FreeMusic(m_game_state.bgm);
}

void EndScene::initialise()
{
    GLuint map_texture_id = Utility::load_texture("assets/greenland.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_DATA_END,
                               map_texture_id, 1.0f, 4, 1);

    g_font_texture_id_END = Utility::load_texture(FONT_FILEPATH);

    GLuint player_texture_id = Utility::load_texture(SPRITESHEET_FILEPATH_END);
    m_game_state.player = new Entity(
        player_texture_id,
        5.0f,
        1.0f,
        1.0f,
        PLAYER
    );
    m_game_state.player->set_position(glm::vec3(1.0f, -6.0f, 0.0f));

    GLuint enemy_texture_id = Utility::load_texture(ENEMY_FILEPATH_END);
    m_game_state.enemies = new Entity[ENEMY_COUNT];
    m_game_state.enemies[0] = Entity(
        enemy_texture_id,
        1.0f,
        1.0f,
        1.0f,
        ENEMY
    );
    m_game_state.enemies[0].set_position(glm::vec3(8.0f, 0.0f, 0.0f));
}

void EndScene::update(float delta_time)
{
}

void EndScene::render(ShaderProgram *program)
{
    glClear(GL_COLOR_BUFFER_BIT);

    std::string message = "YOU LOST";
    Utility::draw_text(
        program,
        g_font_texture_id_END,
        message,
        0.4f,
        0.05f,
        glm::vec3(3.0f, -2.0f, 0.0f)
    );
}
