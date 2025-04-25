#pragma once
#include <string>
#include "glm/vec3.hpp"

class ShaderProgram;

namespace Utility {

    unsigned LoadTexture(const char *file_path, bool nearest = false);

    void DrawText(ShaderProgram &program,
                  unsigned       font_texture,
                  const std::string &text,
                  float          size,
                  float          spacing,
                  const glm::vec3 &position);

    inline void draw_text(ShaderProgram *program,
                          unsigned       font_texture,
                          const std::string &text,
                          float          size,
                          float          spacing,
                          const glm::vec3 &position)
    {
        DrawText(*program, font_texture, text, size, spacing, position);
    }
}
