/**
* Author: Kevin Chen
* Assignment: Rise of the AI
* Date due: 2025-04-05, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "menuScene.h"
#include "Utility.h"
#include <string>
#include <vector>

#define LEVEL_WIDTH 14
#define LEVEL_HEIGHT 8

constexpr char SPRITESHEET_FILEPATH_M[] = "assets/keqing.png";
constexpr char PLATFORM_FILEPATH_M[]    = "assets/greenland.png";
constexpr char ENEMY_FILEPATH_M[]       = "assets/enemy.png";
constexpr char FONT_FILEPATH[]          = "assets/font1.png";

GLuint g_font_texture_id_M;

unsigned int MENU_LEVEL_DATA[] =
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

MenuScene::~MenuScene()
{
    delete [] m_game_state.enemies;
    delete    m_game_state.player;
    delete    m_game_state.map;
    Mix_FreeChunk(m_game_state.jump_sfx);
    Mix_FreeMusic(m_game_state.bgm);
}

void MenuScene::initialise()
{
    GLuint map_texture_id = Utility::load_texture("assets/greenland.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, MENU_LEVEL_DATA, map_texture_id, 1.0f, 4, 1);
    
    GLuint player_texture_id = Utility::load_texture(SPRITESHEET_FILEPATH_M);
    g_font_texture_id_M = Utility::load_texture(FONT_FILEPATH);

    m_game_state.player = new Entity(
        player_texture_id,
        5.0f,
        1.0f,
        1.0f,
        PLAYER
    );
    m_game_state.player->set_position(glm::vec3(1.0f, -6.0f, 0.0f));

    GLuint enemy_texture_id = Utility::load_texture(ENEMY_FILEPATH_M);
    m_game_state.enemies = new Entity[ENEMY_COUNT];
    m_game_state.enemies[0] = Entity(enemy_texture_id, 1.0f, 1.0f, 1.0f, ENEMY);
    m_game_state.enemies[0].set_position(glm::vec3(8.0f, 0.0f, 0.0f));
    
}

void MenuScene::update(float delta_time)
{
}

void MenuScene::render(ShaderProgram *g_shader_program)
{
    glClear(GL_COLOR_BUFFER_BIT);
    
    std::string message = "Press enter to start";
    Utility::draw_text(g_shader_program, g_font_texture_id_M, message, 0.4f, 0.05f, glm::vec3(0.5f, -2.0f, 0.0f));
}
