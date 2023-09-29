#include "kinetic/containers/darray.h"
#include "kinetic/game.h"
#include "kinetic/graphics/shaderProgram.h"
#include "square.h"
#include <unistd.h>

const float length = 0.08f;
float vertices[] = {
        -length / 2, length / 2, 0.0f,
        -length / 2, -length / 2, 0.0f,
        length / 2, -length / 2, 0.0f,

        -length / 2, length / 2, 0.0f,
        length / 2, -length / 2, 0.0f,
        length / 2, length / 2, 0.0f};

//void buildCircle(float radius, int vertex_count, vec3 **out_vertices, unsigned int **out_indices, size_t *index_count) {
//    float angle = 360.0f / vertex_count;
//
//    int triangleCount = vertex_count - 2;
//
//    *out_vertices = calloc(vertex_count, sizeof(vec3));
//    *out_indices = calloc(triangleCount * 3, sizeof(unsigned int));
//    *index_count = triangleCount * 3;
//
//    for (int i = 0; i < vertex_count; i++) {
//        float currentAngle = angle * i;
//        vec3 vertex;
//        vertex[0] = radius * cosf(glm_rad(currentAngle));
//        vertex[1] = radius * sinf(glm_rad(currentAngle));
//        vertex[2] = 0.0f;
//
//        glm_vec3_make(vertex, (*out_vertices)[i]);
//    }
//
//    for (int i = 0; i < triangleCount; i++) {
//        int out_index = i * 3;
//        (*out_indices)[out_index] = 0;
//        (*out_indices)[out_index + 1] = i + 1;
//        (*out_indices)[out_index + 2] = i + 2;
//    }
//}

square **snake_block_list = NULL;
ShaderProgram *myProgram;

void move_snake() {
    for (int i = 0; i < darray_length(snake_block_list); i++) {
        square_move(snake_block_list[i]);
    }

    for (size_t i = darray_length(snake_block_list) - 1; i > 0; i--) {
        square_set_direction(snake_block_list[i], square_get_direction(snake_block_list[i - 1]));
    }
}

void draw_snake(ShaderProgram *program) {
    for (int i = 0; i < darray_length(snake_block_list); i++) {
        vec3 position;
        square_get_position(snake_block_list[i], &position);
        shaderProgram_set_vec3(program, "uMove", position);

        vec4 color;
        square_get_color(snake_block_list[i], &color);
        shaderProgram_set_vec4(program, "uColor", color);

        shaderProgram_draw(program);
    }
}

void add_tail() {
    size_t element_count = darray_length(snake_block_list);
    if (element_count == 0) {
        darray_push(snake_block_list, square_create(0.0f, 0.0f, length));
    } else {
        square *last_square = snake_block_list[element_count - 1];
        vec3 pos;
        square_get_position(last_square, &pos);
        square_direction direction = square_get_direction(last_square);
        switch (direction) {
            case DIR_RIGHT:
                glm_vec3_sub(pos, (vec3){length, 0.0f, 0.0f}, pos);
                break;
            case DIR_LEFT:
                glm_vec3_add(pos, (vec3){length, 0.0f, 0.0f}, pos);
                break;
            case DIR_UP:
                glm_vec3_sub(pos, (vec3){0.0f, length, 0.0f}, pos);
                break;
            case DIR_DOWN:
                glm_vec3_add(pos, (vec3){0.0f, length, 0.0f}, pos);
                break;
        }
        square *new_block = square_create(pos[0], pos[1], length);
        square_set_direction(new_block, direction);
        darray_push(snake_block_list, new_block);
    }
}

void game_input_callback(key key) {
    if (key == KEY_ESCAPE) {
        exit_game();
    }

    if (darray_length(snake_block_list) != 0) {
        square *first = snake_block_list[0];
        if (key == KEY_A) {
            square_set_direction(first, DIR_LEFT);
        }
        if (key == KEY_D) {
            square_set_direction(first, DIR_RIGHT);
        }
        if (key == KEY_W) {
            square_set_direction(first, DIR_UP);
        }
        if (key == KEY_S) {
            square_set_direction(first, DIR_DOWN);
        }
    }
    if (key == KEY_SPACE) {
        add_tail();
    }
}

void game_setup() {
    snake_block_list = darray_create(square *);
    for (int i = 0; i < 5; ++i) {
        add_tail();
    }

    myProgram = shaderProgram_create();
    shaderProgram_attach(myProgram, "./shaders/vs.glsl", SHADER_TYPE_VERTEX);
    shaderProgram_attach(myProgram, "./shaders/fs.glsl", SHADER_TYPE_FRAGMENT);
    shaderProgram_link(myProgram);

    shaderProgram_add_uniform(myProgram, "uMove");
    shaderProgram_add_uniform(myProgram, "uColor");

    shaderProgram_set_vertices(myProgram, vertices, sizeof(vertices) / sizeof(float));
}

void game_loop() {
    shaderProgram_use(myProgram);

    draw_snake(myProgram);

    move_snake();

    sleep(1);
}

void game_cleanup() {
    for (int i = 0; i < darray_length(snake_block_list); ++i) {
        square *square;
        darray_pop(snake_block_list, &square);
        square_destroy(square);
    }


    shaderProgram_destroy(myProgram);
}
