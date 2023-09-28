#include "square.h"

struct square {
    vec4 color;
    vec3 position;
    float length;
    square_direction direction;
};

square *square_create(float x, float y, float length) {
    square *result = malloc(sizeof(struct square));
    glm_vec3_make((vec3){x, y, 0.0f}, result->position);
    glm_vec4_make((vec4){rand() % 25 / 25.0f, rand() % 25 / 25.0f, 0.0f, 0.0f}, result->color);
    result->length = length;
    result->direction = DIR_RIGHT;

    return result;
}

void square_destroy(square *square) {
    free(square);
}

void square_get_position(square *this, vec3 *out_position) {
    glm_vec3_make(this->position, *out_position);
}

void square_get_color(square *this, vec4 *out_color) {
    glm_vec4_make(this->color, *out_color);
}

square_direction square_get_direction(square *this) {
    return this->direction;
}

void square_move(square *this) {
    switch (this->direction) {
        case DIR_LEFT:
            glm_vec3_sub(this->position, (vec3){this->length, 0.0f, 0.0f}, this->position);
            break;
        case DIR_RIGHT:
            glm_vec3_add(this->position, (vec3){this->length, 0.0f, 0.0f}, this->position);
            break;
        case DIR_UP:
            glm_vec3_add(this->position, (vec3){0.0f, this->length, 0.0f}, this->position);
            break;
        case DIR_DOWN:
            glm_vec3_sub(this->position, (vec3){0.0f, this->length, 0.0f}, this->position);
            break;
    }
}

void square_set_direction(square *this, square_direction direction) {
    this->direction = direction;
}
