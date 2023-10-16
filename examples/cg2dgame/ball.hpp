#ifndef BALL_HPP_
#define BALL_HPP_

#include <random>

#include "abcgOpenGL.hpp"
#include "gamedata.hpp"
#include "paddle.hpp"

class Ball {
public:
  void create(GLuint program);
  void paint();
  void destroy();
  void update(Paddle &paddle, const GameData &gameData);
  bool checkPos(Paddle &paddle);

  bool m_paddle;
  bool m_up, m_down, m_left, m_right;
  float m_scale{0.01f};
  int m_top_count;
  abcg::Timer m_ballTimer;
  std::default_random_engine m_randomEngine;
  glm::vec2 m_translation{glm::vec2(0)};

private:
  GLuint m_program{};
  GLint m_colorLoc{};
  GLint m_translationLoc{};
  GLint m_scaleLoc{};

  GLuint m_VAO{};
  GLuint m_VBO{};
};

#endif