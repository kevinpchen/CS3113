#pragma once                       

#include "Scene.h"
#include "glm/vec3.hpp"

class winGame : public Scene {
    GameState m_state;
    unsigned  m_font;
public:
    void initialise() override;
    void update(float) override {}
    void render(ShaderProgram*) override;
    GameState& get_state() override { return m_state; }
};
