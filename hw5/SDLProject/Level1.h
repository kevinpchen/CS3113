#pragma once
#include "Scene.h"
#include "Entity.h"
#include <vector>

class Level1 : public Scene {
    GameState           m_state;
    std::vector<Entity> m_enemies, m_bullets;
    float               m_spawn_timer = 0.0f;

    int        m_kills       = 0;
    const int  m_targetKills = 40;
    unsigned   m_font        = 0;
    Mix_Chunk *m_sfxEnemyDie = nullptr;
    Mix_Chunk *m_sfxPlayerDie= nullptr;

    void spawn_enemy();
public:
    void initialise() override;
    void process_input(const SDL_Event&) override;
    void update(float dt) override;
    void render(ShaderProgram*) override;
    GameState& get_state() override { return m_state; }
};
