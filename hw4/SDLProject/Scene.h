/**
* Author: Kevin Chen
* Assignment: Rise of the AI
* Date due: 2025-04-05, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#ifndef SCENE_H
#define SCENE_H

#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Entity.h"
#include "Map.h"

struct GameState
{
    Map    *map;
    Entity *player;
    Entity *enemies;
    Entity *Background;
    
    Mix_Music *bgm;
    Mix_Chunk *jump_sfx;
    Mix_Chunk *die_effects;
    Mix_Chunk *win;
    
    int next_scene_id;
};

class Scene {
protected:
    GameState m_game_state;
    
public:
    int m_number_of_enemies = 1;
    
    virtual void initialise() = 0;
    virtual void update(float delta_time) = 0;
    virtual void render(ShaderProgram *program) = 0;
    
    GameState& get_state() { return m_game_state; }
    int const get_number_of_enemies() const { return m_number_of_enemies; }
};

#endif
