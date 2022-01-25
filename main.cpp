#include "core/Engine.h"

int main() {
    engine::Engine _e;
    _e.onRun();
}

//#include <GL/glew.h>
//#include <GLFW/glfw3.h>
//#include <cstdlib>
//#include <glm/mat4x4.hpp>
//#include <glm/ext/matrix_clip_space.hpp>
//#include <glm/ext/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>
//#include <vector>
//#include <SDL2/SDL.h>
//
//#include "core/util/Logger.h"
//#include "core/render/elements/Sprite.h"
//
//float aspectRatio = 1280 / 720.f;
//
//struct Vertex {
//    glm::vec3 pos;
//    glm::vec4 color;
//    glm::vec2 textureCoords;
//};
//
//static const char* _vertexShaderSrc =
//        "#version 400\n"
//        "uniform mat4 MVP;\n"
//        "layout (location = 0) in vec3 vPos;\n"
//        "layout (location = 1) in vec4 vCol;\n"
//        "layout (location = 2) in vec2 vText;\n"
//        "uniform mat4 viewProjection;\n"
//        "out vec4 color;\n"
//        "out vec2 textCoords;\n"
//        "void main()\n"
//        "{\n"
//        "    gl_Position = viewProjection * vec4(vPos, 1.0);\n"
//        "    color = vCol;\n"
//        "    textCoords = vText;\n"
//        "}\n";
//
//static const char* _fragmentShaderSrc =
//        "#version 400\n"
//        "in vec4 color;\n"
//        "in vec2 textCoords;\n"
//        "uniform sampler2D tex;\n"
//        "void main()\n"
//        "{\n"
//        "    gl_FragColor = texture(tex, textCoords) * color;\n"
//        "}\n";
//
//static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
//    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
//        glfwSetWindowShouldClose(window, GLFW_TRUE);
//}
//
//int main(void) {
//    LOG_I(aspectRatio)
//
//    uint WindowFlags = SDL_WINDOW_OPENGL;
//    SDL_Window *Window = SDL_CreateWindow("OpenGL Test", 0, 0, 1280, 720, WindowFlags);
//    assert(Window);
//    SDL_GLContext Context = SDL_GL_CreateContext(Window);
//
//    bool Running = 1;
//    bool FullScreen = 0;
//
//
//    GLuint _vertexBuffer, _vertexShader, _fragmentShader, _program;
//    glewInit();
//    glGenBuffers(1, &_vertexBuffer);
//    glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) 0);
//    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) (sizeof(float) * 3));
//    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) (sizeof(float) * 3 + sizeof(float) * 4));
//    glEnableVertexAttribArray(0);
//    glEnableVertexAttribArray(1);
//    glEnableVertexAttribArray(2);
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//    _vertexShader = glCreateShader(GL_VERTEX_SHADER);
//    glShaderSource(_vertexShader, 1, &_vertexShaderSrc, NULL);
//    glCompileShader(_vertexShader);
//
//    _fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
//    glShaderSource(_fragmentShader, 1, &_fragmentShaderSrc, NULL);
//    glCompileShader(_fragmentShader);
//
//    _program = glCreateProgram();
//    glAttachShader(_program, _vertexShader);
//    glAttachShader(_program, _fragmentShader);
//    glLinkProgram(_program);
//
//    glDeleteShader(_vertexShader);
//    glDeleteShader(_fragmentShader);
//
//    glUseProgram(_program);
//
//    glm::mat4 _camara = glm::ortho(-aspectRatio, aspectRatio, -1.f, 1.f, -1.f, 1.f);
//    glm::mat4 _view = glm::translate(glm::mat4(1.0f), {0, 0, 0.0f}) * glm::rotate(glm::mat4(1.0f), glm::radians(0.f), glm::vec3(0, 0, 1));
//
//    _camara = _camara * _view;
//
//    std::vector<Vertex> _vertex;
//    int _currentWindowSize[2];
//
//    engine::Texture* _texture = new engine::Texture;
//    _texture->loadFromFile("assets/test.png");
//
//    engine::Sprite _sprite;
//    _sprite.setTexture(_texture);
//    _sprite.setShader(_program);
//    _sprite.setPosition({0, 0});
//
//    while (Running) {
//        SDL_Event Event;
//        while (SDL_PollEvent(&Event))
//        {
//            if (Event.type == SDL_KEYDOWN)
//            {
//                switch (Event.key.keysym.sym)
//                {
//                    case SDLK_ESCAPE:
//                        Running = 0;
//                        break;
//                    case 'f':
//                        FullScreen = !FullScreen;
//                        if (FullScreen)
//                        {
//                            SDL_SetWindowFullscreen(Window, WindowFlags | SDL_WINDOW_FULLSCREEN_DESKTOP);
//                        }
//                        else
//                        {
//                            SDL_SetWindowFullscreen(Window, WindowFlags);
//                        }
//                        break;
//                    default:
//                        break;
//                }
//            }
//            else if (Event.type == SDL_QUIT)
//            {
//                Running = 0;
//            }
//        }
//
//        glClear(GL_COLOR_BUFFER_BIT);
//        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
//
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, _texture->getGLTexture());
//
//        glm::mat4 _transform = glm::translate(glm::mat4(1.f), {0, 0, 0});
//
//        _vertex.emplace_back(Vertex{_transform * glm::vec4 {-0.5f, -0.5f, 1.f, 1.0f}, {1, 1, 1, 1}, {0, 0}});
//        _vertex.emplace_back(Vertex{_transform * glm::vec4 { 0.5f, -0.5f, 1.f, 1.0f}, {1, 1, 1, 1}, {1, 0}});
//        _vertex.emplace_back(Vertex{_transform * glm::vec4 { 0.5f,  0.5f, 1.f, 1.0f}, {1, 1, 1, 1}, {1, 1}});
//
//        _vertex.emplace_back(Vertex{_transform * glm::vec4 {-0.5f, -0.5f, 1.f, 1.0f}, {1, 1, 1, 1}, {0, 0}});
//        _vertex.emplace_back(Vertex{_transform * glm::vec4 { 0.5f,  0.5f, 1.f, 1.0f}, {1, 1, 1, 1}, {1, 1}});
//        _vertex.emplace_back(Vertex{_transform * glm::vec4 {-0.5f,  0.5f, 1.f, 1.0f}, {1, 1, 1, 1}, {0, 1}});
//
//        glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
//        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * _vertex.size(), &_vertex[0], GL_STATIC_DRAW);
//        glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//        GLint _location = glGetUniformLocation(_program, "viewProjection");
//        glUniformMatrix4fv(_location, 1, GL_FALSE, reinterpret_cast<const GLfloat *>(glm::value_ptr(_camara)));
//
//        // Este si es importante
//        glDrawArrays(GL_TRIANGLES, 0, _vertex.size());
//
//        _vertex.clear();
//
//        // Lo mismo, aun nada
//        SDL_GL_SwapWindow(Window);
//    }
//
//
//    delete _texture;
//    glDeleteProgram(_program);
//    glDeleteBuffers(1, &_vertexBuffer);
//
//    return 0;
//}
