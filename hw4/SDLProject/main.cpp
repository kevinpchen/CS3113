/**
* Author: Kevin Chen
* Assignment: Rise of the AI
* Date due: 2025-04-05, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 8
#define LEVEL1_LEFT_EDGE 5.0f

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Entity.h"
#include "Map.h"
#include "Utility.h"
#include "Scene.h"
#include "menuScene.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"
#include "endScene.h"
#include "winGame.h"

constexpr int WINDOW_WIDTH  = 640;
constexpr int WINDOW_HEIGHT = 480;

constexpr float BG_RED     = 0.1922f;
constexpr float BG_BLUE    = 0.549f;
constexpr float BG_GREEN   = 0.9059f;
constexpr float BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0;
constexpr int VIEWPORT_Y = 0;
constexpr int VIEWPORT_WIDTH  = WINDOW_WIDTH;
constexpr int VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl";
constexpr char F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;

enum AppStatus { RUNNING, TERMINATED };

Scene *g_current_scene;
MenuScene *g_menuScene;
Level1    *g_level1;
Level2    *g_level2;
Level3    *g_level3;
EndScene  *g_endScene;
winGame   *g_winGame;

SDL_Window* g_display_window;
AppStatus g_app_status = RUNNING;
ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;

void switch_to_scene(Scene *scene)
{
    g_current_scene = scene;
    g_current_scene->initialise();
}

void initialise();
void process_input();
void update();
void render();
void shutdown();

void initialise()
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow("Hello, Scenes!",
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      WINDOW_WIDTH, WINDOW_HEIGHT,
                                      SDL_WINDOW_OPENGL);
    
    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);
    if (context == nullptr)
    {
        shutdown();
    }
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    
    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);
    
    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());
    
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    
    g_menuScene = new MenuScene();
    g_level1    = new Level1();
    g_level2    = new Level2();
    g_level3    = new Level3();
    g_endScene  = new EndScene();
    g_winGame   = new winGame();
    
    switch_to_scene(g_menuScene);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    g_current_scene->get_state().player->set_movement(glm::vec3(0.0f));
    
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                g_app_status = TERMINATED;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                    case SDLK_q:
                        g_app_status = TERMINATED;
                        break;
                        
                    case SDLK_SPACE:
                        if (g_current_scene->get_state().player->get_collided_bottom())
                        {
                            g_current_scene->get_state().player->jump();
                            Mix_PlayChannel(-1, g_current_scene->get_state().jump_sfx, 0);
                        }
                        break;

                    case SDLK_3:
                        switch_to_scene(g_level3);
                        g_level3->get_state().player->set_position(glm::vec3(1.0f, -6.0f, 0.0f));
                        break;

                    case SDLK_RETURN:
                        switch_to_scene(g_level1);
                        break;
                        
                    default:
                        break;
                }
                break;
        }
    }
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    if (keys[SDL_SCANCODE_LEFT])
    {
        g_current_scene->get_state().player->move_left();
    }
    else if (keys[SDL_SCANCODE_RIGHT])
    {
        g_current_scene->get_state().player->move_right();
    }

    if (glm::length(g_current_scene->get_state().player->get_movement()) > 1.0f)
    {
        g_current_scene->get_state().player->normalise_movement();
    }
}

void update()
{
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;
    
    delta_time += g_accumulator;
    if (delta_time < FIXED_TIMESTEP)
    {
        g_accumulator = delta_time;
        return;
    }
    
    while (delta_time >= FIXED_TIMESTEP)
    {
        g_current_scene->update(FIXED_TIMESTEP);
        delta_time -= FIXED_TIMESTEP;
    }
    
    g_accumulator = delta_time;
    
    if (g_current_scene->get_state().player->get_lives() <= 0)
    {
        switch_to_scene(g_endScene);
    }

    if (g_current_scene->get_state().player->get_position().y < -10.0f)
    {
        int curr_lives = g_current_scene->get_state().player->get_lives();
        
        if (g_current_scene == g_level1)
        {
            switch_to_scene(g_level2);
            g_level2->get_state().player->set_lives(curr_lives);
            g_level2->get_state().player->set_position(glm::vec3(1.0f, -6.0f, 0.0f));
        }
        else if (g_current_scene == g_level2)
        {
            switch_to_scene(g_level3);
            g_level3->get_state().player->set_lives(curr_lives);
            g_level3->get_state().player->set_position(glm::vec3(1.0f, -6.0f, 0.0f));
        }
        else if ((g_current_scene == g_level3) &&
                 (g_current_scene->get_state().player->get_position().x > 10.0f))
        {
            switch_to_scene(g_winGame);
        }
    }
    
    g_view_matrix = glm::mat4(1.0f);
    float player_x = g_current_scene->get_state().player->get_position().x;
    if (player_x > LEVEL1_LEFT_EDGE)
    {
        g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-player_x, 3.75f, 0.0f));
    }
    else
    {
        g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-5.0f, 3.75f, 0.0f));
    }
}

void render()
{
    g_shader_program.set_view_matrix(g_view_matrix);
    
    glClear(GL_COLOR_BUFFER_BIT);
    g_current_scene->render(&g_shader_program);
    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{
    SDL_Quit();
    delete g_level1;
    delete g_level2;
    delete g_level3;
    delete g_menuScene;
    delete g_endScene;
    delete g_winGame;
}

int main(int argc, char* argv[])
{
    initialise();
    while (g_app_status == RUNNING)
    {
        process_input();
        update();
        render();
    }
    shutdown();
    return 0;
}
