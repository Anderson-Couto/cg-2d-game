#include "window.hpp"

using namespace std;
void Window::onEvent(SDL_Event const &event) {
  // Keyboard events
  if (event.type == SDL_KEYDOWN) {
    if (event.key.keysym.sym == SDLK_SPACE)
      m_gameData.m_input.set(static_cast<size_t>(Input::Start));
    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
      m_gameData.m_input.set(static_cast<size_t>(Input::Left));
    if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
      m_gameData.m_input.set(static_cast<size_t>(Input::Right));
  }
  if (event.type == SDL_KEYUP) {
    if (event.key.keysym.sym == SDLK_SPACE)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Start));
    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Left));
    if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Right));
  }

}

void Window::onCreate() {
  auto const assetsPath{abcg::Application::getAssetsPath()};

  // Load a new font
  auto const filename{assetsPath + "Inconsolata-Medium.ttf"};
  m_font = ImGui::GetIO().Fonts->AddFontFromFileTTF(filename.c_str(), 60.0f);
  if (m_font == nullptr) {
    throw abcg::RuntimeError("Cannot load font file");
  }

  m_font_small = ImGui::GetIO().Fonts->AddFontFromFileTTF(filename.c_str(), 24.0f);
  if (m_font_small == nullptr) {
    throw abcg::RuntimeError("Cannot load font file");
  }

  m_objectsProgram =
      abcg::createOpenGLProgram({{.source = assetsPath + "objects.vert",
                                  .stage = abcg::ShaderStage::Vertex},
                                 {.source = assetsPath + "objects.frag",
                                  .stage = abcg::ShaderStage::Fragment}});

  abcg::glClearColor(0, 0, 0, 1);

#if !defined(__EMSCRIPTEN__)
  abcg::glEnable(GL_PROGRAM_POINT_SIZE);
#endif

  // Start pseudo-random number generator
  auto seed{std::chrono::steady_clock::now().time_since_epoch().count()};
  m_randomEngine.seed(seed);

  restart();
}

void Window::restart() {
  m_gameData.m_state = State::Playing;
  m_paddle.create(m_objectsProgram);
  m_ball.create(m_objectsProgram);
}

void Window::onUpdate() {
  auto const deltaTime{gsl::narrow_cast<float>(getDeltaTime())};

  if (m_gameData.m_state != State::Playing &&
      m_restartWaitTimer.elapsed() > 5) {
    restart();
    return;
  }

  m_paddle.update(m_gameData);
  m_ball.update(m_paddle, m_gameData, deltaTime);

  if (m_gameData.m_state == State::Playing) {
    // checkCollisions();
    checkWinCondition();
    checkLossCondition();
  }
}

void Window::onPaint() {
  abcg::glClear(GL_COLOR_BUFFER_BIT);
  abcg::glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);

  m_paddle.paint(m_gameData);
  m_ball.paint();
}

void Window::onPaintUI() {
  abcg::OpenGLWindow::onPaintUI();
  {
    auto count{ fmt::format("{}", m_ball.m_top_count )};
    auto const sizeStart{ImVec2(m_viewportSize.x, m_viewportSize.y)};
    auto const positionStart{
        ImVec2((m_viewportSize.x) / 23.0f, (m_viewportSize.y) / 17.0f)};
    ImGui::SetNextWindowPos(positionStart);
    ImGui::SetNextWindowSize(sizeStart);
    ImGuiWindowFlags const flags{ImGuiWindowFlags_NoBackground |
                                 ImGuiWindowFlags_NoTitleBar |
                                 ImGuiWindowFlags_NoNavInputs};
    ImGui::Begin(" ", nullptr, flags);
    ImGui::PushFont(m_font_small);
    ImGui::Text(count.c_str());
    ImGui::PopFont();
    ImGui::End();

    auto size{ImVec2(300, 85)};
    auto const position{ImVec2((m_viewportSize.x - size.x) / 2.0f,
                               (m_viewportSize.y - size.y) / 2.0f)};
    ImGui::SetNextWindowPos(position);
    ImGui::SetNextWindowSize(size);
    ImGui::Begin(" ", nullptr, flags);

    ImGui::PushFont(m_font);

    if (m_gameData.m_state == State::GameOver) {
      ImGui::Text("Game Over!");
    } else if (m_gameData.m_state == State::Win) {
      ImGui::Text("*You Win!*");
    }

    ImGui::PopFont();
    ImGui::End();
  }
}

void Window::onResize(glm::ivec2 const &size) {
  m_viewportSize = size;

  abcg::glClear(GL_COLOR_BUFFER_BIT);
}

void Window::onDestroy() {
  abcg::glDeleteProgram(m_objectsProgram);

  m_ball.destroy();
  m_paddle.destroy();
}

void Window::checkWinCondition() {
  if ( m_ball.m_top_count >= 10 ) {
    m_gameData.m_state = State::Win;
    m_restartWaitTimer.restart();
  }
}

void Window::checkLossCondition() {
  if (m_ball.m_translation.y <= m_paddle.m_translation.y) {
    m_gameData.m_state = State::GameOver;
    m_restartWaitTimer.restart();
  }
}
