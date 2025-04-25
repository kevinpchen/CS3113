#pragma once
#include <SDL.h>
#include <SDL_mixer.h>
class Entity;

struct GameState {
    Entity   *player   = nullptr;
    Mix_Chunk*jump_sfx = nullptr;
    int       lives    = 3;           
};

class ShaderProgram;

class Scene {
public:
    virtual ~Scene() = default;
    virtual void initialise() = 0;
    virtual void process_input(const SDL_Event&) {}
    virtual void update(float dt) = 0;
    virtual void render(ShaderProgram*) = 0;
    virtual GameState& get_state() = 0;
};
