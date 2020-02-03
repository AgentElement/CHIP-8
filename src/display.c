#include "display.h"
GLFWwindow* createWindow()
{
    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    GLFWwindow* window
        = glfwCreateWindow(2000, 1000, "Chip-8 Emulator", NULL, NULL);

    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // Set callbacks
    glfwSetKeyCallback(window, key_callback);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { // glad stuff?
        exit(EXIT_FAILURE);
    }

    glfwSwapInterval(1);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set GL_COLOR_BUFFER_BIT

    return window;
}

void loadVertices(float* vertices)
{
    for (int i = 0; i < (MAX_X + 1) * (MAX_Y + 1); i++) {
        // int index_x = i % (MAX_X + 1);
        // int index_y = i / (MAX_Y + 1);

        vertices[3 * i] = -1.0f + ((float)(i % (MAX_X + 1)) * 2) / MAX_X;
        vertices[3 * i + 1] = 1.0f - ((float)(i / (MAX_X + 1)) * 2) / MAX_Y;
        vertices[3 * i + 2] = 0.0f;
    }
}

static void indicesFromPixel(int pixel, uint32_t* indexArray)
{
    int px_x = pixel % MAX_X;
    int px_y = pixel / MAX_X;

    indexArray[0] = px_y * (MAX_X + 1) + px_x;
    indexArray[1] = px_y * (MAX_X + 1) + px_x + 1;
    indexArray[2] = (px_y + 1) * (MAX_X + 1) + px_x;
    indexArray[3] = (px_y + 1) * (MAX_X + 1) + px_x + 1;
    indexArray[4] = indexArray[1];
    indexArray[5] = indexArray[2];
}

void generateIndicesFromDisplay(uint8_t* display, uint32_t* indices)
{
    uint32_t next = 0;
    uint32_t indexBuffer[6];

    ACTIVE_DISPLAY_PIXELS = 0;

    for (int i = 0; i < MAX_X * MAX_Y; i++) {
        if (display[i]) {
            ACTIVE_DISPLAY_PIXELS++;
            indicesFromPixel(i, indexBuffer);
            for (int j = 0; j < 6; j++) {
                indices[next + j] = indexBuffer[j];
            }
            next += 6;
        }
    }
}

char* readShaderFile(char* filename)
{
    char* buffer = 0;
    long fLen;
    FILE* f = fopen(filename, "rb");

    if (f) {
        fseek(f, 0, SEEK_END);
        fLen = ftell(f);
        fseek(f, 0, SEEK_SET);
        buffer = malloc(fLen + 1);
        if (buffer) {
            fread(buffer, 1, fLen, f);
        }
        fclose(f);
        buffer[fLen + 1] = '\0';
    }
    return buffer; // Must free buffer whenever used.
}

uint32_t
generateShaderProgram(char* vertexShaderFile, char* fragmentShaderFile)
{
    const char*
        vertexShaderSource
        = (const char*)readShaderFile(vertexShaderFile);

    // Create and compile vertex shader
    uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Check if vertex shader compiles
    uint32_t success;
    char log[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, log);
        printf(log, '\n');
    }

    const char*
        fragmentShaderSource
        = (const char*)readShaderFile(fragmentShaderFile);

    // Create and compile fragment shader
    uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Check if fragment shader compiles
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, log);
        printf(log, '\n');
    }

    // Generate and link shader program
    uint32_t shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Cleanup
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);

    // Check if shader program has been linked
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, log);
        printf(log, '\n');
    }

    return shaderProgram; // use with glUseProgram(shaderProgram)
}

static void
error_callback(int error, const char* description) // Error handling
{
    fprintf(stderr, "Error: %s\n", description);
}

static void
key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
// Key handling
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        // Quit if esc is pressed
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    if (action == GLFW_PRESS) {

        switch (key) {
        case GLFW_KEY_1:
            KEY_VAL = 0x1;
            break;

        case GLFW_KEY_2:
            KEY_VAL = 0x2;
            break;

        case GLFW_KEY_3:
            KEY_VAL = 0x3;
            break;

        case GLFW_KEY_4:
            KEY_VAL = 0xC;
            break;

        case GLFW_KEY_Q:
            KEY_VAL = 0x4;
            break;

        case GLFW_KEY_W:
            KEY_VAL = 0x5;
            break;

        case GLFW_KEY_E:
            KEY_VAL = 0x6;
            break;

        case GLFW_KEY_R:
            KEY_VAL = 0xD;
            break;

        case GLFW_KEY_A:
            KEY_VAL = 0x7;
            break;

        case GLFW_KEY_S:
            KEY_VAL = 0x8;
            break;

        case GLFW_KEY_D:
            KEY_VAL = 0x9;
            break;

        case GLFW_KEY_F:
            KEY_VAL = 0xE;
            break;

        case GLFW_KEY_Z:
            KEY_VAL = 0xA;
            break;

        case GLFW_KEY_X:
            KEY_VAL = 0x0;
            break;

        case GLFW_KEY_C:
            KEY_VAL = 0xB;
            break;

        case GLFW_KEY_V:
            KEY_VAL = 0xF;
            break;
        }
    }
    if (action == GLFW_RELEASE) {
        KEY_VAL = -1;
    }
}

static void
framebuffer_size_callback(GLFWwindow* window, int width, int height)
// Change size of viewport when window is resized
{
    glViewport(0, 0, width, height);
}
