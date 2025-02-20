#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Check for OpenGL errors
void check_gl_error(const char* stmt) {
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        fprintf(stderr, "OpenGL error at %s: %d\n", stmt, err);
    }
}

// Read shader source
char* read_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Failed to open %s\n", filename);
        return NULL;
    }
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* content = malloc(length + 1);
    fread(content, 1, length, file);
    content[length] = '\0';
    fclose(file);
    return content;
}

// Load and compile shader
GLuint load_shader(const char* filename, GLenum type) {
    char* source = read_file(filename);
    if (!source) return 0;
    GLuint shader = glCreateShader(type);
    const GLchar* sources[] = { source };
    glShaderSource(shader, 1, sources, NULL);
    glCompileShader(shader);
    free(source);
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (!status) {
        char log[512];
        glGetShaderInfoLog(shader, 512, NULL, log);
        fprintf(stderr, "Shader compile error in %s: %s\n", filename, log);
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

int main() {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Get primary monitor and its 4K video mode
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    if (mode->width < 3840 || mode->height < 2160) {
        fprintf(stderr, "Monitor does not support 4K resolution\n");
        glfwTerminate();
        return -1;
    }

    // Create fullscreen window at 4K
    GLFWwindow* window = glfwCreateWindow(3840, 2160, "Surprise Shader", monitor, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create fullscreen window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        glfwTerminate();
        return -1;
    }

    // Set viewport to 4K
    glViewport(0, 0, 3840, 2160);

    // Full-screen quad
    float vertices[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f,  1.0f
    };

    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    check_gl_error("Vertex setup");

    GLuint vertex_shader = load_shader("shader.vert", GL_VERTEX_SHADER);
    GLuint fragment_shader = load_shader("shader.frag", GL_FRAGMENT_SHADER);
    if (!vertex_shader || !fragment_shader) {
        glfwTerminate();
        return -1;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (!status) {
        char log[512];
        glGetProgramInfoLog(program, 512, NULL, log);
        fprintf(stderr, "Program link error: %s\n", log);
        glDeleteProgram(program);
        glfwTerminate();
        return -1;
    }
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    // Uniform locations
    GLint time_loc = glGetUniformLocation(program, "time");
    GLint resolution_loc = glGetUniformLocation(program, "resolution");

    glUseProgram(program);
    glUniform2f(resolution_loc, 3840.0f, 2160.0f); // Update to 4K resolution
    check_gl_error("Static uniform setup");

    double start_time = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }

        float time = (float)(glfwGetTime() - start_time);
        glUniform1f(time_loc, time);
        check_gl_error("Time uniform update");

        glClear(GL_COLOR_BUFFER_BIT);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        check_gl_error("Draw call");
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(program);
    glfwTerminate();
    return 0;
}
