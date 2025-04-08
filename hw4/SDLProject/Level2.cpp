/**
* Author: Kevin Chen
* Assignment: Rise of the AI
* Date due: 2025-04-05, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on Academic Misconduct.
**/

#include "Level2.h"
#include "Utility.h"
#include <string>

#define LEVEL_WIDTH 14
#define LEVEL_HEIGHT 8

constexpr char PLAYER_TEXTURE_PATH[] = "assets/keqing.png";
constexpr char ENEMY_TEXTURE_PATH[]  = "assets/enemy.png";
constexpr char MAP_TEXTURE_PATH[]    = "assets/greenland.png";
constexpr char FONT_TEXTURE_PATH[]   = "assets/font1.png";

GLuint g_font_texture_id_B;
static const int ENEMY_NUM_2 = 2;

static unsigned int LEVEL_DATA_2[] =
{
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

Level2::~Level2()
{
    delete[] m_game_state.enemies;
    delete   m_game_state.player;
    delete   m_game_state.map;

    Mix_FreeChunk(m_game_state.jump_sfx);
    Mix_FreeMusic(m_game_state.bgm);
    Mix_FreeChunk(m_game_state.die_effects);
}

void Level2::initialise()
{
    GLuint map_texture_id = Utility::load_texture(MAP_TEXTURE_PATH);
    m_game_state.map = new Map(
        LEVEL_WIDTH,
        LEVEL_HEIGHT,
        LEVEL_DATA_2,
        map_texture_id,
        1.0f,
        16,
        16
    );
    g_font_texture_id_B = Utility::load_texture(FONT_TEXTURE_PATH);
    GLuint player_tex_id = Utility::load_texture(PLAYER_TEXTURE_PATH);
    m_game_state.player = new Entity(
        player_tex_id,
        5.0f,
        1.0f,
        1.0f,
        PLAYER
    );
    m_game_state.player->set_position(glm::vec3(1.0f, -6.0f, 0.0f));
    m_game_state.player->set_jumping_power(10.0f);
    m_game_state.player->set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    GLuint enemy_tex_id = Utility::load_texture(ENEMY_TEXTURE_PATH);
    m_game_state.enemies = new Entity[ENEMY_NUM_2];
    m_game_state.enemies[0] = Entity(enemy_tex_id, 1.0f, 1.0f, 1.0f, ENEMY, GUARD, IDLE);
    m_game_state.enemies[1] = Entity(enemy_tex_id, 1.0f, 1.0f, 1.0f, ENEMY, GUARD, IDLE);
    m_game_state.enemies[0].set_position(glm::vec3(4.0f, -6.0f, 0.0f));
    m_game_state.enemies[1].set_position(glm::vec3(9.0f, -6.0f, 0.0f));
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    m_game_state.bgm        = Mix_LoadMUS("assets/bgm.mp3");
    m_game_state.jump_sfx   = Mix_LoadWAV("assets/jump.mp3");
    m_game_state.die_effects= Mix_LoadWAV("assets/player_dead.mp3");
    m_game_state.win        = Mix_LoadWAV("assets/win.mp3");
    Mix_PlayMusic(m_game_state.bgm, -1);
    Mix_VolumeMusic(30);
}

void Level2::update(float delta_time)
{
    m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_NUM_2, m_game_state.map);
    for (int i = 0; i < ENEMY_NUM_2; i++)
    {
        m_game_state.enemies[i].update(delta_time, m_game_state.player, nullptr, 0, m_game_state.map);
    }
    if (m_game_state.player->get_position().y <= -6.3f)
    {
        Mix_PlayChannel(-1, m_game_state.win, 0);
        return;
    }
    if (m_game_state.player->get_damage_cooldown() <= 0.0f) {
        bool collisionDetected = false;
        for (int i = 0; i < ENEMY_NUM_2; i++) {
            if (m_game_state.player->check_collision(&m_game_state.enemies[i])) {
                collisionDetected = true;
                break;
            }
        }
        if (collisionDetected) {
            m_game_state.player->dec_live();
            Mix_PlayChannel(-1, m_game_state.die_effects, 0);
            m_game_state.player->set_position(glm::vec3(1.0f, -6.0f, 0.0f));
        }
    }
}

void Level2::render(ShaderProgram* program)
{
    m_game_state.map->render(program);
    m_game_state.player->render(program);
    for (int i = 0; i < ENEMY_NUM_2; i++)
    {
        m_game_state.enemies[i].render(program);
    }
    std::string lives_msg = "LIVES:" + std::to_string(m_game_state.player->get_lives());
    Utility::draw_text(program, g_font_texture_id_B, lives_msg, 0.4f, 0.05f, glm::vec3(1, -1, 0));
    std::string level_msg = "LEVEL 2";
    Utility::draw_text(program, g_font_texture_id_B, level_msg, 0.4f, 0.05f, glm::vec3(1, -1.5f, 0));
}
