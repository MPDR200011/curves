#pragma once

#include <vector>
#include <SFML/Graphics.hpp>
#include "lines.hpp"

class BezierEngine;

class BezierPoint {
  sf::Vector2f m_position;

  bool focused;

  friend class BezierEngine;

public:
  BezierPoint(sf::Vector2f position);

  sf::Vector2f position() const {
    return m_position;
  }

  void setFocused(bool value) {
    this->focused = value;
  }
};

class BezierEngine;

class BezierInput : public InputHandler {
  enum BezierState {DEFAULT, MOVING_POINT};
  BezierEngine* m_engine;

  long m_focusedPoint;

  BezierState m_currentState;
public:
  BezierInput(BezierEngine* engine);
  ~BezierInput() {};
  void MouseMoved(sf::Vector2f vector);
  void ButtonPressed(sf::Mouse::Button button, sf::Vector2f position);
  void ButtonReleased(sf::Mouse::Button button, sf::Vector2f position);
  void KeyPressed(sf::Event::KeyEvent event);
  void KeyReleased(sf::Event::KeyEvent event);
};

class BezierEngine : public LineEngine {
  std::vector<BezierPoint> m_points;

  unsigned m_degree;

  sf::Vector2f InterpolateFromPoint(float t, size_t pointIdx) const;

public:
  BezierEngine(unsigned degree = 2);
  ~BezierEngine() {};
  void RenderLine(sf::RenderWindow&) const;

  long AddPoint(sf::Vector2f coords);
  void UpdatePointPosition(long idx, sf::Vector2f pos);
  long DeleteLastPoint();

  void IncrementDegree();
  void DecrementDegree();

  std::unique_ptr<InputHandler> GetInputHandler();

  long GetHitPointIdx(sf::Vector2f pos);
  BezierPoint& GetPoint(long index);
};
