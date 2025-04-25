#include "Map.h"
#include "Utility.h"
#include "ShaderProgram.h"
#include "glm/mat4x4.hpp"

#ifdef _WINDOWS
#   include <GL/glew.h>
#endif
#define GL_GLEXT_PROTOTYPES 1
#include <SDL_opengl.h>

static const float Q[]  = { -1,-1,  1,-1,  1,1,  -1,1 };
static const float UV[] = {  0,1,  1,1,  1,0,   0,0 };

Map::Map(const char *p) : m_path(p) { }      

void Map::Render(ShaderProgram &prog)
{
  
    if (m_tex == 0)
        m_tex = Utility::LoadTexture(m_path, true);

    prog.set_model_matrix(glm::mat4(1.0f));
    glUseProgram(prog.get_program_id());

    GLuint pos = prog.get_position_attribute();
    GLuint uv  = prog.get_tex_coordinate_attribute();

    glBindTexture(GL_TEXTURE_2D, m_tex);
    glEnableVertexAttribArray(pos);
    glEnableVertexAttribArray(uv);
    glVertexAttribPointer(pos, 2, GL_FLOAT, false, 0, Q);
    glVertexAttribPointer(uv,  2, GL_FLOAT, false, 0, UV);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDisableVertexAttribArray(pos);
    glDisableVertexAttribArray(uv);
}
