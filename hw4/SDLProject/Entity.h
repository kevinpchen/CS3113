/**
* Author: Kevin Chen
* Assignment: Rise of the AI
* Date due: 2025-04-05, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#ifndef ENTITY_H
#define ENTITY_H

#include "Map.h"
#include "glm/glm.hpp"
#include "ShaderProgram.h"

enum EntityType { PLATFORM, PLAYER, ENEMY, MENU };
enum AIType     { WALKER, GUARD, FLYER };
enum AIState    { WALKING, IDLE, ATTACKING, FLYING };

class Entity
{
private:
    bool   m_is_active  = true;
    EntityType m_entity_type;
    AIType     m_ai_type    = WALKER;
    AIState    m_ai_state   = IDLE;

    glm::vec3 m_position    = glm::vec3(0.0f);
    glm::vec3 m_movement    = glm::vec3(0.0f);
    glm::vec3 m_scale       = glm::vec3(1.0f);
    glm::vec3 m_velocity    = glm::vec3(0.0f);
    glm::vec3 m_acceleration= glm::vec3(0.0f);
    
    glm::mat4 m_model_matrix= glm::mat4(1.0f);
    
    float m_speed          = 0.0f;
    float m_jumping_power  = 0.0f;
    bool  m_is_jumping     = false;

    GLuint m_texture_id;

    float m_width  = 1.0f;
    float m_height = 1.0f;
    
    bool m_collided_top    = false;
    bool m_collided_bottom = false;
    bool m_collided_left   = false;
    bool m_collided_right  = false;
    
    unsigned int m_lives = 3;
    
    float m_damageCooldown = 0.0f;

    void ai_walk();
    void ai_guard(Entity* player);
    void ai_flyer();

public:
    Entity(GLuint texture_id, float speed, float width, float height, EntityType type);

    Entity(GLuint texture_id, float speed, float width, float height,
           EntityType type, AIType ai_type, AIState ai_state);

    Entity();
    ~Entity();

    void update(float delta_time, Entity* player, Entity* collidables,
                int collidable_count, Map* map);
    void render(ShaderProgram* program);
    
    void set_movement(glm::vec3 new_movement)
    {
        m_movement = new_movement;
    }
    
    void set_acceleration(glm::vec3 new_acceleration) {
        m_acceleration = new_acceleration;
    }

    bool const check_collision(Entity* other) const;
    void const check_collision_y(Entity* collidables, int collidable_count);
    void const check_collision_x(Entity* collidables, int collidable_count);
    void const check_collision_y(Map* map);
    void const check_collision_x(Map* map);

    bool check_collision_with_enemy(Entity* enemies, int enemy_count);

    void ai_activate(Entity* player);
    
    void update_cooldown(float delta_time) {
        if (m_damageCooldown > 0.0f) {
            m_damageCooldown -= delta_time;
            if (m_damageCooldown < 0.0f) m_damageCooldown = 0.0f;
        }
    }

    void start_damage_cooldown(float cooldown) {
        m_damageCooldown = cooldown;
    }

    float get_damage_cooldown() const { return m_damageCooldown; }

    void normalise_movement() { m_movement = glm::normalize(m_movement); }
    void move_left()  { m_movement.x = -1.0f; }
    void move_right() { m_movement.x =  1.0f; }
    void move_up()    { m_movement.y =  1.0f; }
    void move_down()  { m_movement.y = -1.0f; }
    void jump()       { m_is_jumping = true; }

    bool const is_active()          const { return m_is_active; }
    void set_active(bool active)          { m_is_active = active; }

    EntityType const get_entity_type() const { return m_entity_type; }
    AIType     const get_ai_type()     const { return m_ai_type; }
    AIState    const get_ai_state()    const { return m_ai_state; }

    glm::vec3 const get_position()     const { return m_position; }
    glm::vec3 const get_velocity()     const { return m_velocity; }
    glm::vec3 const get_acceleration() const { return m_acceleration; }
    glm::vec3 const get_movement()     const { return m_movement; }
    float     const get_speed()        const { return m_speed; }

    bool const get_collided_top()      const { return m_collided_top; }
    bool const get_collided_bottom()   const { return m_collided_bottom; }
    bool const get_collided_left()     const { return m_collided_left; }
    bool const get_collided_right()    const { return m_collided_right; }

    int  const get_lives()            const { return m_lives; }
    void dec_live()                         { --m_lives; }
    void set_lives(int new_lives)           { m_lives = new_lives; }

    void set_position(glm::vec3 pos)        { m_position = pos; }
    void set_speed(float spd)              { m_speed    = spd; }
    void set_jumping_power(float jp)        { m_jumping_power = jp; }
    void set_ai_type(AIType a)             { m_ai_type  = a; }
    void set_ai_state(AIState s)           { m_ai_state = s; }
};

#endif
