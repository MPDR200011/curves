#include "bezier.hpp"
#include "utils.hpp"
#include <algorithm>

const float pointRadius = 10.0f;
const float lineRadius = 2.0f;
const int num_points = 200;
const float increment = 1.0/num_points;

BezierPoint::BezierPoint(sf::Vector2f position): 
  m_position(position) {}

BezierInput::BezierInput(BezierEngine* engine): m_engine{engine}, 
  m_currentState{ DEFAULT } {}

void BezierInput::MouseMoved(sf::Vector2f vector) {
  if (m_currentState == DEFAULT) return;
  switch (m_currentState) {
    case MOVING_POINT:
      {
        m_engine->UpdatePointPosition(*m_focusedPoint, vector);
      }
      break;
    default:
      break;
  }
}

void BezierInput::ButtonPressed(sf::Mouse::Button button, sf::Vector2f position) {
  if (button != sf::Mouse::Left) return;

  auto point = m_engine->GetHitPointIdx(position);
  if (point) {
    m_focusedPoint = point;
    m_currentState = MOVING_POINT;
  } else {
    m_focusedPoint = m_engine->AddPoint(position);
    m_currentState = MOVING_POINT;
  }
}

void BezierInput::ButtonReleased(sf::Mouse::Button button, sf::Vector2f position) {
  if (button != sf::Mouse::Left) return;

  m_focusedPoint.reset();
  m_currentState = DEFAULT;
}

void BezierInput::KeyPressed(sf::Event::KeyEvent event) {
  if (m_currentState != DEFAULT) return;
  switch (event.code) {
    case sf::Keyboard::S:
      m_engine->ToggleSmoothMode();
      break;
    case sf::Keyboard::Z:
      {
        if (m_engine->DeleteLastPoint() == m_focusedPoint) m_focusedPoint.reset();
      }
      break;
    case sf::Keyboard::Left:
      m_engine->DecrementDegree();
      break;
    case sf::Keyboard::Right:
      m_engine->IncrementDegree();
      break;
    default:
      break;
  }
}
void BezierInput::KeyReleased(sf::Event::KeyEvent event) {}

BezierEngine::BezierEngine(unsigned degree): 
  m_degree(degree), m_smoothMode{false} { }

sf::Vector2f BezierEngine::InterpolateFromPoint(float t, size_t pointIdx) const {
  sf::Vector2f positions[m_degree+1];
  for (size_t i = 0; i < m_degree + 1; ++i) {
    positions[i] = m_points[pointIdx+i].position();
  }

  for (unsigned degree = m_degree; degree > 0; --degree) {
    for (size_t i = 0; i < degree; i++) {
      auto vector = positions[i+1] - positions[i];
      positions[i] += vector * t;
    }
  }

  return positions[0];
}

void BezierEngine::RenderLine(sf::RenderWindow& window) const {
  sf::CircleShape pointShape{pointRadius};
  pointShape.setOrigin({pointRadius, pointRadius});

  sf::CircleShape lineShape{lineRadius};
  lineShape.setOrigin({lineRadius, lineRadius});

  for (size_t i = 0; i + 1 < m_points.size(); ++i) {
    sf::Vertex vertices[2] = {
      { m_points[i].position(), sf::Color{180,180,180} },
      { m_points[i+1].position(), sf::Color{180,180,180} }
    };

    window.draw(vertices, 2, sf::Lines);
  }

  size_t counter = 0;
  for (BezierPoint const & point: m_points) {
    pointShape.setPosition(point.position());
    if (counter % m_degree == 0) {
      //edge point
      pointShape.setFillColor(sf::Color{0,255,0});
      window.draw(pointShape);
    } else {
      //control point
      pointShape.setFillColor(sf::Color{255,255,255});
      window.draw(pointShape);
    }
    counter++;
  }

  for (int i = 0; i + m_degree < m_points.size(); i+= m_degree) {
    for (int j = 0; j < num_points; ++j) {
      lineShape.setPosition(InterpolateFromPoint(increment * j, i));
      window.draw(lineShape);
    }
  }
}

size_t BezierEngine::AddPoint(sf::Vector2f coords) {
  size_t idx = m_points.size();
  m_points.emplace_back(coords);
  if (m_smoothMode && idx % m_degree == 1 && idx > 1) {
    auto direction = m_points[idx-1].position() - coords;
    m_points[idx-2].m_position = m_points[idx-1].position() + direction;
  }
  return idx;
}


void BezierEngine::UpdatePointPosition(size_t idx, sf::Vector2f pos) {
  if (m_smoothMode) {
    if (idx % m_degree == 0) {
      auto direction = pos - m_points[idx].position();
      if (idx > 0) m_points[idx-1].m_position += direction;
      if (idx + 1 < m_points.size()) m_points[idx+1].m_position += direction;
    } else if (idx % m_degree == m_degree-1 && idx + 2 < m_points.size()) {
      auto direction = m_points[idx+1].position() - pos;
      m_points[idx+2].m_position = m_points[idx+1].position() + direction;
    } else if (idx % m_degree == 1 && idx > 1) {
      auto direction = m_points[idx-1].position() - pos;
      m_points[idx-2].m_position = m_points[idx-1].position() + direction;
    }
  }
  m_points[idx].m_position = pos;
}

size_t BezierEngine::DeleteLastPoint() {
  m_points.pop_back();
  return m_points.size();
}

void BezierEngine::ToggleSmoothMode() {
  m_smoothMode = !m_smoothMode;
  CorrectSmooth();
}

void BezierEngine::IncrementDegree() {
  m_degree = std::clamp(m_degree + 1, 1U, 6U);
  CorrectSmooth();
}
void BezierEngine::DecrementDegree() {
  m_degree = std::clamp(m_degree - 1, 1U, 6U);
  CorrectSmooth();
}

void BezierEngine::CorrectSmooth() {
  if (m_degree < 3) {
    m_smoothMode = false;
    return;
  }
  if (m_smoothMode) {
    for (size_t i = m_degree-1; i + 2 < m_points.size(); i+=m_degree) {
      auto direction = m_points[i+1].position() - m_points[i].position();
      m_points[i+2].m_position = m_points[i+1].position() + direction;
    }
  }
}

std::unique_ptr<InputHandler> BezierEngine::GetInputHandler() {
  return std::make_unique<BezierInput>(this);
}

std::optional<size_t> BezierEngine::GetHitPointIdx(sf::Vector2f pos) {
  auto point = std::find_if(std::begin(m_points), std::end(m_points), 
    [&pos](auto point) {
      return square_dist(pos, point.position()) < pointRadius*pointRadius;
    });
  if (point == std::end(m_points)) return std::nullopt;
  return std::distance(std::begin(m_points), point);
}

BezierPoint& BezierEngine::GetPoint(long index) {
  return m_points[index];
}
