#include "chip8.h"
#include "display.h"

#include "sys/time.h"

#define CLOCK_FREQ 60
#define CLOCK_PERIOD (1000 / CLOCK_FREQ)

void runGlWindow(chip8* machine);

void tick(chip8* machine);

int main(int argc, char* argv[])
{
    chip8 machine;
    FILE* rom = fopen("roms/test.c8", "rb");
    if (rom == NULL) {
        fprintf(stderr, "File does not exist\n");
        exit(1);
    }
    load(&machine, rom);
    fclose(rom);

    runGlWindow(&machine);
}

void runGlWindow(chip8* machine)
{

    uint32_t indices[MAX_X * MAX_Y * 6];

    float vertices[(MAX_X + 1) * (MAX_Y + 1) * 3];

    loadVertices(vertices);

    generateIndicesFromDisplay(machine->screen, indices);

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

        if (!(machine->DT > 0)) {
            tick(machine);
            if (machine->DF) {
                generateIndicesFromDisplay(machine->screen, indices);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);
                machine->DF = 0;
            }
        }

        static struct timeval prev;
        struct timeval now;
        gettimeofday(&now, NULL);

        if (((now.tv_sec - prev.tv_sec) * 1000 + (now.tv_usec - prev.tv_usec) / 1000) >= CLOCK_PERIOD) {
            update_timers(machine);
            prev = now;
        }

        glDrawElements(GL_TRIANGLES, ACTIVE_DISPLAY_PIXELS * 6, GL_UNSIGNED_INT, indices);
        // glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
        // wait(1);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

void tick(chip8* machine)
{
    static uint8_t key_change;

    uint16_t inst = (machine->memory[machine->PC] << 8)
        | machine->memory[machine->PC + 1];
    exec(machine, inst);

    for (int i = 0; i < 16; i++) {
        printf("%x ", machine->V[i]);
    }
    printf("\n");
    printf("I: %x, SP: %x, PC: %x, DT: %x, Inst: %x, Mem: %x %x %x %x %x\n\n",
        machine->I, machine->SP, machine->PC, machine->DT, inst,
        machine->memory[machine->I],
        machine->memory[machine->I+1],
        machine->memory[machine->I+2],
        machine->memory[machine->I+3],
        machine->memory[machine->I+4]

    );

    if (KEY_VAL != -1) {
        machine->keyState[KEY_VAL] = 1;
        key_change = 1;

    } else if (KEY_VAL == -1 && key_change) {
        for (int i = 0; i < 16; i++) {
            machine->keyState[KEY_VAL] = 0;
        }
        key_change = 0;
    }
}