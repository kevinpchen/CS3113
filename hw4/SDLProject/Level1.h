/**
* Author: Kevin Chen
* Assignment: Rise of the AI
* Date due: 2025-04-05, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#ifndef LEVEL1_H
#define LEVEL1_H

#include "Scene.h"

class Level1 : public Scene {
public:
    int ENEMY_COUNT = 1;
    ~Level1();

    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram *program) override;
};

#endif
