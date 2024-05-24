#include "PathFinding.hpp"

bool ComparePair::operator()(const std::pair<POINT, double>& a, const std::pair<POINT, double>& b) const
{
  return a.second > b.second;
}

AStar::AStar(Level* level) : m_level(level)
{
  ySize = m_level->m_layout.size();
  xSize = m_level->m_layout[0].size();
}

AStar::AStar()
{
  m_level = nullptr;
  ySize = 0;
  xSize = 0;
}

AStar AStar::operator=(const AStar& other)
{
  m_level = other.m_level;
  ySize = other.ySize;
  xSize = other.xSize;
  m_visitedArr = other.m_visitedArr;
  m_cameFromArr = other.m_cameFromArr;
  m_gScoreArr = other.m_gScoreArr;
  m_fScoreArr = other.m_fScoreArr;
  return *this;
}

void AStar::init()
{
  // Reset all data structures
  m_visitedArr.clear();
  m_cameFromArr.clear();
  m_gScoreArr.clear();
  m_fScoreArr.clear();

  // Resize all data structures to the size of the map
  m_visitedArr.resize(ySize, std::vector<bool>(xSize, false));
  m_cameFromArr.resize(ySize, std::vector<POINT>(xSize));
  m_gScoreArr.resize(ySize, std::vector<double>(xSize, std::numeric_limits<double>::infinity()));
  m_fScoreArr.resize(ySize, std::vector<double>(xSize, std::numeric_limits<double>::infinity()));
}

std::vector<POINT> AStar::findPath(POINT start, POINT end)
{
  init();
  std::priority_queue<std::pair<POINT, double>, std::vector<std::pair<POINT, double>>, ComparePair> queue;
  queue.push({ start, 0 });
  m_visitedArr[start.y][start.x] = true;
  m_gScoreArr[start.y][start.x] = 0;
  m_fScoreArr[start.y][start.x] = heuristic(start, end);

  while (!queue.empty())
  {
    POINT current = queue.top().first;
    queue.pop();

    if (current.x == end.x && current.y == end.y)
    {
      return reconstructPath(start, end);
    }

    for (int dx = -1; dx <= 1; dx++)
    {
      for (int dy = -1; dy <= 1; dy++)
      {
        if (dx == 0 && dy == 0) continue;

        int newX = current.x + dx;
        int newY = current.y + dy;

        if (newX >= 0 && newX < xSize && newY >= 0 && newY < ySize)
        {
          if (!m_visitedArr[newY][newX] && m_level->m_layout[newY][newX] == 0)
          {
            double tentative_gScore = m_gScoreArr[current.y][current.x] + 1;
            if (tentative_gScore < m_gScoreArr[newY][newX])
            {
              m_cameFromArr[newY][newX] = current;
              m_gScoreArr[newY][newX] = tentative_gScore;
              m_fScoreArr[newY][newX] = m_gScoreArr[newY][newX] + heuristic(POINT{ newX, newY }, end);
              if (!m_visitedArr[newY][newX])
              {
                queue.push({ POINT{newX, newY}, m_fScoreArr[newY][newX] });
                m_visitedArr[newY][newX] = true;
              }
            }
          }
        }
      }
    }
  }

  return {};  // return empty path if no path found
}

std::vector<POINT> AStar::reconstructPath(POINT start, POINT end)
{
  std::vector<POINT> path;
  for (POINT p = end; p.x != start.x || p.y != start.y; p = m_cameFromArr[p.y][p.x])
  {
    path.push_back(p);
  }
  std::reverse(path.begin(), path.end());
  return path;
}

double AStar::heuristic(POINT a, POINT b)
{
  // Using Euclidean distance as heuristic
  return std::sqrt(std::pow(b.x - a.x, 2) + std::pow(b.y - a.y, 2));
}

