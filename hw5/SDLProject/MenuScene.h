#pragma once
#include "Scene.h"


class Level1;

class MenuScene : public Scene {
    GameState m_state;
    unsigned  m_font;
    Level1   *m_level1;
public:
    explicit MenuScene(Level1 *firstLevel) : m_level1(firstLevel) {}
    void initialise() override;
    void process_input(const SDL_Event& e) override;   
    void update(float) override {}
    void render(ShaderProgram*) override;
    GameState& get_state() override { return m_state; }
};
