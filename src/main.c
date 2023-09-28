#include <glad/glad.h>

#include "cglm/cglm.h"
#include <GLFW/glfw3.h>

#include "shaderProgram.h"
#include <stdio.h>

unsigned int vertexBuffer;
unsigned int vertexArray;

unsigned int elementBuffer;

void buildCircle(float radius, int vertex_count, vec3 **out_vertices, unsigned int **out_indices, size_t *index_count) {
    float angle = 360.0f / vertex_count;

    int triangleCount = vertex_count - 2;

    *out_vertices = calloc(vertex_count, sizeof(vec3));
    *out_indices = calloc(triangleCount * 3, sizeof(unsigned int));
    *index_count = triangleCount * 3;

    for (int i = 0; i < vertex_count; i++) {
        float currentAngle = angle * i;
        vec3 vertex;
        vertex[0] = radius * cosf(glm_rad(currentAngle));
        vertex[1] = radius * sinf(glm_rad(currentAngle));
        vertex[2] = 0.0f;

        glm_vec3_make(vertex, (*out_vertices)[i]);
    }

    for (int i = 0; i < triangleCount; i++) {
        int out_index = i * 3;
        (*out_indices)[out_index] = 0;
        (*out_indices)[out_index + 1] = i + 1;
        (*out_indices)[out_index + 2] = i + 2;
    }
}

int main(int argc, char *argv[]) {
    if (!glfwInit()) return -1;

    GLFWwindow *window = glfwCreateWindow(640, 480, "Hello Window", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Error: Could not create a window!\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        fprintf(stderr, "Error: Failed to initialize GLAD\n");
        glfwTerminate();
        return -1;
    }

    vec3 *vertices;
    unsigned int *indices;
    size_t index_count;
    int vertex_count = 128;
    buildCircle(1, vertex_count, &vertices, &indices, &index_count);

    ShaderProgram *myProgram = shaderProgram_create();
    shaderProgram_attach(myProgram, "./shaders/vs.glsl", GL_VERTEX_SHADER);
    shaderProgram_attach(myProgram, "./shaders/fs.glsl", GL_FRAGMENT_SHADER);
    shaderProgram_link(myProgram);

    glGenVertexArrays(1, &vertexArray);
    glGenBuffers(1, &vertexBuffer);

    glGenBuffers(1, &elementBuffer);

    glBindVertexArray(vertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

    glBufferData(GL_ARRAY_BUFFER, (long long) (vertex_count * sizeof(vec3)), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);

    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * index_count, indices, GL_STATIC_DRAW);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.0f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shaderProgram_use(myProgram);

        glBindVertexArray(vertexArray);

        glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
