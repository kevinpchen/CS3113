/**
* Author: Kevin Chen
* Assignment: Rise of the AI
* Date due: 2025-04-05, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define LOG(argument) std::cout << argument << '\n'
#define STB_IMAGE_IMPLEMENTATION
#define NUMBER_OF_TEXTURES 1
#define LEVEL_OF_DETAIL    0
#define TEXTURE_BORDER     0
#define FONTBANK_SIZE      16

#include "Utility.h"
#include <SDL_image.h>
#include "stb_image.h"

GLuint Utility::load_texture(const char* filepath)
{
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);
    
    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct: " << filepath);
        assert(false);
    }
    
    GLuint texture_id;
    glGenTextures(NUMBER_OF_TEXTURES, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER,
                 GL_RGBA, GL_UNSIGNED_BYTE, image);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    stbi_image_free(image);
    return texture_id;
}

void Utility::draw_text(ShaderProgram *program, GLuint font_texture_id, std::string text,
                        float screen_size, float spacing, glm::vec3 position)
{
    float width  = 1.0f / FONTBANK_SIZE;
    float height = 1.0f / FONTBANK_SIZE;

    std::vector<float> vertices;
    std::vector<float> texture_coordinates;
    
    for (int i = 0; i < (int)text.size(); i++)
    {
        int sprite_index = (int)text[i];
        float offset = (screen_size + spacing) * i;
        
        float u_coord = (float)(sprite_index % FONTBANK_SIZE) / (float)FONTBANK_SIZE;
        float v_coord = (float)(sprite_index / FONTBANK_SIZE) / (float)FONTBANK_SIZE;

        // 6 vertices per character
        vertices.insert(vertices.end(), {
            offset + (-0.5f * screen_size),  0.5f * screen_size,
            offset + (-0.5f * screen_size), -0.5f * screen_size,
            offset + ( 0.5f * screen_size),  0.5f * screen_size,

            offset + ( 0.5f * screen_size), -0.5f * screen_size,
            offset + ( 0.5f * screen_size),  0.5f * screen_size,
            offset + (-0.5f * screen_size), -0.5f * screen_size
        });

        texture_coordinates.insert(texture_coordinates.end(), {
            u_coord,                 v_coord,
            u_coord,                 v_coord + height,
            u_coord + width,        v_coord,
            
            u_coord + width,        v_coord + height,
            u_coord + width,        v_coord,
            u_coord,                v_coord + height
        });
    }

    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix = glm::translate(model_matrix, position);
    
    program->set_model_matrix(model_matrix);
    glUseProgram(program->get_program_id());
    
    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->get_position_attribute());
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0,
                          texture_coordinates.data());
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());
    
    glBindTexture(GL_TEXTURE_2D, font_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));
    
    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}
