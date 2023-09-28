#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include "darray.h"
#include "shaderProgram.h"
#include "square.h"
#include <stdio.h>
#include <unistd.h>

unsigned int vertexBuffer;
unsigned int vertexArray;

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

        glDrawArrays(GL_TRIANGLES, 0, 6);
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

void key_callback(GLFWwindow *window, int key, int scancode, int action, int modifiers) {
    if (key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    if (action == GLFW_PRESS) {
        if (darray_length(snake_block_list) != 0) {
            square *first = snake_block_list[0];
            if (key == GLFW_KEY_A) {
                square_set_direction(first, DIR_LEFT);
            }
            if (key == GLFW_KEY_D) {
                square_set_direction(first, DIR_RIGHT);
            }
            if (key == GLFW_KEY_W) {
                square_set_direction(first, DIR_UP);
            }
            if (key == GLFW_KEY_S) {
                square_set_direction(first, DIR_DOWN);
            }
        }
        if (key == GLFW_KEY_SPACE) {
            add_tail();
        }
    }
}

int main(int argc, char *argv[]) {
    if (!glfwInit()) return -1;

    GLFWwindow *window = glfwCreateWindow(800, 800, "Snake", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Error: Could not create a window!\n");
        glfwTerminate();
        return -1;
    }

    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        fprintf(stderr, "Error: Failed to initialize GLAD\n");
        glfwTerminate();
        return -1;
    }

    snake_block_list = darray_create(square *);
    for (int i = 0; i < 5; ++i) {
        add_tail();
    }

    ShaderProgram *myProgram = shaderProgram_create();
    shaderProgram_attach(myProgram, "./shaders/vs.glsl", GL_VERTEX_SHADER);
    shaderProgram_attach(myProgram, "./shaders/fs.glsl", GL_FRAGMENT_SHADER);
    shaderProgram_link(myProgram);

    shaderProgram_add_uniform(myProgram, "uMove");
    shaderProgram_add_uniform(myProgram, "uColor");

    glGenVertexArrays(1, &vertexArray);
    glGenBuffers(1, &vertexBuffer);

    glBindVertexArray(vertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);

    glEnableVertexAttribArray(0);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.0f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shaderProgram_use(myProgram);

        glBindVertexArray(vertexArray);

        draw_snake(myProgram);

        move_snake();

        sleep(1);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    for (int i = 0; i < darray_length(snake_block_list); ++i) {
        square *square;
        darray_pop(snake_block_list, &square);
        square_destroy(square);
    }

    shaderProgram_destroy(myProgram);
    glfwTerminate();
    return 0;
}
