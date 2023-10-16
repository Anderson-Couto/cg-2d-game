#include "ball.hpp"

#include <iostream>
#include <random>

void Ball::create(GLuint program) {
  destroy();
  m_randomEngine.seed(
      std::chrono::steady_clock::now().time_since_epoch().count());

  std::uniform_int_distribution<int> intDistribution(0, 1);

  if (intDistribution(m_randomEngine) == 1) {
    m_left = true;
    m_right = false;
  } else {
    m_right = true;
    m_left = false;
  }

  m_down = true;
  m_paddle = false;
  m_top_count = 0;
  m_program = program;
  m_colorLoc = abcg::glGetUniformLocation(m_program, "color");
  m_scaleLoc = abcg::glGetUniformLocation(m_program, "scale");
  m_translationLoc = abcg::glGetUniformLocation(m_program, "translation");
  m_translation = glm::vec2(0, 0.7f);

  auto const sides{10};

  std::vector<glm::vec2> positions(0);
  positions.emplace_back(0, 0);
  auto const step{M_PI * 2 / sides};
  for (auto const angle : iter::range(0.0, M_PI * 2, step)) {
    positions.emplace_back(std::cos(angle), std::sin(angle));
  }
  positions.push_back(positions.at(1));

  // Generate VBO of positions
  abcg::glGenBuffers(1, &m_VBO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  abcg::glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2),
                     positions.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  auto const positionAttribute{
      abcg::glGetAttribLocation(m_program, "inPosition")};

  // Create VAO
  abcg::glGenVertexArrays(1, &m_VAO);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(m_VAO);

  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);
}

void Ball::paint() {
  abcg::glUseProgram(m_program);

  abcg::glBindVertexArray(m_VAO);
  abcg::glUniform4f(m_colorLoc, 1, 1, 1, 0);
  abcg::glUniform1f(m_scaleLoc, m_scale);

  abcg::glUniform2f(m_translationLoc, m_translation.x, m_translation.y);
  abcg::glDrawArrays(GL_TRIANGLE_FAN, 0, 12);

  abcg::glBindVertexArray(0);

  abcg::glUseProgram(0);
}

void Ball::destroy() {
  abcg::glDeleteBuffers(1, &m_VBO);
  abcg::glDeleteVertexArrays(1, &m_VAO);
  m_left = false, m_right = false, m_up = false;
}

// return the position of the ball in the game
bool Ball::checkPos(Paddle &paddle) {
  return m_paddle = paddle.m_translation.x - 0.12 <= m_translation.x &&
                    paddle.m_translation.x + 0.12 >= m_translation.x;
}

void Ball::update(Paddle &paddle, const GameData &gameData) {
  if (gameData.m_state == State::Playing &&
      m_ballTimer.elapsed() > 10.0 / 1000.0) {
    m_ballTimer.restart();

    // check the borders to change the direction of the ball
    if (m_translation.x >= 0.99f) {
      m_left = true;
      m_right = false;
    }

    if (m_translation.x <= -0.99f) {
      m_left = false;
      m_right = true;
    }

    if (m_translation.y >= 0.99f) {
      m_down = true;
      m_up = false;
      m_top_count += 1;
    }

    // when the ball hit the paddle it changes the direction

    // when the ball hit a brick, it changes the direction according to which
    // side of the brick the ball hit (up, down, left or right side)
    if (m_paddle) {
      m_down = false;
      m_up = true;
      m_paddle = false;
    }

    // directions of the ball
    if (m_left) {
      if (m_translation.x < -0.0f) {
        if (m_translation.x - 0.01f < -0.99f) {
          m_translation.x = -0.99f;
        } else {
          m_translation.x -= 0.01f;
        }
      } else {
        m_translation.x -= 0.01;
      }

    } else if (m_right) {
      if (m_translation.x < -0.0f) {
        m_translation.x += 0.01;
      } else {
        if (m_translation.x + 0.01f < 0.99f) {
          m_translation.x += 0.01f;
        } else {
          m_translation.x = 0.99f;
        }
      }
    }

    if (m_down) {
      if (m_translation.y < -0.0f) {
        if (m_translation.y - 0.01f > -0.87f) {
          m_translation.y -= 0.01;
        } else {
          if (checkPos(paddle)) {
            m_translation.y = -0.87f;
          } else {
            m_translation.y -= 0.01;
          }
        }
      } else {
        m_translation.y -= 0.01;
      }

    } else if (m_up) {
      if (m_translation.y < -0.0f) {
        m_translation.y += 0.01f;
      } else {
        if (m_translation.y + 0.01f < 0.99f) {
          m_translation.y += 0.01f;
        } else {
          m_translation.y = 0.99f;
        }
      }
    }
  }
}