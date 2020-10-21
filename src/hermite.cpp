#include "hermite.hpp"
#include "utils.hpp"
#include <cmath>
#include <iostream>

const float pointRadius = 10.0f;
const float indicatorRadius = 15.0f;
const float lineRadius = 2.0f;
const float normalScaling = 0.1f;
const int num_points = 200;
const float increment = 1.0/num_points;

CubicHermitePoint::CubicHermitePoint(sf::Vector2f position, sf::Vector2f tangent):
  m_position(position), m_tangent(tangent), m_focused{false} { }

CubicHermiteInput::CubicHermiteInput(CubicHermiteEngine* engine) :
  m_currentState{DEFAULT},
  m_engine{engine} {}

void CubicHermiteInput::MouseMoved(sf::Vector2f position) {
  //If point is focused, move it with the mouse
  if (m_currentState == DEFAULT) return;

  if (m_focusedPoint) {
    switch (m_currentState) {
      case MOVING_POINT: 
        {
          m_engine->UpdatePointPosition(*m_focusedPoint, position);
        }
        break;
      case CREATING_POINT:
      case MOVING_TANGENT:
        {
          m_engine->UpdatePointTangent(*m_focusedPoint, 
              (position - m_engine->GetPoint(*m_focusedPoint).position()) / normalScaling);
        }
        break;
      default:
        break;
    }
  }

}

void CubicHermiteInput::ButtonPressed(sf::Mouse::Button button, sf::Vector2f position) {
  if (button != sf::Mouse::Left) return;

  //find pressed points, focus the pressed one
  auto pointIdx = m_engine->GetHitPointIdx(position);

  //switch to new state
  if (pointIdx) {
    if (m_focusedPoint) {
      CubicHermitePoint& focusedPoint = m_engine->GetPoint(*m_focusedPoint);
      sf::Vector2f indicatorPos = (focusedPoint.position() + focusedPoint.tangent() * normalScaling);
      if (square_dist(position, indicatorPos) < pointRadius*pointRadius) {
        m_currentState = MOVING_TANGENT;
        return;
      }
      focusedPoint.setFocused(false);
      m_focusedPoint.reset();
    }

    m_focusedPoint = m_engine->AddPoint(position, {0.0, 0.0});
    m_engine->GetPoint(*m_focusedPoint).setFocused(true);
    m_currentState = CREATING_POINT;
    return;
  } else {
    if (m_focusedPoint) m_engine->GetPoint(*m_focusedPoint).setFocused(false);

    m_focusedPoint = pointIdx;
    m_engine->GetPoint(*m_focusedPoint).setFocused(true);
    m_currentState = MOVING_POINT;
  }
}

void CubicHermiteInput::ButtonReleased(sf::Mouse::Button button, sf::Vector2f position) {
  if (button != sf::Mouse::Left) return;

  m_currentState = DEFAULT;
}

void CubicHermiteInput::KeyPressed(sf::Event::KeyEvent event) {
  if (m_currentState != DEFAULT) return;
  if (event.code == sf::Keyboard::Z) {
    if (m_engine->DeleteLastPoint() == m_focusedPoint) m_focusedPoint.reset();
  }
}

void CubicHermiteInput::KeyReleased(sf::Event::KeyEvent event) {}

CubicHermiteEngine::CubicHermiteEngine() {}

size_t CubicHermiteEngine::AddPoint(sf::Vector2f coords, sf::Vector2f tangent) {
  const long idx = m_points.size();
  m_points.emplace_back(coords, tangent);

  if (idx <= 0) {
    return idx;
  }

  return idx;
}

void CubicHermiteEngine::UpdatePointPosition(long idx, sf::Vector2f pos) {
  m_points[idx].m_position = pos;
}

void CubicHermiteEngine::UpdatePointTangent(long idx, sf::Vector2f tangent) {
  m_points[idx].m_tangent = tangent;
}

void CubicHermiteEngine::RenderLine(sf::RenderWindow & window) const {
  sf::CircleShape pointShape{pointRadius};
  sf::CircleShape indicatorShape{indicatorRadius};
  sf::CircleShape highlightedShape{pointRadius};
  sf::CircleShape lineShape{lineRadius};
  pointShape.setOrigin({pointRadius, pointRadius});
  indicatorShape.setOrigin({indicatorRadius, indicatorRadius});
  indicatorShape.setFillColor(sf::Color(255,0,0));
  highlightedShape.setOrigin({pointRadius, pointRadius});
  highlightedShape.setFillColor(sf::Color{0,255,0});
  lineShape.setOrigin({lineRadius, lineRadius});

  if (m_points.size() > 0) {
    for (size_t i = 0; i < m_points.size() - 1; ++i) {
      for (size_t j = 1; j < num_points; ++j) {
        lineShape.setPosition(cubic_hermite_interpolation(increment * j, m_points[i], m_points[i+1]));
        window.draw(lineShape);
      }
    }
  }
  for (CubicHermitePoint const & point : m_points) {

    if (point.m_focused) {
      indicatorShape.setPosition(( point.position() + point.tangent() * normalScaling ));
      window.draw(indicatorShape);
      highlightedShape.setPosition(point.position());
      window.draw(highlightedShape);
    } else {
      pointShape.setPosition(point.position());
      window.draw(pointShape);
    }


  }
}

size_t CubicHermiteEngine::DeleteLastPoint() {
  m_points.pop_back();
  return m_points.size();
}

std::unique_ptr<InputHandler> CubicHermiteEngine::GetInputHandler() {
  return std::make_unique<CubicHermiteInput>(this);
}

std::optional<size_t> CubicHermiteEngine::GetHitPointIdx(sf::Vector2f pos) {
  auto point = std::find_if(std::begin(m_points), std::end(m_points), 
    [&pos](auto point) {
      return square_dist(pos, point.position()) < pointRadius*pointRadius;
    });
  if (point == std::end(m_points)) return std::nullopt;
  return std::distance(std::begin(m_points), point);
}

CubicHermitePoint& CubicHermiteEngine::GetPoint(long index) {
  return m_points[index];
}

sf::Vector2f cubic_hermite_interpolation(float t, CubicHermitePoint const & p0, CubicHermitePoint const & p1) {
  return 
    (2*t*t*t - 3*t*t + 1)*p0.position()
    + (t*t*t - 2*t*t + t)*p0.tangent()
    + (-2*t*t*t + 3*t*t)*p1.position()
    + (t*t*t - t*t)*p1.tangent();
}
