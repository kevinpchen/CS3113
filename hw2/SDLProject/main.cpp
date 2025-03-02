/**
* Author: Kevin Chen
* Assignment: Pong Clone
* Date due: 2025-3-01, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define LOG(argument) std::cout << argument << '\n'
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

enum AppStatus { RUNNING, TERMINATED };

constexpr int WINDOW_WIDTH  = 640 * 2,
              WINDOW_HEIGHT = 480 * 2;

constexpr float BG_RED     = 0.9765625f,
                BG_GREEN   = 0.97265625f,
                BG_BLUE    = 0.9609375f,
                BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X      = 0,
              VIEWPORT_Y      = 0,
              VIEWPORT_WIDTH  = WINDOW_WIDTH,
              VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
               F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;
constexpr GLint NUMBER_OF_TEXTURES = 1,
                LEVEL_OF_DETAIL    = 0,
                TEXTURE_BORDER     = 0;

constexpr char LEFT_PADDLE_FILEPATH[]   = "assets/fire.png";
constexpr char RIGHT_PADDLE_FILEPATH[]  = "assets/electro.png";
constexpr char BALL_FILEPATH[]          = "assets/primogem.png";
constexpr char FIRE_WINS_FILEPATH[]     = "assets/fire_wins.png";
constexpr char ELECTRO_WINS_FILEPATH[]  = "assets/electro_wins.png";

constexpr float LEFT_BOUND    = -5.0f;
constexpr float RIGHT_BOUND   =  5.0f;
constexpr float TOP_BOUND     =  3.75f;
constexpr float BOTTOM_BOUND  = -3.75f;

constexpr float PADDLE_SPEED  = 4.0f;
constexpr float PADDLE_WIDTH  = 0.2f;
constexpr float PADDLE_HEIGHT = 1.0f;

constexpr float BALL_SPEED    = 1.0f;
constexpr float BALL_SIZE     = 1.0f;

constexpr int MAX_BALLS = 3;

struct Ball {
    glm::vec3 position;
    glm::vec3 velocity;
    bool active;
};

Ball g_balls[MAX_BALLS];
SDL_Window* g_display_window;
AppStatus g_app_status = RUNNING;
ShaderProgram g_shader_program;
glm::mat4 g_view_matrix(1.0f);
glm::mat4 g_projection_matrix(1.0f);
GLuint g_left_paddle_texture;
GLuint g_right_paddle_texture;
GLuint g_ball_texture;
GLuint g_fire_wins_texture;
GLuint g_electro_wins_texture;
glm::vec3 g_left_paddle_pos   = glm::vec3(-4.5f, 0.0f, 0.0f);
glm::vec3 g_right_paddle_pos  = glm::vec3( 4.5f, 0.0f, 0.0f);
glm::vec3 g_left_paddle_vel   = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_right_paddle_vel  = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_ball_pos          = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_ball_vel          = glm::vec3(BALL_SPEED, BALL_SPEED, 0.0f);
bool g_single_player_mode = false;
float g_previous_ticks = 0.0f;
bool g_game_over = false;
std::string g_winner_text = "hi";

GLuint load_texture(const char* filepath) {
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);
    if (image == NULL) {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }
    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    stbi_image_free(image);
    return textureID;
}

bool check_collision(const glm::vec3& posA, float halfWidthA, float halfHeightA, const glm::vec3& posB, float halfWidthB, float halfHeightB) {
    bool collisionX = fabs(posA.x - posB.x) <= (halfWidthA + halfWidthB);
    bool collisionY = fabs(posA.y - posB.y) <= (halfHeightA + halfHeightB);
    return (collisionX && collisionY);
}

void initialise() {
    SDL_Init(SDL_INIT_VIDEO);
    g_display_window = SDL_CreateWindow("genshinpong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);
    if (g_display_window == nullptr) {
        std::cerr << "Error: SDL window could not be created.\n";
        SDL_Quit();
        exit(1);
    }
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
    g_left_paddle_texture  = load_texture(LEFT_PADDLE_FILEPATH);
    g_right_paddle_texture = load_texture(RIGHT_PADDLE_FILEPATH);
    g_ball_texture         = load_texture(BALL_FILEPATH);
    g_fire_wins_texture    = load_texture(FIRE_WINS_FILEPATH);
    g_electro_wins_texture = load_texture(ELECTRO_WINS_FILEPATH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for (int i = 0; i < MAX_BALLS; i++) {
        g_balls[i].position = glm::vec3(0.0f, 0.0f, 0.0f);
        g_balls[i].velocity = glm::vec3(BALL_SPEED, BALL_SPEED, 0.0f);
        g_balls[i].active   = false;
    }
    g_balls[0].active = true;
}

void process_input() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            g_app_status = TERMINATED;
        } else if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_1) {
                for (int i = 0; i < MAX_BALLS; i++) g_balls[i].active = (i == 0);
            } else if (event.key.keysym.sym == SDLK_2) {
                for (int i = 0; i < MAX_BALLS; i++) g_balls[i].active = (i < 2);
            } else if (event.key.keysym.sym == SDLK_3) {
                for (int i = 0; i < MAX_BALLS; i++) g_balls[i].active = true;
            } else if (event.key.keysym.sym == SDLK_t) {
                g_single_player_mode = !g_single_player_mode;
            }
        }
    }
    g_left_paddle_vel.y  = 0.0f;
    g_right_paddle_vel.y = 0.0f;
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    if (keys[SDL_SCANCODE_W]) {
        g_left_paddle_vel.y = PADDLE_SPEED;
    } else if (keys[SDL_SCANCODE_S]) {
        g_left_paddle_vel.y = -PADDLE_SPEED;
    }
    if (!g_single_player_mode) {
        if (keys[SDL_SCANCODE_UP]) {
            g_right_paddle_vel.y = PADDLE_SPEED;
        } else if (keys[SDL_SCANCODE_DOWN]) {
            g_right_paddle_vel.y = -PADDLE_SPEED;
        }
    } else {
        float auto_move_speed = 2.0f;
        if (g_ball_pos.y > g_right_paddle_pos.y) {
            g_right_paddle_vel.y = auto_move_speed;
        } else if (g_ball_pos.y < g_right_paddle_pos.y) {
            g_right_paddle_vel.y = -auto_move_speed;
        }
    }
}

void update() {
    if (g_game_over) return;
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;
    g_left_paddle_pos += g_left_paddle_vel * delta_time;
    g_right_paddle_pos += g_right_paddle_vel * delta_time;
    float max_paddle_y = TOP_BOUND - PADDLE_HEIGHT;
    float min_paddle_y = BOTTOM_BOUND + PADDLE_HEIGHT;
    if (g_left_paddle_pos.y > max_paddle_y) {
        g_left_paddle_pos.y = max_paddle_y;
    } else if (g_left_paddle_pos.y < min_paddle_y) {
        g_left_paddle_pos.y = min_paddle_y;
    }
    if (g_right_paddle_pos.y > max_paddle_y) {
        g_right_paddle_pos.y = max_paddle_y;
    } else if (g_right_paddle_pos.y < min_paddle_y) {
        g_right_paddle_pos.y = min_paddle_y;
    }
    g_ball_pos += g_ball_vel * delta_time;
    if (g_ball_pos.y + BALL_SIZE >= TOP_BOUND) {
        g_ball_pos.y = TOP_BOUND - BALL_SIZE;
        g_ball_vel.y = -g_ball_vel.y;
    } else if (g_ball_pos.y - BALL_SIZE <= BOTTOM_BOUND) {
        g_ball_pos.y = BOTTOM_BOUND + BALL_SIZE;
        g_ball_vel.y = -g_ball_vel.y;
    }
    if (check_collision(g_ball_pos, BALL_SIZE, BALL_SIZE, g_left_paddle_pos, PADDLE_WIDTH, PADDLE_HEIGHT)) {
        g_ball_pos.x = g_left_paddle_pos.x + PADDLE_WIDTH + BALL_SIZE;
        g_ball_vel.x = fabs(g_ball_vel.x);
    }
    if (check_collision(g_ball_pos, BALL_SIZE, BALL_SIZE, g_right_paddle_pos, PADDLE_WIDTH, PADDLE_HEIGHT)) {
        g_ball_pos.x = g_right_paddle_pos.x - (PADDLE_WIDTH + BALL_SIZE);
        g_ball_vel.x = -fabs(g_ball_vel.x);
    }
    if (g_ball_pos.x < LEFT_BOUND - BALL_SIZE) {
        g_game_over = true;
        g_winner_text = "electro";
    } else if (g_ball_pos.x > RIGHT_BOUND + BALL_SIZE) {
        g_game_over = true;
        g_winner_text = "fire";
    }
    for (int i = 0; i < MAX_BALLS; i++) {
        if (!g_balls[i].active) continue;
        g_balls[i].position += g_balls[i].velocity * delta_time;
        if (g_balls[i].position.y + BALL_SIZE >= TOP_BOUND) {
            g_balls[i].position.y = TOP_BOUND - BALL_SIZE;
            g_balls[i].velocity.y = -g_balls[i].velocity.y;
        } else if (g_balls[i].position.y - BALL_SIZE <= BOTTOM_BOUND) {
            g_balls[i].position.y = BOTTOM_BOUND + BALL_SIZE;
            g_balls[i].velocity.y = -g_balls[i].velocity.y;
        }
        if (check_collision(g_balls[i].position, BALL_SIZE, BALL_SIZE, g_left_paddle_pos, PADDLE_WIDTH, PADDLE_HEIGHT)) {
            g_balls[i].position.x = g_left_paddle_pos.x + PADDLE_WIDTH + BALL_SIZE;
            g_balls[i].velocity.x = fabs(g_balls[i].velocity.x);
        }
        if (check_collision(g_balls[i].position, BALL_SIZE, BALL_SIZE, g_right_paddle_pos, PADDLE_WIDTH, PADDLE_HEIGHT)) {
            g_balls[i].position.x = g_right_paddle_pos.x - (PADDLE_WIDTH + BALL_SIZE);
            g_balls[i].velocity.x = -fabs(g_balls[i].velocity.x);
        }
        if (g_balls[i].position.x < LEFT_BOUND - BALL_SIZE) {
            g_game_over = true;
            g_winner_text = "electro";
        } else if (g_balls[i].position.x > RIGHT_BOUND + BALL_SIZE) {
            g_game_over = true;
            g_winner_text = "fire";
        }
        if (g_game_over) break;
    }
}

void draw_object(glm::mat4 &object_g_model_matrix, GLuint &object_texture_id) {
    g_shader_program.set_model_matrix(object_g_model_matrix);
    glBindTexture(GL_TEXTURE_2D, object_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void render_rectangle(GLuint texture_id, glm::vec3 position, float halfWidth, float halfHeight) {
    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix = glm::translate(model_matrix, position);
    model_matrix = glm::scale(model_matrix, glm::vec3(2.0f * halfWidth, 2.0f * halfHeight, 1.0f));
    draw_object(model_matrix, texture_id);
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
    render_rectangle(g_left_paddle_texture, g_left_paddle_pos, 0.5f, 1.0f);
    render_rectangle(g_right_paddle_texture, g_right_paddle_pos, 0.5f, 1.0f);
    for (int i = 0; i < MAX_BALLS; i++) {
        if (!g_balls[i].active) continue;
        render_rectangle(g_ball_texture, g_balls[i].position, BALL_SIZE, BALL_SIZE);
    }
    if (g_game_over) {
        GLuint winner_texture = 0;
        if (g_winner_text == "fire") {
            winner_texture = g_fire_wins_texture;
        } else if (g_winner_text == "electro") {
            winner_texture = g_electro_wins_texture;
        }
        if (winner_texture != 0) {
            glm::mat4 model_matrix = glm::mat4(1.0f);
            model_matrix = glm::translate(model_matrix, glm::vec3(0.0f, 0.0f, 0.0f));
            model_matrix = glm::scale(model_matrix, glm::vec3(2.0f, 1.0f, 1.0f));
            g_shader_program.set_model_matrix(model_matrix);
            glBindTexture(GL_TEXTURE_2D, winner_texture);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
    }
    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
    SDL_GL_SwapWindow(g_display_window);
}

void shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    initialise();
    while (g_app_status == RUNNING) {
        process_input();
        update();
        render();
    }
    shutdown();
    return 0;
}
