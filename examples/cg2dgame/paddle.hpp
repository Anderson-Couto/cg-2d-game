#ifndef PADDLE_HPP_
#define PADDLE_HPP_

#include "abcgOpenGL.hpp"
#include "gamedata.hpp"

class Paddle {
public:
    void create(GLuint program);
    void paint(const GameData &gameData);
    void destroy();
    void update(const GameData &gameData);

    GLuint m_VAO{};
    GLuint m_VBO{};
    GLuint m_EBO{};
    glm::vec4 m_color{1};
    float m_rotation{};
    float m_scale{0.1f};
    glm::vec2 m_translation{glm::vec2(0)};
    abcg::Timer m_trailBlinkTimer;
    abcg::Timer m_paddleTimer;

private:
    GLuint m_program{};
    GLint m_translationLoc{};
    GLint m_colorLoc{};
    GLint m_scaleLoc{};
    GLint m_rotationLoc{};
};

#endif
