#pragma once
#include "Scene.h"

class EndScene : public Scene {
    GameState m_state;
    unsigned  m_font = 0;       

public:
    void initialise() override;
    void update(float) override {}
    void render(ShaderProgram*) override;
    GameState& get_state() override { return m_state; }
};
