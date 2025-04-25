#include "MenuScene.h"
#include "Level1.h"
#include "Utility.h"
#include "ShaderProgram.h"


extern void switch_scene(Scene*);

void MenuScene::initialise()
{
    m_font = Utility::LoadTexture("assets/textures/font1.png");
}

void MenuScene::process_input(const SDL_Event& e)
{
    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN)
    {
        switch_scene(m_level1);     
    }
}

void MenuScene::render(ShaderProgram *prog)
{
    glClear(GL_COLOR_BUFFER_BIT);

    Utility::draw_text(prog, m_font,
                       "Space Shooter", 0.6f, 0.05f,
                       {-3.2f, 1.0f, 0.0f});

    Utility::draw_text(prog, m_font,
                       "PRESS ENTER TO START", 0.4f, 0.05f,
                       {-4.0f,-2.0f, 0.0f});
}
