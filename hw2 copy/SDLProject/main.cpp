/**
* Author: Kevin Chen
* Assignment: Lunar Lander
* Date due: 2025-03-18, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define GL_GLEXT_PROTOTYPES 1

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"

#include <iostream>
#include <cassert>
#include <cmath>

constexpr int WINDOW_WIDTH  = 640 * 2,
              WINDOW_HEIGHT = 480 * 2;

constexpr int VIEWPORT_X      = 0,
              VIEWPORT_Y      = 0,
              VIEWPORT_WIDTH  = WINDOW_WIDTH,
              VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr float BG_RED     = 0.9765625f,
                BG_GREEN   = 0.97265625f,
                BG_BLUE    = 0.9609375f,
                BG_OPACITY = 1.0f;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
               F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr float GRAVITY = -0.1f;
constexpr float THRUST_ACCEL = 1.0f;
constexpr float FUEL_CONSUMPTION_RATE = 20.0f;

constexpr float SHIP_WIDTH  = 0.5f;
constexpr float SHIP_HEIGHT = 0.5f;
constexpr float PLATFORM_WIDTH  = 1.0f;
constexpr float PLATFORM_HEIGHT = 0.25f;

constexpr float LEFT_BOUND   = -5.0f;
constexpr float RIGHT_BOUND  =  5.0f;
constexpr float TOP_BOUND    =  3.75f;
constexpr float BOTTOM_BOUND = -3.75f;

constexpr char SHIP_TEXTURE_FILE[]      = "assets/firefly.png";
constexpr char FAIL_PLATFORM_FILE[]     = "assets/fail.png";
constexpr char WIN_PLATFORM_FILE[]      = "assets/win.png";
constexpr char FAIL_TEXT_TEXTURE_FILE[] = "assets/fail_txt.png";
constexpr char WIN_TEXT_TEXTURE_FILE[]  = "assets/win_txt.png";

struct Entity {
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    float width;
    float height;
    GLuint texture;
    bool active;
};

SDL_Window* g_window;
ShaderProgram g_shader_program;
glm::mat4 g_view_matrix(1.0f);
glm::mat4 g_projection_matrix(1.0f);

Entity g_ship;
Entity g_failPlatform;
Entity g_winPlatform;
float g_winPlatformInitialX = 3.0f;

GLuint g_failTextTexture;
GLuint g_winTextTexture;

GLuint g_white_texture;
GLuint g_black_texture;
GLuint g_green_texture;

bool g_game_over = false;
bool g_mission_accomplished = false;
float g_fuel = 100.0f;

float g_previous_ticks = 0.0f;

GLuint load_texture(const char* filepath) {
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);
    if (image == nullptr) {
        std::cout << "Unable to load image: " << filepath << std::endl;
        assert(false);
    }
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    stbi_image_free(image);
    return textureID;
}

GLuint create_colored_texture(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    unsigned char pixel[4] = { r, g, b, a };
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    return textureID;
}

bool check_collision(const Entity& a, const Entity& b) {
    bool collisionX = fabs(a.position.x - b.position.x) <= (a.width + b.width);
    bool collisionY = fabs(a.position.y - b.position.y) <= (a.height + b.height);
    return collisionX && collisionY;
}

void render_entity(const Entity& entity) {
    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix = glm::translate(model_matrix, entity.position);
    model_matrix = glm::scale(model_matrix, glm::vec3(2.0f * entity.width, 2.0f * entity.height, 1.0f));
    g_shader_program.set_model_matrix(model_matrix);
    glBindTexture(GL_TEXTURE_2D, entity.texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void render_rectangle(GLuint texture, glm::vec3 position, float halfWidth, float halfHeight) {
    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix = glm::translate(model_matrix, position);
    model_matrix = glm::scale(model_matrix, glm::vec3(2.0f * halfWidth, 2.0f * halfHeight, 1.0f));
    g_shader_program.set_model_matrix(model_matrix);
    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void render_fuel_bar() {
    glm::vec3 bar_center = glm::vec3(LEFT_BOUND + 1.0f, TOP_BOUND - 0.5f, 0.0f);
    float full_half_width = 1.0f;
    float half_height = 0.15f;
    render_rectangle(g_black_texture, bar_center, full_half_width, half_height);
    float fuel_ratio = g_fuel / 100.0f;
    float fuel_half_width = full_half_width * fuel_ratio;
    glm::vec3 fuel_center = glm::vec3(LEFT_BOUND + 1.0f - (full_half_width - fuel_half_width), TOP_BOUND - 0.5f, 0.1f);
    render_rectangle(g_green_texture, fuel_center, fuel_half_width, half_height);
}

void initialise() {
    SDL_Init(SDL_INIT_VIDEO);
    g_window = SDL_CreateWindow("Lunar Lander", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(g_window);
    SDL_GL_MakeCurrent(g_window, context);
#ifdef _WINDOWS
    glewInit();
#endif
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);
    g_projection_matrix = glm::ortho(LEFT_BOUND, RIGHT_BOUND, BOTTOM_BOUND, TOP_BOUND, -1.0f, 1.0f);
    g_view_matrix = glm::mat4(1.0f);
    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);
    glClearColor(BG_RED, BG_GREEN, BG_BLUE, BG_OPACITY);
    GLuint shipTexture = load_texture(SHIP_TEXTURE_FILE);
    GLuint failPlatformTexture = load_texture(FAIL_PLATFORM_FILE);
    GLuint winPlatformTexture  = load_texture(WIN_PLATFORM_FILE);
    g_failTextTexture = load_texture(FAIL_TEXT_TEXTURE_FILE);
    g_winTextTexture  = load_texture(WIN_TEXT_TEXTURE_FILE);
    g_white_texture = create_colored_texture(255,255,255,255);
    g_black_texture = create_colored_texture(0,0,0,255);
    g_green_texture = create_colored_texture(0,255,0,255);
    g_ship.position = glm::vec3(0.0f, TOP_BOUND - 0.5f, 0.0f);
    g_ship.velocity = glm::vec3(0.0f);
    g_ship.acceleration = glm::vec3(0.0f);
    g_ship.width = SHIP_WIDTH;
    g_ship.height = SHIP_HEIGHT;
    g_ship.texture = shipTexture;
    g_ship.active = true;
    g_failPlatform.position = glm::vec3(-3.0f, BOTTOM_BOUND + PLATFORM_HEIGHT + 0.5f, 0.0f);
    g_failPlatform.velocity = glm::vec3(0.0f);
    g_failPlatform.acceleration = glm::vec3(0.0f);
    g_failPlatform.width = PLATFORM_WIDTH;
    g_failPlatform.height = PLATFORM_HEIGHT;
    g_failPlatform.texture = failPlatformTexture;
    g_failPlatform.active = true;
    g_winPlatform.position = glm::vec3(g_winPlatformInitialX, BOTTOM_BOUND + PLATFORM_HEIGHT + 0.5f, 0.0f);
    g_winPlatform.velocity = glm::vec3(0.0f);
    g_winPlatform.acceleration = glm::vec3(0.0f);
    g_winPlatform.width = PLATFORM_WIDTH;
    g_winPlatform.height = PLATFORM_HEIGHT;
    g_winPlatform.texture = winPlatformTexture;
    g_winPlatform.active = true;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    g_previous_ticks = (float)SDL_GetTicks() / 1000.0f;
}

void process_input() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            g_game_over = true;
        }
    }
    g_ship.acceleration.x = 0.0f;
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    if (g_fuel > 0.0f) {
        if (keys[SDL_SCANCODE_LEFT]) {
            g_ship.acceleration.x = -THRUST_ACCEL;
        } else if (keys[SDL_SCANCODE_RIGHT]) {
            g_ship.acceleration.x = THRUST_ACCEL;
        }
    }
}

void update() {
    if (g_game_over) return;
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;
    g_ship.acceleration.y = GRAVITY;
    if (fabs(g_ship.acceleration.x) > 0.0f && g_fuel > 0.0f) {
        g_fuel -= FUEL_CONSUMPTION_RATE * delta_time;
        if (g_fuel < 0.0f) {
            g_fuel = 0.0f;
            g_ship.acceleration.x = 0.0f;
        }
    }
    g_ship.velocity += g_ship.acceleration * delta_time;
    g_ship.position += g_ship.velocity * delta_time;
    if (g_ship.position.x + g_ship.width >= RIGHT_BOUND) {
        g_game_over = true;
        g_mission_accomplished = false;
    }
    if (g_ship.position.x - g_ship.width <= LEFT_BOUND) {
        g_game_over = true;
        g_mission_accomplished = false;
    }
    if (g_ship.position.y - g_ship.height <= BOTTOM_BOUND) {
        g_game_over = true;
        g_mission_accomplished = false;
    }
    if (g_ship.position.y + g_ship.height > TOP_BOUND) {
        g_ship.position.y = TOP_BOUND - g_ship.height;
        g_ship.velocity.y = 0.0f;
    }
    constexpr float amplitude = 1.0f;
    g_winPlatform.position.x = g_winPlatformInitialX + sin(ticks) * amplitude;
    if (check_collision(g_ship, g_winPlatform)) {
        g_game_over = true;
        g_mission_accomplished = true;
    }
    if (check_collision(g_ship, g_failPlatform)) {
        g_game_over = true;
        g_mission_accomplished = false;
    }
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT);
    float vertices[] = {
        -0.5f, -0.5f,
         0.5f, -0.5f,
         0.5f,  0.5f,
        -0.5f, -0.5f,
         0.5f,  0.5f,
        -0.5f,  0.5f
    };
    float texture_coordinates[] = {
         0.0f, 1.0f,
         1.0f, 1.0f,
         1.0f, 0.0f,
         0.0f, 1.0f,
         1.0f, 0.0f,
         0.0f, 0.0f
    };
    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());
    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
    render_entity(g_ship);
    render_entity(g_failPlatform);
    render_entity(g_winPlatform);
    render_fuel_bar();
    if (g_game_over) {
        GLuint overlayTexture = g_mission_accomplished ? g_winTextTexture : g_failTextTexture;
        glm::mat4 model_matrix = glm::mat4(1.0f);
        model_matrix = glm::translate(model_matrix, glm::vec3(0.0f, 0.0f, 0.0f));
        model_matrix = glm::scale(model_matrix, glm::vec3(3.0f, 1.5f, 1.0f));
        g_shader_program.set_model_matrix(model_matrix);
        glBindTexture(GL_TEXTURE_2D, overlayTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
    SDL_GL_SwapWindow(g_window);
}

void shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    initialise();
    while (!g_game_over) {
        process_input();
        update();
        render();
    }
    SDL_Delay(2000);
    shutdown();
    return 0;
}
