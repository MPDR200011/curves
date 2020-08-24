#include "bezier.hpp"
#include <cmath>
#include <iostream>

const float pointRadius = 5.0f;
const float midPointRadius = 2.0f;
const float normalScaling = 0.1f;
const int num_points = 200;
const float increment = 1.0/num_points;

float square_dist(sf::Vector2f const & v1, sf::Vector2f const & v2) {
  float p1 = v1.x - v2.x;
  float p2 = v1.y - v2.y;
  return p1*p1 + p2*p2;
}


BezierPoint::BezierPoint(sf::Vector2f position, sf::Vector2f tangent):
  m_position(position), m_tangent(tangent) {
    this->focused = false;
  }

BezierInput::BezierInput(BezierEngine* engine) {
  this->m_currentState = DEFAULT;
  this->m_engine = engine;
  this->m_focusedPoint = -1;
}

void BezierInput::MouseMoved(sf::Vector2f position) {
  //If point is focused, move it with the mouse
  if (m_currentState == DEFAULT) return;

  if (m_focusedPoint != -1) {
    switch (m_currentState) {
      case MOVING_POINT: 
        {
          m_engine->UpdatePointPosition(m_focusedPoint, position);
        }
        break;
      case CREATING_POINT:
      case MOVING_TANGENT:
        {
          m_engine->UpdatePointTangent(m_focusedPoint, 
              (position - m_engine->GetPoint(m_focusedPoint).position()) / normalScaling);
        }
        break;
      default:
        break;
    }
  }

}

void BezierInput::ButtonPressed(sf::Mouse::Button button, sf::Vector2f position) {
  if (button != sf::Mouse::Left) return;

  //find pressed points, focus the pressed one
  long pointIdx = m_engine->GetHitPointIdx(position);

  //switch to new state
  if (pointIdx == -1) {
    if (m_focusedPoint != -1) {
      BezierPoint& focusedPoint = m_engine->GetPoint(m_focusedPoint);
      sf::Vector2f indicatorPos = ( focusedPoint.position() + focusedPoint.tangent() * normalScaling);
      if (square_dist(position, indicatorPos) < pointRadius*pointRadius) {
        m_currentState = MOVING_TANGENT;
        return;
      }
      focusedPoint.setFocused(false);
      m_focusedPoint = -1;
    }

    m_focusedPoint = m_engine->AddPoint(position, {0.0, 0.0});
    m_engine->GetPoint(m_focusedPoint).setFocused(true);
    m_currentState = CREATING_POINT;
    return;
  } else {
    if (m_focusedPoint != -1) m_engine->GetPoint(m_focusedPoint).setFocused(false);

    m_focusedPoint = pointIdx;
    m_engine->GetPoint(m_focusedPoint).setFocused(true);
    m_currentState = MOVING_POINT;
  }
}

void BezierInput::ButtonReleased(sf::Mouse::Button button, sf::Vector2f position) {
  if (button != sf::Mouse::Left) return;

  m_currentState = DEFAULT;
}

BezierEngine::BezierEngine() {}

long BezierEngine::AddPoint(sf::Vector2f coords, sf::Vector2f tangent) {
  const long idx = m_points.size();
  m_points.emplace_back(coords, tangent);

  if (idx <= 0) {
    return idx;
  }

  return idx;
}

void BezierEngine::UpdatePointPosition(long idx, sf::Vector2f pos) {
  m_points[idx].m_position = pos;
}

void BezierEngine::UpdatePointTangent(long idx, sf::Vector2f tangent) {
  m_points[idx].m_tangent = tangent;
}

void BezierEngine::RenderLine(sf::RenderWindow & window) const {
  sf::CircleShape pointShape{pointRadius};
  sf::CircleShape lineShape{midPointRadius};
  pointShape.setOrigin({pointRadius, pointRadius});
  lineShape.setOrigin({midPointRadius, midPointRadius});

  if (m_points.size() > 0) {
    for (size_t i = 0; i < m_points.size() - 1; ++i) {
      for (size_t j = 1; j < num_points; ++j) {
        lineShape.setPosition(bezier_interpolate(increment * j, m_points[i], m_points[i+1]));
        window.draw(lineShape);
      }
    }
  }
  for (BezierPoint const & point : m_points) {

    if (point.focused) {
      pointShape.setPosition(( point.position() + point.tangent() * normalScaling ));
      pointShape.setFillColor(sf::Color(255,0,0));
      window.draw(pointShape);
      pointShape.setFillColor(sf::Color{0,255,0});
      pointShape.setPosition(point.position());
      window.draw(pointShape);
      pointShape.setFillColor(sf::Color{255,255,255});
    } else {
      pointShape.setPosition(point.position());
      window.draw(pointShape);
    }


  }
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

sf::Vector2f bezier_interpolate(float t, BezierPoint const & p0, BezierPoint const & p1) {
  return 
    (2*t*t*t - 3*t*t + 1)*p0.position()
    + (t*t*t - 2*t*t + t)*p0.tangent()
    + (-2*t*t*t + 3*t*t)*p1.position()
    + (t*t*t - t*t)*p1.tangent();
}
