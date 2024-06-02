//-----------------------------------------------------------------
// Actor Object
// C++ Source - Actor.cpp
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "Actor.h"
#include "resource.h"
#include <chrono>

GameEngine* Actor::_pGame = nullptr;

Actor::Actor(Bitmap* bmpBitmap, Level* pLevel) : Sprite(bmpBitmap)
{
  m_pLevel = pLevel;
}

//-----------------------------------------------------------------
// Actor General Methods
//-----------------------------------------------------------------
void Actor::LinkBitmapToState(int iState, Bitmap* bmpBitmap)
{
  m_pSpriteStates[iState] = bmpBitmap;
}

SPRITEACTION Actor::Update()
{
  // Because we are using integer instead of float, we should add a buffer to the collision detection
  constexpr int BUFFER{ 6 };

  if (m_ptVelocity.x > 0)
  {
    // We are moving right, do we collide with the right wall?
    POINT nextPosition{ m_rcPosition.right + m_ptVelocity.x + BUFFER, (m_rcPosition.top + m_rcPosition.bottom) / 2 };
    if (m_pLevel->m_layout[nextPosition.y / 32][nextPosition.x / 32] != 0)
    {
      // Yes, we do collide, zero out the x velocity
      m_ptVelocity.x = 0;
    }
  }
  else if (m_ptVelocity.x < 0)
  {
    // We are moving left, do we collide with the left wall?
    POINT nextPosition{ m_rcPosition.left + m_ptVelocity.x - BUFFER, (m_rcPosition.top + m_rcPosition.bottom) / 2 };
    if (m_pLevel->m_layout[nextPosition.y / 32][nextPosition.x / 32] != 0)
    {
      // Yes, we do collide, zero out the x velocity
      m_ptVelocity.x = 0;
    }
  }

  if (m_ptVelocity.y > 0)
  {
    // We are moving down, do we collide with the bottom wall?
    POINT nextPosition{ (m_rcPosition.left + m_rcPosition.right) / 2, m_rcPosition.bottom + m_ptVelocity.y + BUFFER};
    if (m_pLevel->m_layout[nextPosition.y / 32][nextPosition.x / 32] != 0)
    {
      // Yes, we do collide, zero out the y velocity
      m_ptVelocity.y = 0;
    }
  }
  else if (m_ptVelocity.y < 0)
  {
    // We are moving up, do we collide with the top wall?
    POINT nextPosition{ (m_rcPosition.left + m_rcPosition.right) / 2, m_rcPosition.top + m_ptVelocity.y - BUFFER };
    if (m_pLevel->m_layout[nextPosition.y / 32][nextPosition.x / 32] != 0)
    {
      // Yes, we do collide, zero out the y velocity
      m_ptVelocity.y = 0;
    }
  }

  SPRITEACTION out = Sprite::Update();

  return out;
}

bool Actor::AmIStuck()
{
  BOOL bTopLeft = m_pLevel->IsPointCollidable(POINT{ m_rcPosition.left, m_rcPosition.top });
  BOOL bTopRight = m_pLevel->IsPointCollidable(POINT{ m_rcPosition.right, m_rcPosition.top });
  BOOL bBottomLeft = m_pLevel->IsPointCollidable(POINT{ m_rcPosition.left, m_rcPosition.bottom });
  BOOL bBottomRight = m_pLevel->IsPointCollidable(POINT{ m_rcPosition.right, m_rcPosition.bottom });

  if (!bTopLeft || !bTopRight || !bBottomLeft || !bBottomRight)
  {
    return true;
  }
  return false;
}

//-----------------------------------------------------------------
// Player Constructor(s)/Destructor
//-----------------------------------------------------------------
Player::Player(Bitmap* bmpBitmap, Level* pLevel) : Actor(bmpBitmap, pLevel)
{
  m_ptTargetVelocity = POINT{ 0,0 };
  m_iState = 0;
  m_iSize = 24;
  m_iMaxHealth = 100;
  m_iCurrentHealth = m_iMaxHealth;
  m_iInvFrames = 0;

  m_pSpriteStates[0] = bmpBitmap;
}

//-----------------------------------------------------------------
// Player General Methods
//-----------------------------------------------------------------
void Player::UpdateVelocity()
{
  if (m_ptTargetVelocity.x < m_ptVelocity.x)
    m_ptVelocity.x = max(m_ptTargetVelocity.x, m_ptVelocity.x - 1);
  else if (m_ptTargetVelocity.x > m_ptVelocity.x)
    m_ptVelocity.x = min(m_ptTargetVelocity.x, m_ptVelocity.x + 1);

  if (m_ptTargetVelocity.y < m_ptVelocity.y)
    m_ptVelocity.y = max(m_ptTargetVelocity.y, m_ptVelocity.y - 1);
  else if (m_ptTargetVelocity.y > m_ptVelocity.y)
    m_ptVelocity.y = min(m_ptTargetVelocity.y, m_ptVelocity.y + 1);
}

void Player::SubtractHealth(int value)
{
  if (m_iCurrentHealth > 0 && m_iInvFrames <= 0)
  {
    m_iInvFrames = 10;
    m_iCurrentHealth = m_iCurrentHealth - value > 0 ? m_iCurrentHealth - value : 0;
  }
}

SPRITEACTION Player::Update()
{
  UpdateVelocity();
  SPRITEACTION out = Actor::Update();
  POINT ptPlayerCenterPos = GetPositionFromCenter();

  POINT ptMouseOffset = POINT{ m_ptMousePos.x - ptPlayerCenterPos.x, m_ptMousePos.y - ptPlayerCenterPos.y };

  if (ptMouseOffset.y >= ptMouseOffset.x && ptMouseOffset.y >= -ptMouseOffset.x)
  {
    SetState(PLR_DOWN);
    //DOWN
  }
  if (ptMouseOffset.y >= ptMouseOffset.x && ptMouseOffset.y <= -ptMouseOffset.x)
  {
    SetState(PLR_LEFT);
    //LEFT
  }
  if (ptMouseOffset.y <= ptMouseOffset.x && ptMouseOffset.y >= -ptMouseOffset.x)
  {
    SetState(PLR_RIGHT);
    //RIGHT
  }
  if (ptMouseOffset.y <= ptMouseOffset.x && ptMouseOffset.y <= -ptMouseOffset.x)
  {
    SetState(PLR_UP);
    //UP
  }

  m_iInvFrames--;

  return out;
}

//-----------------------------------------------------------------
// Swing Constructor(s)/Destructor
//-----------------------------------------------------------------
Swing::Swing(Bitmap* bmpBitmap, Level* pLevel, POINT ptDirection) : Actor(bmpBitmap, pLevel)
{
  m_iActiveTime = 2;
  m_ptDirection = ptDirection;
}

//-----------------------------------------------------------------
// Swing General Methods
//-----------------------------------------------------------------

Bitmap* Enemy::m_bmpBullet = nullptr;

SPRITEACTION Swing::Update()
{
    SPRITEACTION out = Sprite::Update();

	if (m_iActiveTime-- <= 0)
	{
		return SA_KILL;
	}
	return out;
}

//-----------------------------------------------------------------
// Enemy Constructor(s)/Destructor
//-----------------------------------------------------------------
Enemy::Enemy(Bitmap* bmpBitmap, Level* pLevel, EnemyType type, Player* pTarget)
  : Actor(bmpBitmap, pLevel), m_type(type), m_pTarget(pTarget), m_speed(0)
  , m_state(EnemyState::PATROLLING)
  , m_lastPosition(GetPositionFromCenter())
  , m_lastPositionUpdateTime(GetCurrentTimeMillis())
{
  m_aStar = AStar(pLevel);
  m_ptTargetVelocity = POINT{ 0,0 };
  m_iState = 0;
  m_iSize = 24;
  m_pSpriteStates[0] = bmpBitmap;
  m_destination = FindNextDestination();
  switch (type)
  {
    case EnemyType::ANGRY_GUY:
    {
        m_iAbilityTimer = 30;
      m_speed = 3;
      break;
    }
    case EnemyType::DUTY_GUY:
    {
      m_enemySize = 24;
      m_speed = 2;
      break;
    }
    case EnemyType::HEAVY_GUY:
    {
      m_enemySize = 32;
      m_speed = 1;
      break;
    }
    case EnemyType::COWARD_GUY:
    {
      m_enemySize = 20;
      m_speed = 4;
      break;
    }
  }
}

//-----------------------------------------------------------------
// Enemy General Methods
//-----------------------------------------------------------------
/*
void Enemy::ChangeBitmap()
{
  switch (m_type)
  {
    case EnemyType::ANGRY_GUY:
    {
      GetBitmap()->Create(m_enemySize, m_enemySize, RGB(255, 75, 75));
      break;
    }
    case EnemyType::DUTY_GUY:
    {
      GetBitmap()->Create(m_enemySize, m_enemySize, RGB(255, 75, 75));
      break;
    }
    case EnemyType::HEAVY_GUY:
    {
      GetBitmap()->Create(m_enemySize, m_enemySize, RGB(75, 255, 75));
      break;
    }
    case EnemyType::COWARD_GUY:
    {
      GetBitmap()->Create(m_enemySize, m_enemySize, RGB(75, 255, 75));
      break;
    }
  }
}
*/

void Enemy::HandleStuck()
{
  if (GetCurrentTimeMillis() < m_lastPositionUpdateTime + 500)
  {
    return;
  }

  POINT currentPosition = GetPositionFromCenter();

  if (m_lastPosition.x == currentPosition.x
      && m_lastPosition.y == currentPosition.y)
  {
    m_destination = FindNextDestination();
    m_state = EnemyState::PATROLLING;
  }
}

void Enemy::UpdateState()
{
    if (m_type == EnemyType::ANGRY_GUY) {
        if (m_ptVelocity.x > 0) {
            SetState(AngryState::ANGRYRIGHT);
        }
        else {
            SetState(AngryState::ANGRYLEFT);
        }
    }
    else if (m_type == EnemyType::COWARD_GUY) {
        if (m_ptVelocity.x > 0) {
            if (m_ptVelocity.y > 0) {
                SetState(CowardState::COWARDRIGHT);
            }
            else {
                SetState(CowardState::COWARDBACKRIGHT);
            }
        }
        else {
            if (m_ptVelocity.y > 0) {
                SetState(CowardState::COWARDLEFT);
            }
            else {
                SetState(CowardState::COWARDBACKLEFT);
            }
        }
    }
    

    

  if (m_state == EnemyState::PATROLLING)
  {
    // Check distance to target
    POINT playerPos = m_pTarget->GetPositionFromCenter();
    POINT enemyPos = GetPositionFromCenter();
    int distance = sqrt(pow(playerPos.x - enemyPos.x, 2) + pow(playerPos.y - enemyPos.y, 2));
    if (distance <= 128)
    {
      if (m_type == EnemyType::COWARD_GUY)
      {
        m_state = EnemyState::ESCAPING;
      }
      else
      {
        m_state = EnemyState::ATTACKING;
      }
    }
  }
  else if (m_state == EnemyState::ATTACKING)
  {
    if (m_type != EnemyType::ANGRY_GUY)
    {
      // Check distance to target
      POINT playerPos = m_pTarget->GetPositionFromCenter();
      POINT enemyPos = GetPositionFromCenter();
      int distance = sqrt(pow(playerPos.x - enemyPos.x, 2) + pow(playerPos.y - enemyPos.y, 2));
      if (distance > 128)
      {
        // Set last known position as target
        m_destination = m_pLevel->GetNodeFromPosition(playerPos);
        m_state = EnemyState::PATROLLING;
      }
    }
  }
  else if (m_state == EnemyState::ESCAPING)
  {
    if (m_type == EnemyType::COWARD_GUY)
    {
      // Check distance to target
      POINT playerPos = m_pTarget->GetPositionFromCenter();
      POINT enemyPos = GetPositionFromCenter();
      int distance = sqrt(pow(playerPos.x - enemyPos.x, 2) + pow(playerPos.y - enemyPos.y, 2));
      if (distance > 128)
      {
        // Set last known position as target
        m_destination = m_pLevel->GetNodeFromPosition(playerPos);
        m_state = EnemyState::PATROLLING;
      }
    }
  }
}

void Enemy::UpdateVelocity()
{
  if (m_ptTargetVelocity.x < m_ptVelocity.x) m_ptVelocity.x = max(m_ptTargetVelocity.x, m_ptVelocity.x - 1);
  else if (m_ptTargetVelocity.x > m_ptVelocity.x) m_ptVelocity.x = min(m_ptTargetVelocity.x, m_ptVelocity.x + 1);

  if (m_ptTargetVelocity.y < m_ptVelocity.y) m_ptVelocity.y = max(m_ptTargetVelocity.y, m_ptVelocity.y - 1);
  else if (m_ptTargetVelocity.y > m_ptVelocity.y) m_ptVelocity.y = min(m_ptTargetVelocity.y, m_ptVelocity.y + 1);
}

POINT Enemy::FindNextDestination()
{
  POINT dest{ 0,0 };
  bool impossiblePosition{ true };
  while (impossiblePosition)
  {
    dest = POINT{
            static_cast<long>((size_t)rand() % m_pLevel->m_layout[0].size()),
            static_cast<long>((size_t)rand() % m_pLevel->m_layout.size())
    };

    // Check if the destination is a wall
    impossiblePosition = m_pLevel->m_layout[dest.y][dest.x] != 0;
  }
  return dest;
}

void Enemy::Move()
{
  HandleStuck();

  // Which node am I on?
  POINT startNode = m_pLevel->GetNodeFromPosition(GetPositionFromCenter());

  // Which node is target on?
  POINT endNode{ 0,0 };
  if (m_state == EnemyState::PATROLLING)
    endNode = m_destination;
  else if (m_state == EnemyState::ATTACKING)
    endNode = m_pLevel->GetNodeFromPosition(m_pTarget->GetPositionFromCenter());
  else if (m_state == EnemyState::ESCAPING)
    endNode = m_pLevel->GetNodeFromPosition(m_pTarget->GetPositionFromCenter());

  // Find path
  std::vector<POINT> path = m_aStar.findPath(startNode, endNode);

  if (path.empty())
  {
    if (m_state == EnemyState::PATROLLING)
    {
      m_destination = FindNextDestination();
    }
    return;
  }

  POINT nextNode = path[0];

  // Calculate x, y distances to the end node from start node
  int xDistance = nextNode.x - startNode.x;
  int yDistance = nextNode.y - startNode.y;

  // Give the m_speed to bigger component and adjust the other component accordingly
  if (abs(xDistance) > abs(yDistance))
  {
    m_ptTargetVelocity.x = m_speed * (xDistance / abs(xDistance));
    m_ptTargetVelocity.y = m_speed * (yDistance / abs(xDistance));
  }
  else
  {
    m_ptTargetVelocity.x = m_speed * (xDistance / abs(yDistance));
    m_ptTargetVelocity.y = m_speed * (yDistance / abs(yDistance));
  }

  // Reverse if enemy escaping
  if (m_state == EnemyState::ESCAPING)
  {
    m_ptTargetVelocity.x = -m_ptTargetVelocity.x;
    m_ptTargetVelocity.y = -m_ptTargetVelocity.y;
  }
}

SPRITEACTION Enemy::Update()
{
  UpdateState();
  Move();
  UpdateVelocity();
  SPRITEACTION out = Actor::Update();

  if (m_pTarget != NULL)
  {
    if (TestCollision(m_pTarget))
    {
      m_pTarget->SubtractHealth(10);
    }
  }

  // Update last position
  POINT currentPosition = GetPositionFromCenter();

  if (m_lastPosition.x != currentPosition.x || m_lastPosition.y != currentPosition.y)
  {
    m_lastPosition = currentPosition;
    m_lastPositionUpdateTime = GetCurrentTimeMillis();
  }

  if (--m_iAbilityTimer <= 0) {
      if (m_type == EnemyType::ANGRY_GUY) {
          if (m_state == EnemyState::ATTACKING) {
              Flame* pFlame = new Flame(Flame::m_pFlameBitmap, m_pLevel);
              pFlame->SetNumFrames(3);
              pFlame->SetPositionFromCenter(GetPositionFromCenter());
              pFlame->SetTime(40);
              _pGame->AddSprite(pFlame);
              m_iAbilityTimer = 15;
          }
          else {
              m_iAbilityTimer = 0;
          }
      }
      if (m_type == EnemyType::COWARD_GUY) {
          if (m_state == EnemyState::PATROLLING) {
              m_iAbilityTimer = 0;
          }
          else {
              Fireball* pBullet = new Fireball(Enemy::m_bmpBullet, m_pLevel);
              POINT ptTargetPos = m_pTarget->GetPositionFromCenter();
              POINT ptEnemyPos = GetPositionFromCenter();
              POINT ptVelocity = POINT{ ptTargetPos.x - ptEnemyPos.x, ptTargetPos.y - ptEnemyPos.y};

              float multiplier = 10 / sqrt((ptVelocity.x * ptVelocity.x) + (ptVelocity.y * ptVelocity.y));
              pBullet->setEnemy(true);
              pBullet->SetPositionFromCenter(ptEnemyPos);
              pBullet->SetVelocity(ptVelocity.x * multiplier, ptVelocity.y * multiplier);
              pBullet->SetNumFrames(2);
              pBullet->SetFrameDelay(1);

              _pGame->AddSprite(pBullet);

              m_iAbilityTimer = 20;
          }
      }
      if (m_type == EnemyType::DUTY_GUY) {
          if (m_state == EnemyState::ATTACKING) {
              Enemy* enemy = enemy = new Enemy(
                  m_pBitmap, m_pLevel, EnemyType::DUTY_GUY, m_pTarget
              );
              enemy->SetZOrder(7);
              enemy->SetNumFrames(4);
              enemy->SetFrameDelay(10);
              enemy->SetPositionFromCenter(GetPositionFromCenter().x + (rand() % 50) - 25, GetPositionFromCenter().y + (rand() % 50) - 25);
              enemy->SetAbilityTimer(50);
              _pGame->AddSprite(enemy);

              m_iAbilityTimer = 50;
          }
          else {
              m_iAbilityTimer = 0;
          }
      }
  }

  return out;
}

long long Enemy::GetCurrentTimeMillis()
{
  auto now = std::chrono::steady_clock::now();
  auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
  auto epoch = now_ms.time_since_epoch();
  auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(epoch);
  return milliseconds.count();
}

//-----------------------------------------------------------------
// Rock Constructor(s)/Destructor
//-----------------------------------------------------------------
Rock::Rock(Bitmap* _bmpBitmap, Level* pLevel) : Actor(_bmpBitmap, pLevel)
{
  m_iNumHits = 0;
  m_iMaxHits = 3;
}


//-----------------------------------------------------------------
// Rock General Methods
//-----------------------------------------------------------------
void Rock::UpdateVelocity()
{
  if (m_ptVelocity.x == 0 && m_ptVelocity.y == 0) return;

  float multiplier = 2 / sqrt((m_ptVelocity.x * m_ptVelocity.x) + (m_ptVelocity.y * m_ptVelocity.y));

  int decrease_x = (m_ptVelocity.x * multiplier);
  int decrease_y = (m_ptVelocity.y * multiplier);

  if (0 < m_ptVelocity.x) m_ptVelocity.x = max(0, m_ptVelocity.x - (m_ptVelocity.x * multiplier));
  else if (0 > m_ptVelocity.x) m_ptVelocity.x = min(0, m_ptVelocity.x - (m_ptVelocity.x * multiplier));

  if (0 < m_ptVelocity.y) m_ptVelocity.y = max(0, m_ptVelocity.y - (m_ptVelocity.y * multiplier));
  else if (0 > m_ptVelocity.y) m_ptVelocity.y = min(0, m_ptVelocity.y - (m_ptVelocity.y * multiplier));
}

SPRITEACTION Rock::Update()
{
  SPRITEACTION out = Sprite::Update();
  UpdateVelocity();

  if (m_iCooldown > 0) m_iCooldown--;

  BOOL bTopLeft = m_pLevel->IsPointCollidable(POINT{ m_rcPosition.left, m_rcPosition.top });
  BOOL bTopRight = m_pLevel->IsPointCollidable(POINT{ m_rcPosition.right, m_rcPosition.top });
  BOOL bBottomLeft = m_pLevel->IsPointCollidable(POINT{ m_rcPosition.left, m_rcPosition.bottom });
  BOOL bBottomRight = m_pLevel->IsPointCollidable(POINT{ m_rcPosition.right, m_rcPosition.bottom });

  if ((!bTopLeft && !bTopRight) || (!bBottomLeft && !bBottomRight))
  {
    m_ptVelocity.y = -m_ptVelocity.y;
    POINT position = GetPositionFromCenter();
    position.y += m_ptVelocity.y;
    SetPositionFromCenter(position);
  }
  if ((!bTopLeft && !bBottomLeft) || (!bTopRight && !bBottomRight))
  {
    m_ptVelocity.x = -m_ptVelocity.x;
    POINT position = GetPositionFromCenter();
    position.x += m_ptVelocity.x;
    SetPositionFromCenter(position);
  }

	return out;
}

//-----------------------------------------------------------------
// Fireball Constructor(s)/Destructor
//-----------------------------------------------------------------
Fireball::Fireball(Bitmap* _bmpBitmap, Level* pLevel) : Actor(_bmpBitmap, pLevel) {
    m_bEnemy = 0;
    m_bParried = 0;
}

Bitmap* Flame::m_pFlameBitmap = nullptr;

//-----------------------------------------------------------------
// Fireball General Methods
//-----------------------------------------------------------------
SPRITEACTION Fireball::Update() {

	SPRITEACTION out = Sprite::Update();

    if ((!m_bEnemy && Fireball::AmIStuck())||(m_bEnemy && Actor::AmIStuck())) {

        if (!m_bEnemy) {
            Flame* pFlame = new Flame(Flame::m_pFlameBitmap, m_pLevel);
            pFlame->SetNumFrames(3);
            pFlame->SetPositionFromCenter(GetPositionFromCenter().x - GetVelocity().x, GetPositionFromCenter().y - GetVelocity().y);
            pFlame->SetVelocity((-GetVelocity().x / 3) + (rand() % 10) - 5, (-GetVelocity().y / 3) + (rand() % 10) - 5);
            _pGame->AddSprite(pFlame);
            pFlame = new Flame(Flame::m_pFlameBitmap, m_pLevel);
            pFlame->SetNumFrames(3);
            pFlame->SetPositionFromCenter(GetPositionFromCenter().x - GetVelocity().x, GetPositionFromCenter().y - GetVelocity().y);
            pFlame->SetVelocity((-GetVelocity().x / 3) + (rand() % 10) - 5, (-GetVelocity().y / 3) + (rand() % 10) - 5);
            _pGame->AddSprite(pFlame);
        }
        return SA_KILL;
    }

	return out;
}

bool Fireball::AmIStuck()
{
    BOOL bTopLeft = m_pLevel->IsPointCollidable(POINT{ m_rcPosition.left, m_rcPosition.top }) ||
        m_pLevel->IsPointMeltable(POINT{ m_rcPosition.left, m_rcPosition.top });

    BOOL bTopRight = m_pLevel->IsPointCollidable(POINT{ m_rcPosition.right, m_rcPosition.top }) ||
        m_pLevel->IsPointMeltable(POINT{ m_rcPosition.right, m_rcPosition.top });

    BOOL bBottomLeft = m_pLevel->IsPointCollidable(POINT{ m_rcPosition.left, m_rcPosition.bottom }) ||
        m_pLevel->IsPointMeltable(POINT{ m_rcPosition.left, m_rcPosition.bottom });

    BOOL bBottomRight = m_pLevel->IsPointCollidable(POINT{ m_rcPosition.right, m_rcPosition.bottom }) ||
        m_pLevel->IsPointMeltable(POINT{ m_rcPosition.right, m_rcPosition.bottom });


    return (!bTopLeft || !bTopRight || !bBottomLeft || !bBottomRight);
}


//-----------------------------------------------------------------
// Flame Constructor(s)/Destructor
//-----------------------------------------------------------------
Flame::Flame(Bitmap* _bmpBitmap, Level* pLevel) : Actor(_bmpBitmap, pLevel) {
	m_iTime = (rand() % 40) + 80;
	m_bCloned = false;
	m_iCloneAgain = 0;
	m_iCloneDepth = 0;
}

//-----------------------------------------------------------------
// Flame General Methods
//-----------------------------------------------------------------
SPRITEACTION Flame::Update() {

	SPRITEACTION out = Actor::Update();
	UpdateVelocity();

	if (--m_iTime < 0) {
		return SA_KILL;
	}

	if (m_bCloned) {
		if (m_ptVelocity.x == 0 && m_ptVelocity.y == 0) {
			m_iCloneAgain++;
			if (m_iCloneAgain > 2) {
				m_bCloned = false;
				m_iCloneAgain = 0;
			}
		}
		else {
			m_iCloneAgain = 0;
		}
	}

	return out;
}

void Flame::UpdateVelocity()
{
	if (m_ptVelocity.x == 0 && m_ptVelocity.y == 0) return;

	float multiplier = 2 / sqrt((m_ptVelocity.x * m_ptVelocity.x) + (m_ptVelocity.y * m_ptVelocity.y));

	int decrease_x = (m_ptVelocity.x * multiplier);
	int decrease_y = (m_ptVelocity.y * multiplier);

	if (0 < m_ptVelocity.x) m_ptVelocity.x = max(0, m_ptVelocity.x - (m_ptVelocity.x * multiplier));
	else if (0 > m_ptVelocity.x) m_ptVelocity.x = min(0, m_ptVelocity.x - (m_ptVelocity.x * multiplier));

	if (0 < m_ptVelocity.y) m_ptVelocity.y = max(0, m_ptVelocity.y - (m_ptVelocity.y * multiplier));
	else if (0 > m_ptVelocity.y) m_ptVelocity.y = min(0, m_ptVelocity.y - (m_ptVelocity.y * multiplier));
}

//-----------------------------------------------------------------
// Puddle Constructor(s)/Destructor
//-----------------------------------------------------------------
Puddle::Puddle(Bitmap* _bmpBitmap, Level* pLevel) : Actor(_bmpBitmap, pLevel) {
	m_iTime = 100;
}

//-----------------------------------------------------------------
// Puddle General Methods
//-----------------------------------------------------------------
SPRITEACTION Puddle::Update() {

	SPRITEACTION out = Sprite::Update();

	if (--m_iTime < 0) {
		return SA_KILL;
	}

	return out;
}

//-----------------------------------------------------------------
// Mud Constructor(s)/Destructor
//-----------------------------------------------------------------
Mud::Mud(Bitmap* _bmpBitmap, Level* pLevel) : Actor(_bmpBitmap, pLevel) {
	m_iTime = 200;
	m_iSpreadCooldown = 3;
}

//-----------------------------------------------------------------
// Mud General Methods
//-----------------------------------------------------------------
SPRITEACTION Mud::Update() {

	SPRITEACTION out = Sprite::Update();

	if (--m_iTime < 0) {
		return SA_KILL;
	}

	if (m_iSpreadCooldown > 0) {
		m_iSpreadCooldown--;
	}

	return out;
}


//-----------------------------------------------------------------
// Gust Constructor(s)/Destructor
//-----------------------------------------------------------------
Gust::Gust(Bitmap* _bmpBitmap, Level* pLevel) : Actor(_bmpBitmap, pLevel) {
	m_iTime = 10;
}


//-----------------------------------------------------------------
// Gust General Methods
//-----------------------------------------------------------------
SPRITEACTION Gust::Update() {

	SPRITEACTION out = Sprite::Update();

	if (!m_pLevel->IsPointCollidable(GetPositionFromCenter())) return SA_KILL;

	if (--m_iTime < 0) {
		return SA_KILL;
	}

	return out;
}


//-----------------------------------------------------------------
// Ice Constructor(s)/Destructor
//-----------------------------------------------------------------
Ice::Ice(Bitmap* _bmpBitmap, Level* pLevel) : Actor(_bmpBitmap, pLevel) {
    m_iTime = 300;
}

Ice::~Ice() {
    m_pLevel->SetTile(m_pLevel->GetNodeFromPosition(GetPositionFromCenter()), 0);
}


//-----------------------------------------------------------------
// Ice General Methods
//-----------------------------------------------------------------
SPRITEACTION Ice::Update() {

    SPRITEACTION out = Sprite::Update();

    if (--m_iTime < 0) {
        return SA_KILL;
    }

    return out;
}

void Ice::SetPositionFromCenter(POINT ptPosition)
{
    Sprite::SetPositionFromCenter(ptPosition);
    m_pLevel->SetTile(m_pLevel->GetNodeFromPosition(ptPosition), 2);
}

