//
//  File.m
//  SDLProject
//
//  Created by Chen Kevin on 4/7/25.
//  Copyright © 2025 ctg. All rights reserved.
//


/**
* Author: Kevin
* Assignment: ...
*/
#pragma once
#include "Scene.h"

class Level1 : public Scene {
private:
    GLuint m_map_texture_id;
    GLuint m_player_texture_id;
    GLuint m_enemy_texture_id;

    float m_camera_x;

public:
    void initialise() override;
    void process_input() override;
    void update(float dt) override;
    void render(ShaderProgram *program) override;
};
