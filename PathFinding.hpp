#pragma once

#include "Level.h"
#include <cmath>
#include <queue>
#include <vector>
#include <windef.h>

struct ComparePair
{
  bool operator()(const std::pair<POINT, double>& a, const std::pair<POINT, double>& b) const;
};


class AStar
{
public:
  AStar(Level* level);
  AStar();
  // copy operator
  AStar operator=(const AStar& other);

  std::vector<POINT> findPath(POINT start, POINT end);

private:
  Level* m_level;
  std::size_t ySize;
  std::size_t xSize;
  std::vector<std::vector<bool>> m_visitedArr{ 0, std::vector<bool>{0, false} };
  std::vector<std::vector<POINT>> m_cameFromArr{ 0, std::vector<POINT>{0, POINT{0, 0}} };
  std::vector<std::vector<double>> m_gScoreArr{ 0, std::vector<double>{0, std::numeric_limits<double>::infinity()} };
  std::vector<std::vector<double>> m_fScoreArr{ 0, std::vector<double>{0, std::numeric_limits<double>::infinity()} };

  void init();
  std::vector<POINT> reconstructPath(POINT start, POINT end);
  double heuristic(POINT a, POINT b);
};
