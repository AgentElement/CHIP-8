#include "chip8.h"
#include "display.h"

void runGlWindow();

int main(int argc, char* argv[])
{
    runGlWindow();
}

void runGlWindow()
{

    uint32_t indices[MAX_X * MAX_Y * 6];

    uint8_t display[MAX_X * MAX_Y];

    float vertices[(MAX_X + 1) * (MAX_Y + 1) * 3];

    loadVertices(vertices);

    for (int i = 0; i < MAX_X * MAX_Y; i++) {
        display[i] = ((i % 3) == 1);
    }

    generateIndicesFromDisplay(display, indices);

    GLFWwindow* window = createWindow();

    uint32_t VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Copy vertex data to VBO memory. GL_STATIC_DRAW: data does not change
    // when redrawn.
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    uint32_t shaderProgram = generateShaderProgram(
        "shaders/vertexshader.glsl",
        "shaders/fragmentshader.glsl");

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, ACTIVE_DISPLAY_PIXELS * 6, GL_UNSIGNED_INT, indices);
        // glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}