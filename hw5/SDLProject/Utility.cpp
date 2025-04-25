#include "Utility.h"
#include "ShaderProgram.h"
#include <SDL_image.h>

#ifdef _WINDOWS
#   include <GL/glew.h>
#endif
#define GL_GLEXT_PROTOTYPES 1
#include <SDL_opengl.h>
#include "glm/gtc/matrix_transform.hpp"

namespace Utility {

unsigned LoadTexture(const char *file, bool nearest)
{
    SDL_Surface *surf = IMG_Load(file);
    if (!surf) { SDL_Log("IMG_Load failed: %s", IMG_GetError()); return 0; }

    GLuint tex; glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                 surf->w, surf->h, 0,
                 surf->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB,
                 GL_UNSIGNED_BYTE, surf->pixels);

    int filter = nearest ? GL_NEAREST : GL_LINEAR;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    SDL_FreeSurface(surf);
    return tex;
}

void DrawText(ShaderProgram &prog,
              unsigned font,
              const std::string &text,
              float size,
              float spacing,
              const glm::vec3 &start)
{
    const int   cols  = 16;
    const float glyph = 1.0f / cols;
    float cursor = 0;

    GLuint aPos = prog.get_position_attribute();
    GLuint aUV  = prog.get_tex_coordinate_attribute();

    for (char ch : text)
    {
        int idx = static_cast<unsigned char>(ch);
        float u = (idx % cols) * glyph;
        float v = (idx / cols) * glyph;

        glm::mat4 model = glm::translate(glm::mat4(1.0f),
                                         start + glm::vec3(cursor, 0, 0)) *
                          glm::scale(glm::mat4(1.0f), glm::vec3(size));

        prog.set_model_matrix(model);
        glUseProgram(prog.get_program_id());

        const float quad[8] = { -0.5f,-0.5f,  0.5f,-0.5f,  0.5f,0.5f,  -0.5f,0.5f };
        float uvs[8] = {
            u,          v + glyph,
            u + glyph,  v + glyph,
            u + glyph,  v,
            u,          v
        };

        glBindTexture(GL_TEXTURE_2D, font);
        glEnableVertexAttribArray(aPos);
        glEnableVertexAttribArray(aUV);
        glVertexAttribPointer(aPos, 2, GL_FLOAT, false, 0, quad);
        glVertexAttribPointer(aUV , 2, GL_FLOAT, false, 0, uvs);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glDisableVertexAttribArray(aPos);
        glDisableVertexAttribArray(aUV);

        cursor += size + spacing;
    }
}

} 
