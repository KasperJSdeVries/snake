#ifndef SNAKE_SQUARE_H
#define SNAKE_SQUARE_H

#include "cglm/vec3.h"
#include "cglm/vec4.h"

typedef struct square square;

typedef enum square_direction {
    DIR_RIGHT = 1,
    DIR_LEFT = -1,
    DIR_UP = 2,
    DIR_DOWN = -2,
} square_direction;

square *square_create(float x, float y, float length);
void square_destroy(square *square);

void square_get_position(square *this, vec3 *out_position);
void square_get_color(square *this, vec4 *out_color);
square_direction square_get_direction(square *this);

void square_move(square *this);
void square_set_direction(square *this, square_direction direction);

#endif//SNAKE_SQUARE_H
