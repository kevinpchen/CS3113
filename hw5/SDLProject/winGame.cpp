#include "winGame.h"
#include "Utility.h"
#include "ShaderProgram.h"

#ifdef _WINDOWS
#   include <GL/glew.h>
#endif
#define GL_GLEXT_PROTOTYPES 1
#include <SDL_opengl.h>

void winGame::initialise()
{
    m_font = Utility::LoadTexture("assets/textures/font1.png");
}

void winGame::render(ShaderProgram *prog)
{
    glClear(GL_COLOR_BUFFER_BIT);

    Utility::draw_text(prog, m_font,
                       "YOU WIN!", 0.6f, 0.05f,
                       glm::vec3(-2.8f, 0.0f, 0.0f));
}
