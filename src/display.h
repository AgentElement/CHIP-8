#ifndef DISPLAY_H
#define DISPLAY_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "linmath.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "chip8.h"

uint32_t ACTIVE_DISPLAY_PIXELS;


int8_t KEY_VAL;


void loadVertices(float* vertices);

static void indicesFromPixel(int pixel, uint32_t* indexArray);

void generateIndicesFromDisplay(uint8_t* display, uint32_t* indices);

char* readShaderFile(char* filename);

uint32_t
generateShaderProgram(char* vertexShaderFile, char* fragmentShaderFile);

static void
error_callback(int error, const char* description); // Error handling

static void
key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

static void
framebuffer_size_callback(GLFWwindow* window, int width, int height);

GLFWwindow* createWindow();

void runGlWindow();

#endif // DISPLAY_H