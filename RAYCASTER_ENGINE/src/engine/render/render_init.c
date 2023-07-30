#include "render_internal.h"
#include "../util.h"
#include "../io/file_io.h"

SDL_Window* render_init_window(u32 width, u32 height, const char* window_name) {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        ERROR_EXIT("Failed to intialise sdl!\n");

    SDL_Window* window = SDL_CreateWindow(
        window_name,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        SDL_WINDOW_OPENGL
    );

    if (!window)
        ERROR_EXIT("Failed to create sdl window!\n");

    if (!SDL_GL_CreateContext(window))
        ERROR_EXIT("Failed to create sdl context\n");

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
        ERROR_EXIT("Failed to wrangle opengl functions!\n");

    return window;
}

u32 render_init_shaders(const char* vertex_path, const char* fragment_path) {
    File vertex_source = io_file_read(vertex_path);
    if (!vertex_source.is_valid) {
        ERROR_RETURN(0, "Failed to read in vertex shader\n");
    }

    u32 vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_source.file_data, NULL);
    glCompileShader(vertex_shader);
    free(vertex_source.file_data);

    int success;
    char info_log[512];

    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, sizeof(info_log), NULL, info_log);
        ERROR_RETURN(0, "Failed to compile vertex shader:\n%s", info_log);
    }

    File fragment_source = io_file_read(fragment_path);
    if (!fragment_source.is_valid) {
        ERROR_RETURN(0, "Failed to read in fragment shader\n");
    }

    u32 fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_source.file_data, NULL);
    glCompileShader(fragment_shader);
    free(fragment_source.file_data);

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, sizeof(info_log), NULL, info_log);
        ERROR_RETURN(0, "Failed to compile fragment shader:\n%s", info_log);
    }

    u32 shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program, sizeof(info_log), NULL, info_log);
        ERROR_RETURN(0, "Failed to link shaders:\n%s", info_log);
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return shader_program;
}

void render_init_texture_default(u32* texture_id) {
    static const u8 texture_data[] = {
        255, 255, 255, 255
    };

    glGenTextures(1, texture_id);
    glBindTexture(GL_TEXTURE_2D, *texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_data);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void render_init_quad(u32* vbo_quad, u32* ebo_quad, u32* vao_quad) {
    static const f32 vertices[] = {
         0.5,  0.5, 0, 1, 1,
         0.5, -0.5, 0, 1, 0,
        -0.5, -0.5, 0, 0, 0,
        -0.5,  0.5, 0, 0, 1
    };

    static const u32 indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    glGenVertexArrays(1, vao_quad);
    glBindVertexArray(*vao_quad);

    glGenBuffers(1, vbo_quad);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo_quad);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 20, vertices, GL_STATIC_DRAW);

    glGenBuffers(1, ebo_quad);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ebo_quad);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * 6, indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, NULL);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(sizeof(float) * 3));

    glBindVertexArray(0);
}

void render_init_line(u32* vbo_line, u32* vao_line) {
    glGenVertexArrays(1, vao_line);
    glBindVertexArray(*vao_line);

    glGenBuffers(1, vbo_line);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo_line);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(f32), NULL, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), NULL);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}
