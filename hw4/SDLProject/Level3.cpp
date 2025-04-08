/**
* Author: Kevin Chen
* Assignment: Rise of the AI
* Date due: 2025-04-05, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "Level3.h"
#include "Utility.h"
#include <string>

#define LEVEL_WIDTH 14
#define LEVEL_HEIGHT 8

constexpr char PLAYER_TEXTURE_PATH_C[] = "assets/keqing.png";
constexpr char ENEMY_TEXTURE_PATH_C[]  = "assets/enemy.png";
constexpr char MAP_TEXTURE_PATH_C[]    = "assets/greenland.png";
constexpr char FONT_TEXTURE_PATH_C[]   = "assets/font1.png";

GLuint g_font_texture_id_C;

static const int ENEMY_NUM_C = 5;

static unsigned int LEVEL_DATA_C[] =
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

Level3::~Level3()
{
    delete[] m_game_state.enemies;
    delete   m_game_state.player;
    delete   m_game_state.map;

    Mix_FreeChunk(m_game_state.jump_sfx);
    Mix_FreeMusic(m_game_state.bgm);
}

void Level3::initialise()
{
    GLuint map_tex_id = Utility::load_texture(MAP_TEXTURE_PATH_C);
    m_game_state.map = new Map(
        LEVEL_WIDTH,
        LEVEL_HEIGHT,
        LEVEL_DATA_C,
        map_tex_id,
        1.0f,
        16,
        16
    );
    
    g_font_texture_id_C = Utility::load_texture(FONT_TEXTURE_PATH_C);

    GLuint player_tex_id = Utility::load_texture(PLAYER_TEXTURE_PATH_C);
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
    
    GLuint enemy_tex_id = Utility::load_texture(ENEMY_TEXTURE_PATH_C);
    m_game_state.enemies = new Entity[ENEMY_NUM_C];
    m_game_state.enemies[0] = Entity(enemy_tex_id, 1.0f, 1.0f, 1.0f, ENEMY, FLYER,  FLYING);
    m_game_state.enemies[1] = Entity(enemy_tex_id, 1.0f, 1.0f, 1.0f, ENEMY, FLYER,  FLYING);
    m_game_state.enemies[2] = Entity(enemy_tex_id, 1.0f, 1.0f, 1.0f, ENEMY, FLYER,  FLYING);
    m_game_state.enemies[3] = Entity(enemy_tex_id, 1.0f, 1.0f, 1.0f, ENEMY, GUARD, IDLE);
    m_game_state.enemies[4] = Entity(enemy_tex_id, 1.0f, 1.0f, 1.0f, ENEMY, GUARD, IDLE);

    m_game_state.enemies[0].set_position(glm::vec3(3.0f, -5.0f, 0.0f));
    m_game_state.enemies[1].set_position(glm::vec3(5.0f, -5.0f, 0.0f));
    m_game_state.enemies[2].set_position(glm::vec3(7.0f, -5.0f, 0.0f));
    m_game_state.enemies[3].set_position(glm::vec3(4.0f, -6.0f, 0.0f));
    m_game_state.enemies[4].set_position(glm::vec3(9.0f, -6.0f, 0.0f));

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    m_game_state.bgm        = Mix_LoadMUS("assets/bgm.mp3");
    m_game_state.jump_sfx   = Mix_LoadWAV("assets/jump.mp3");
    m_game_state.die_effects= Mix_LoadWAV("assets/player_dead.mp3");
    m_game_state.win        = Mix_LoadWAV("assets/win.mp3");
    

    Mix_PlayMusic(m_game_state.bgm, -1);
    Mix_VolumeMusic(30);
}

void Level3::update(float delta_time)
{
    m_game_state.player->update(delta_time,
                                m_game_state.player,
                                m_game_state.enemies,
                                ENEMY_NUM_C,
                                m_game_state.map);

    for (int i = 0; i < ENEMY_NUM_C; i++)
    {
        m_game_state.enemies[i].update(
            delta_time,
            m_game_state.player,
            nullptr,
            0,
            m_game_state.map
        );
    }

    if (m_game_state.player->get_position().y <= -6.3f)
    {
        Mix_PlayChannel(-1, m_game_state.win, 0);
        return;
    }

    m_game_state.player->update_cooldown(3.0f);
    for (int i = 0; i < ENEMY_NUM_C; i++)
    {
        if (m_game_state.enemies[i].is_active())
        {
            if (m_game_state.player->check_collision(&m_game_state.enemies[i]))
            {
                m_game_state.player->dec_live();
                Mix_PlayChannel(-1, m_game_state.die_effects, 0);
                m_game_state.player->set_position(glm::vec3(1.0f, -6.0f, 0.0f));
            }
        }
    }
}

void Level3::render(ShaderProgram* program)
{
    m_game_state.map->render(program);
    m_game_state.player->render(program);

    for (int i = 0; i < ENEMY_NUM_C; i++)
    {
        m_game_state.enemies[i].render(program);
    }

    std::string lives_msg = "LIVES:" + std::to_string(m_game_state.player->get_lives());
    Utility::draw_text(program, g_font_texture_id_C, lives_msg, 0.4f, 0.05f, glm::vec3(1, -1, 0));

    std::string level_msg = "LEVEL 3";
    Utility::draw_text(program, g_font_texture_id_C, level_msg, 0.4f, 0.05f, glm::vec3(1, -1.5f, 0));
}
