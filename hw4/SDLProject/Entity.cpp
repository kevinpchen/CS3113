/**
* Author: Kevin Chen
* Assignment: Rise of the AI
* Date due: 2025-04-05, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on Academic Misconduct.
**/

#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include <cmath>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Entity.h"
#include "ShaderProgram.h"

Entity::Entity() {}

Entity::Entity(GLuint texture_id, float speed, float width, float height, EntityType type)
    : m_texture_id(texture_id), m_speed(speed),
      m_width(width), m_height(height), m_entity_type(type)
{
}

Entity::Entity(GLuint texture_id, float speed, float width, float height,
               EntityType type, AIType ai_type, AIState ai_state)
    : m_texture_id(texture_id), m_speed(speed),
      m_width(width), m_height(height),
      m_entity_type(type), m_ai_type(ai_type), m_ai_state(ai_state)
{
}

Entity::~Entity() {}

void Entity::ai_walk()
{
    m_movement.x = -1.0f;
}

void Entity::ai_guard(Entity* player)
{
    switch (m_ai_state)
    {
        case IDLE:
            if (glm::distance(m_position, player->get_position()) < 3.0f)
                m_ai_state = WALKING;
            break;
        case WALKING:
            if (m_position.x > player->get_position().x)
                m_movement.x = -1.0f;
            else
                m_movement.x = 1.0f;
            break;
        default:
            break;
    }
}

void Entity::ai_flyer()
{
    float fly_speed = 1.0f;
    float amplitudeY = 2.0f;
    m_ai_state = FLYING;
    float t = SDL_GetTicks() * 0.001f * fly_speed;
    m_movement.y = sinf(t) * amplitudeY;
}

void Entity::ai_activate(Entity* player)
{
    switch (m_ai_type)
    {
        case WALKER: ai_walk(); break;
        case GUARD:  ai_guard(player); break;
        case FLYER:  ai_flyer(); break;
        default: break;
    }
}

bool const Entity::check_collision(Entity* other) const
{
    float xdist = std::fabs(m_position.x - other->m_position.x) - ((m_width + other->m_width) / 2.0f);
    float ydist = std::fabs(m_position.y - other->m_position.y) - ((m_height + other->m_height) / 2.0f);
    return (xdist <= 0 && ydist <= 0);
}

void const Entity::check_collision_y(Entity* collidables, int collidable_count)
{
    for (int i = 0; i < collidable_count; i++)
    {
        Entity* obj = &collidables[i];
        if (check_collision(obj))
        {
            float ydist = std::fabs(m_position.y - obj->m_position.y);
            float y_overlap = std::fabs(ydist - (m_height / 2.0f) - (obj->m_height / 2.0f));
            if (m_velocity.y > 0)
            {
                m_position.y -= y_overlap;
                m_velocity.y = 0;
                m_collided_top = true;
            }
            else if (m_velocity.y < 0)
            {
                m_position.y += y_overlap;
                m_velocity.y = 0;
                m_collided_bottom = true;
            }
        }
    }
}

void const Entity::check_collision_x(Entity* collidables, int collidable_count)
{
    for (int i = 0; i < collidable_count; i++)
    {
        Entity* obj = &collidables[i];
        if (check_collision(obj))
        {
            float xdist = std::fabs(m_position.x - obj->m_position.x);
            float x_overlap = std::fabs(xdist - (m_width / 2.0f) - (obj->m_width / 2.0f));
            if (m_velocity.x > 0)
            {
                m_position.x -= x_overlap;
                m_velocity.x = 0;
                m_collided_right = true;
            }
            else if (m_velocity.x < 0)
            {
                m_position.x += x_overlap;
                m_velocity.x = 0;
                m_collided_left = true;
            }
        }
    }
}

void const Entity::check_collision_y(Map* map)
{
    glm::vec3 top = glm::vec3(m_position.x, m_position.y + (m_height / 2), m_position.z);
    glm::vec3 bottom = glm::vec3(m_position.x, m_position.y - (m_height / 2), m_position.z);
    glm::vec3 top_left = glm::vec3(m_position.x - (m_width / 2), m_position.y + (m_height / 2), m_position.z);
    glm::vec3 top_right = glm::vec3(m_position.x + (m_width / 2), m_position.y + (m_height / 2), m_position.z);
    glm::vec3 bottom_left = glm::vec3(m_position.x - (m_width / 2), m_position.y - (m_height / 2), m_position.z);
    glm::vec3 bottom_right = glm::vec3(m_position.x + (m_width / 2), m_position.y - (m_height / 2), m_position.z);
    float pen_x = 0.0f;
    float pen_y = 0.0f;
    if (m_velocity.y > 0)
    {
        if (map->is_solid(top, &pen_x, &pen_y) || map->is_solid(top_left, &pen_x, &pen_y) || map->is_solid(top_right, &pen_x, &pen_y))
        {
            m_position.y -= pen_y;
            m_velocity.y = 0;
            m_collided_top = true;
        }
    }
    if (m_velocity.y < 0)
    {
        if (map->is_solid(bottom, &pen_x, &pen_y) || map->is_solid(bottom_left, &pen_x, &pen_y) || map->is_solid(bottom_right, &pen_x, &pen_y))
        {
            m_position.y += pen_y;
            m_velocity.y = 0;
            m_collided_bottom = true;
        }
    }
}

void const Entity::check_collision_x(Map* map)
{
    glm::vec3 left = glm::vec3(m_position.x - (m_width / 2), m_position.y, m_position.z);
    glm::vec3 right = glm::vec3(m_position.x + (m_width / 2), m_position.y, m_position.z);
    float pen_x = 0.0f;
    float pen_y = 0.0f;
    if (m_velocity.x < 0)
    {
        if (map->is_solid(left, &pen_x, &pen_y))
        {
            m_position.x += pen_x;
            m_velocity.x = 0;
            m_collided_left = true;
        }
    }
    else if (m_velocity.x > 0)
    {
        if (map->is_solid(right, &pen_x, &pen_y))
        {
            m_position.x -= pen_x;
            m_velocity.x = 0;
            m_collided_right = true;
        }
    }
}

bool Entity::check_collision_with_enemy(Entity* enemies, int enemy_count)
{
    for (int i = 0; i < enemy_count; i++)
    {
        if (check_collision(&enemies[i]))
        {
            m_position = glm::vec3(0.0f, 0.0f, 0.0f);
            return true;
        }
    }
    return false;
}

void Entity::update(float delta_time, Entity* player, Entity* collidables, int collidable_count, Map* map)
{
    if (!m_is_active) return;
    m_collided_top = false;
    m_collided_bottom = false;
    m_collided_left = false;
    m_collided_right = false;
    if (m_entity_type == ENEMY)
        ai_activate(player);
    m_velocity.x = m_movement.x * m_speed;
    m_velocity += (m_acceleration * delta_time);
    if (m_entity_type == ENEMY && m_ai_type == FLYER) {
        m_velocity.y = m_movement.y * m_speed;
    } else {
        m_velocity.y += m_acceleration.y * delta_time;
    }
    if (m_is_jumping)
    {
        m_is_jumping = false;
        m_velocity.y += m_jumping_power;
    }
    m_position.y += (m_velocity.y * delta_time);
    check_collision_y(collidables, collidable_count);
    check_collision_y(map);
    m_position.x += (m_velocity.x * delta_time);
    check_collision_x(collidables, collidable_count);
    check_collision_x(map);
    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);
}

void Entity::render(ShaderProgram* program)
{
    program->set_model_matrix(m_model_matrix);
    float vertices[] = {
        -0.5f, -0.5f,
         0.5f, -0.5f,
         0.5f,  0.5f,
        -0.5f, -0.5f,
         0.5f,  0.5f,
        -0.5f,  0.5f
    };
    float tex_coords[] = {
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f
    };
    glBindTexture(GL_TEXTURE_2D, m_texture_id);
    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}
