#include "Level1.h"
#include "Utility.h"
#include "ShaderProgram.h"
#include "Level2.h"
#include "EndScene.h"
#include <SDL_mixer.h>
#include <SDL.h>
#include "glm/gtc/matrix_transform.hpp"

extern void     switch_scene(Scene*);
extern Level2  *s_l2;
extern EndScene*s_end;

constexpr float OFFSCREEN_X = -5.3f;    

void Level1::initialise()
{
    m_font = Utility::LoadTexture("assets/textures/font1.png");

    m_state.player = new Entity;
    m_state.player->textureID = Utility::LoadTexture("assets/textures/player.png");
    m_state.player->width = m_state.player->height = 1.0f;
    m_state.player->position = {-4.8f, 0.0f, 0.0f};

    m_state.lives      = 3;
    m_state.jump_sfx   = Mix_LoadWAV("assets/sounds/shoot.wav");
    m_sfxEnemyDie      = Mix_LoadWAV("assets/sounds/damage.wav");
    m_sfxPlayerDie     = Mix_LoadWAV("assets/sounds/explosion.wav");
    if (!m_sfxPlayerDie) SDL_Log("explosion.wav load error: %s", Mix_GetError());
    else                 Mix_VolumeChunk(m_sfxPlayerDie, MIX_MAX_VOLUME);
}


void Level1::spawn_enemy()
{
    Entity e;
    e.textureID = Utility::LoadTexture("assets/textures/enemy1.png");
    e.width = e.height = 1.0f;


    e.position = { 5.2f, (rand()%700 - 350)/100.0f, 0.0f };
    e.velocity = { -2.0f, 0.0f, 0.0f };

    m_enemies.push_back(e);
}


void Level1::process_input(const SDL_Event &e)
{
    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE)
    {
        Mix_PlayChannel(-1, m_state.jump_sfx, 0);
        Entity b;
        b.textureID = Utility::LoadTexture("assets/textures/laser.png");
        b.width = b.height = 0.3f;
        b.position = m_state.player->position + glm::vec3(0.8f, 0, 0);
        b.velocity = { 8, 0, 0 };
        m_bullets.push_back(b);
    }
}

void Level1::update(float dt)
{
    const Uint8* k = SDL_GetKeyboardState(nullptr);
    if (k[SDL_SCANCODE_LEFT ])  m_state.player->position.x -= 5 * dt;
    if (k[SDL_SCANCODE_RIGHT])  m_state.player->position.x += 5 * dt;
    if (k[SDL_SCANCODE_UP   ])  m_state.player->position.y += 5 * dt;
    if (k[SDL_SCANCODE_DOWN ])  m_state.player->position.y -= 5 * dt;

    m_spawn_timer -= dt;
    if (m_spawn_timer <= 0){ m_spawn_timer = 1.2f; spawn_enemy(); }

    m_state.player->Update(dt);
    for (auto &b : m_bullets)  b.Update(dt);
    for (auto &e : m_enemies)  e.Update(dt);


    for (auto &b : m_bullets)
        for (auto &e : m_enemies)
            if (b.active && e.active && b.CheckCollision(e))
            {
                b.active = e.active = false;
                ++m_kills;
                Mix_PlayChannel(-1, m_sfxEnemyDie, 0);
            }


    for (auto &e : m_enemies)
        if (e.active && m_state.player->CheckCollision(e))
        {
            if (m_sfxPlayerDie)
                Mix_PlayChannel(-1, m_sfxPlayerDie, 0);
            if (--m_state.lives <= 0) { switch_scene(s_end); return; }
            m_state.player->position = {-4.8f, 0, 0};
            return;
        }


    for (auto &e : m_enemies)
        if (e.active && e.position.x < OFFSCREEN_X)
        {
            e.active = false;
            if (--m_state.lives <= 0) { switch_scene(s_end); return; }
        }

    if (m_kills >= m_targetKills) switch_scene(s_l2);
}


void Level1::render(ShaderProgram *p)
{
    m_state.player->Render(*p);
    for (auto &b : m_bullets) if (b.active) b.Render(*p);
    for (auto &e : m_enemies) if (e.active) e.Render(*p);

    Utility::draw_text(p, m_font,
        "Level: 1  Kills: " + std::to_string(m_kills) +
        "  Lives: " + std::to_string(m_state.lives),
        0.3f, 0.02f, glm::vec3(-4.9f, 3.2f, 0.0f));
}
