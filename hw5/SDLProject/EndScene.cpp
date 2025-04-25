#include "EndScene.h"
#include "Utility.h"
#include "ShaderProgram.h"

#ifdef _WINDOWS
#   include <GL/glew.h>
#endif
#define GL_GLEXT_PROTOTYPES 1
#include <SDL_opengl.h>

void EndScene::initialise()
{
    m_font = Utility::LoadTexture("assets/textures/font1.png");
}

void EndScene::render(ShaderProgram *prog)
{
    glClear(GL_COLOR_BUFFER_BIT);

    Utility::draw_text(
        prog,
        m_font,
        "GAME OVER",
        0.6f,
        0.05f,          
        glm::vec3(-3.0f, 0.0f, 0.0f));
}
