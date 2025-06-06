/**
* Author: Kevin Chen
* Assignment: Rise of the AI
* Date due: 2025-04-05, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#ifndef UTILITY_H
#define UTILITY_H

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <string>
#include <vector>
#include <SDL.h>
#include <SDL_opengl.h>
#include <assert.h>
#include <iostream>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

class Utility {
public:
    static GLuint load_texture(const char* filepath);
    static void draw_text(ShaderProgram *program, GLuint font_texture_id, std::string text,
                          float screen_size, float spacing, glm::vec3 position);
};

#endif
