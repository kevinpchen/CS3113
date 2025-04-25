/**
* Author: Kevin Chen
* Assignment: Space Shooter
* Date due: 2025/04/25, 2:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f

#ifdef _WINDOWS
#   include <GL/glew.h>
#endif
#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "MenuScene.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"
#include "EndScene.h"
#include "winGame.h"

constexpr int WIN_W = 640, WIN_H = 480;

SDL_Window   *win  = nullptr;
ShaderProgram prog;
Scene        *scene = nullptr;

MenuScene *s_menu = nullptr;
Level1    *s_l1   = nullptr;
Level2    *s_l2   = nullptr;
Level3    *s_l3   = nullptr;
EndScene  *s_end  = nullptr;
winGame   *s_win  = nullptr;

glm::mat4 view(1.0f), proj;

void switch_scene(Scene *s) { scene = s; scene->initialise(); }

void init()
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    Mix_Music *bgm = Mix_LoadMUS("assets/sounds/bgm.mp3");
    Mix_PlayMusic(bgm, -1);

    win = SDL_CreateWindow("Space Shooter",
                           SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                           WIN_W, WIN_H, SDL_WINDOW_OPENGL);
    SDL_GLContext ctx = SDL_GL_CreateContext(win);
    SDL_GL_MakeCurrent(win, ctx);
#ifdef _WINDOWS
    glewInit();
#endif
    glViewport(0, 0, WIN_W, WIN_H);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    prog.load("shaders/vertex_textured.glsl",
              "shaders/fragment_textured.glsl");
    proj = glm::ortho(-5.f, 5.f, -3.75f, 3.75f, -1.f, 1.f);
    prog.set_projection_matrix(proj);

    s_l1   = new Level1;
    s_l2   = new Level2;
    s_l3   = new Level3;
    s_end  = new EndScene;
    s_win  = new winGame;
    s_menu = new MenuScene(s_l1);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    switch_scene(s_menu);
}

void loop()
{
    float prev = SDL_GetTicks() / 1000.0f;
    float acc  = 0.0f;
    bool running = true;

    while (running)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT) running = false;
            scene->process_input(e);
        }

        float now = SDL_GetTicks() / 1000.0f;
        float dt  = now - prev; prev = now; dt += acc;

        while (dt >= FIXED_TIMESTEP)
        {
            scene->update(FIXED_TIMESTEP);
            dt -= FIXED_TIMESTEP;
        }
        acc = dt;

        prog.set_view_matrix(view);
        glClear(GL_COLOR_BUFFER_BIT);
        scene->render(&prog);
        SDL_GL_SwapWindow(win);
    }
}

int main()
{
    init();
    loop();
    SDL_Quit();
    return 0;
}
