#include "Entity.h"
#include "ShaderProgram.h"
#include "glm/gtc/matrix_transform.hpp"

#ifdef _WINDOWS
#   include <GL/glew.h>
#endif
#define GL_GLEXT_PROTOTYPES 1
#include <SDL_opengl.h>

static const float V[] ={-0.5f,-0.5f, 0.5f,-0.5f, 0.5f,0.5f, -0.5f,0.5f};
static const float U[] ={0,1, 1,1, 1,0, 0,0};
static const unsigned I[]={0,1,2,2,3,0};

void Entity::Render(ShaderProgram &prog) const
{
    if(!active) return;

    glm::mat4 model = glm::translate(glm::mat4(1), position) *
                      glm::scale(glm::mat4(1), glm::vec3(width,height,1));
    prog.set_model_matrix(model);
    glUseProgram(prog.get_program_id());

    GLuint aPos = prog.get_position_attribute();
    GLuint aUV  = prog.get_tex_coordinate_attribute();

    glBindTexture(GL_TEXTURE_2D, textureID);
    glEnableVertexAttribArray(aPos);
    glEnableVertexAttribArray(aUV);
    glVertexAttribPointer(aPos,2,GL_FLOAT,false,0,V);
    glVertexAttribPointer(aUV ,2,GL_FLOAT,false,0,U);
    glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,I);
    glDisableVertexAttribArray(aPos);
    glDisableVertexAttribArray(aUV);
}

bool Entity::CheckCollision(const Entity &o) const
{
    float dx = fabs(position.x - o.position.x) - (width+o.width)/2;
    float dy = fabs(position.y - o.position.y) - (height+o.height)/2;
    return active && o.active && dx<0 && dy<0;
}
