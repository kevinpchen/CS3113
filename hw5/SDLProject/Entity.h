#pragma once
#include "glm/vec3.hpp"
class ShaderProgram;

class Entity {
public:
    glm::vec3 position{0}, velocity{0};
    float width=1, height=1;
    bool  active=true;
    unsigned textureID=0;

    void Update(float dt){ if(active) position += velocity*dt; }
    void Render(ShaderProgram &) const;
    bool CheckCollision(const Entity &) const;
};
