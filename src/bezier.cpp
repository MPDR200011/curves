#include "bezier.hpp"
#include <iostream>
#include <algorithm>

const float pointRadius = 10.0f;
const float lineRadius = 2.0f;
const int num_points = 200;
const float increment = 1.0/num_points;

float square_dist(sf::Vector2f const & v1, sf::Vector2f const & v2) {
  float p1 = v1.x - v2.x;
  float p2 = v1.y - v2.y;
  return p1*p1 + p2*p2;
}

BezierPoint::BezierPoint(sf::Vector2f position): 
  m_position(position), focused(false) {

  }

BezierInput::BezierInput(BezierEngine* engine): m_engine(engine), 
  m_focusedPoint{-1}, m_currentState{ DEFAULT } {}

void BezierInput::MouseMoved(sf::Vector2f vector) {
  if (m_currentState == DEFAULT) return;
  switch (m_currentState) {
    case MOVING_POINT:
      {
        m_engine->UpdatePointPosition(m_focusedPoint, vector);
      }
      break;
    default:
      break;
  }
}

void BezierInput::ButtonPressed(sf::Mouse::Button button, sf::Vector2f position) {
  if (button != sf::Mouse::Left) return;

  long point = m_engine->GetHitPointIdx(position);
  if (point != -1) {
    m_focusedPoint = point;
    m_currentState = MOVING_POINT;
  } else {
    m_focusedPoint = m_engine->AddPoint(position);
    m_currentState = MOVING_POINT;
  }
}

void BezierInput::ButtonReleased(sf::Mouse::Button button, sf::Vector2f position) {
  if (button != sf::Mouse::Left) return;

  m_currentState = DEFAULT;
}

void BezierInput::KeyPressed(sf::Event::KeyEvent event) {
  switch (event.code) {
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

BezierEngine::BezierEngine(unsigned degree): m_degree(degree) {
  std::printf("Built BezierEngine for degree = %u.\n", m_degree);
}

sf::Vector2f BezierEngine::InterpolateFromPoint(float t, size_t pointIdx) const {
  sf::Vector2f positions[m_degree+1];
  for (int i = 0; i < m_degree + 1; ++i) {
    positions[i] = m_points[pointIdx+i].position();
  }

  for (unsigned degree = m_degree; degree > 0; --degree) {
    for (int i = 0; i < degree; i++) {
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

  for (BezierPoint const & point: m_points) {
    pointShape.setPosition(point.position());
    window.draw(pointShape);
  }

  for (int i = 0; i + m_degree < m_points.size(); i+= m_degree) {
    for (int j = 0; j < num_points; ++j) {
      lineShape.setPosition(InterpolateFromPoint(increment * j, i));
      window.draw(lineShape);
    }
  }
}

long BezierEngine::AddPoint(sf::Vector2f coords) {
  long idx = m_points.size();
  m_points.emplace_back(coords);
  return idx;
}


void BezierEngine::UpdatePointPosition(long idx, sf::Vector2f pos) {
  m_points[idx].m_position = pos;
}

long BezierEngine::DeleteLastPoint() {
  m_points.pop_back();
  return m_points.size();
}

void BezierEngine::IncrementDegree() {
  m_degree = std::clamp(++m_degree, 1U, 6U);
}
void BezierEngine::DecrementDegree() {
  m_degree = std::clamp(--m_degree, 1U, 6U);
}

std::unique_ptr<InputHandler> BezierEngine::GetInputHandler() {
  return std::make_unique<BezierInput>(this);
}

long BezierEngine::GetHitPointIdx(sf::Vector2f pos) {
  auto point = std::find_if(std::begin(m_points), std::end(m_points), 
    [&pos](auto point) {
      return square_dist(pos, point.position()) < pointRadius*pointRadius;
    });
  if (point == std::end(m_points)) return -1;
  return std::distance(std::begin(m_points), point);
}

BezierPoint& BezierEngine::GetPoint(long index) {
  return m_points[index];
}
