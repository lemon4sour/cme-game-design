//-----------------------------------------------------------------
// Actor Object
// C++ Source - Actor.cpp
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "Actor.h"

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
  constexpr int BUFFER{ 24 };

  if (m_ptVelocity.x > 0)
  {
    // We are moving right, do we collide with the right wall?
    POINT nextPosition{ m_rcPosition.right + m_ptVelocity.x + BUFFER, (m_rcPosition.top + m_rcPosition.bottom) / 2 };
    if (m_pLevel->m_layout[nextPosition.y / 128][nextPosition.x / 128] != 0)
    {
      // Yes, we do collide, zero out the x velocity
      m_ptVelocity.x = 0;
    }
  }
  else if (m_ptVelocity.x < 0)
  {
    // We are moving left, do we collide with the left wall?
    POINT nextPosition{ m_rcPosition.left + m_ptVelocity.x - BUFFER, (m_rcPosition.top + m_rcPosition.bottom) / 2 };
    if (m_pLevel->m_layout[nextPosition.y / 128][nextPosition.x / 128] != 0)
    {
      // Yes, we do collide, zero out the x velocity
      m_ptVelocity.x = 0;
    }
  }

  if (m_ptVelocity.y > 0)
  {
    // We are moving down, do we collide with the bottom wall?
    POINT nextPosition{ (m_rcPosition.left + m_rcPosition.right) / 2, m_rcPosition.bottom + m_ptVelocity.y + BUFFER};
    if (m_pLevel->m_layout[nextPosition.y / 128][nextPosition.x / 128] != 0)
    {
      // Yes, we do collide, zero out the y velocity
      m_ptVelocity.y = 0;
    }
  }
  else if (m_ptVelocity.y < 0)
  {
    // We are moving up, do we collide with the top wall?
    POINT nextPosition{ (m_rcPosition.left + m_rcPosition.right) / 2, m_rcPosition.top + m_ptVelocity.y - BUFFER };
    if (m_pLevel->m_layout[nextPosition.y / 128][nextPosition.x / 128] != 0)
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

  m_pSpriteStates[0] = bmpBitmap;
}

//-----------------------------------------------------------------
// Player General Methods
//-----------------------------------------------------------------
void Player::UpdateVelocity()
{
  if (m_ptTargetVelocity.x < m_ptVelocity.x)
    m_ptVelocity.x = max(m_ptTargetVelocity.x, m_ptVelocity.x - 5);
  else if (m_ptTargetVelocity.x > m_ptVelocity.x)
    m_ptVelocity.x = min(m_ptTargetVelocity.x, m_ptVelocity.x + 5);

  if (m_ptTargetVelocity.y < m_ptVelocity.y)
    m_ptVelocity.y = max(m_ptTargetVelocity.y, m_ptVelocity.y - 5);
  else if (m_ptTargetVelocity.y > m_ptVelocity.y)
    m_ptVelocity.y = min(m_ptTargetVelocity.y, m_ptVelocity.y + 5);
}

void Player::SubtractHealth(int value)
{
  if (m_iCurrentHealth > 0)
  {
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

  return out;
}

//-----------------------------------------------------------------
// Swing Constructor(s)/Destructor
//-----------------------------------------------------------------
Swing::Swing(Bitmap* bmpBitmap, Level* pLevel, POINT ptDirection) : Actor(bmpBitmap, pLevel)
{
  m_iActiveTime = 3;
  m_ptDirection = ptDirection;
}

//-----------------------------------------------------------------
// Swing General Methods
//-----------------------------------------------------------------
SPRITEACTION Swing::Update()
{
	if (m_iActiveTime-- <= 0)
	{
		return SA_KILL;
	}
	return SA_NONE;
}

//-----------------------------------------------------------------
// Enemy Constructor(s)/Destructor
//-----------------------------------------------------------------
Enemy::Enemy(Bitmap* bmpBitmap, Level* pLevel, EnemyType type, Player* pTarget)
  : Actor(bmpBitmap, pLevel), m_type(type), m_pTarget(pTarget)
{
  m_aStar = AStar(pLevel);
  m_ptTargetVelocity = POINT{ 0,0 };
  m_iState = 0;
  m_iSize = 24;
  m_pSpriteStates[0] = bmpBitmap;
  switch (type)
  {
    case EnemyType::FIRE:
      m_speed = 10;
      break;
    case EnemyType::WATER:
      m_speed = 7;
      break;
    case EnemyType::EARTH:
      m_speed = 2;
      break;
    case EnemyType::AIR:
      m_speed = 11;
      break;
  }
}

//-----------------------------------------------------------------
// Enemy General Methods
//-----------------------------------------------------------------
void Enemy::UpdateVelocity()
{
  if (m_ptTargetVelocity.x < m_ptVelocity.x) m_ptVelocity.x = max(m_ptTargetVelocity.x, m_ptVelocity.x - 5);
  else if (m_ptTargetVelocity.x > m_ptVelocity.x) m_ptVelocity.x = min(m_ptTargetVelocity.x, m_ptVelocity.x + 5);

  if (m_ptTargetVelocity.y < m_ptVelocity.y) m_ptVelocity.y = max(m_ptTargetVelocity.y, m_ptVelocity.y - 5);
  else if (m_ptTargetVelocity.y > m_ptVelocity.y) m_ptVelocity.y = min(m_ptTargetVelocity.y, m_ptVelocity.y + 5);
}

void Enemy::Catch()
{
  if (m_pTarget == NULL) return;

  // Which node am I on?
  POINT startNode = m_pLevel->GetNodeFromPosition(GetPositionFromCenter());

  // Which node is target on?
  POINT endNode = m_pLevel->GetNodeFromPosition(m_pTarget->GetPositionFromCenter());

  // Find path
  std::vector<POINT> path = m_aStar.findPath(startNode, endNode);

  if (path.size() == 0) return;

  POINT nextNode = path[0];

  if (nextNode.x < startNode.x)
  {
    m_ptTargetVelocity.x = -m_speed;
  }
  else if (nextNode.x > startNode.x)
  {
    m_ptTargetVelocity.x = m_speed;
  }

  if (nextNode.y < startNode.y)
  {
    m_ptTargetVelocity.y = -m_speed;
  }
  else if (nextNode.y > startNode.y)
  {
    m_ptTargetVelocity.y = m_speed;
  }
}

SPRITEACTION Enemy::Update()
{
  Catch();
  UpdateVelocity();
  SPRITEACTION out = Actor::Update();
  if (m_pTarget != NULL)
  {
    if (TestCollision(m_pTarget))
    {
      m_pTarget->SubtractHealth(1);
    }
  }
  return out;
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

  float multiplier = 8 / sqrt((m_ptVelocity.x * m_ptVelocity.x) + (m_ptVelocity.y * m_ptVelocity.y));

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
}

Bitmap* Flame::m_pFlameBitmap = nullptr;

//-----------------------------------------------------------------
// Fireball General Methods
//-----------------------------------------------------------------
SPRITEACTION Fireball::Update() {

	SPRITEACTION out = Sprite::Update();

	if (AmIStuck()) {
		Flame* pFlame = new Flame(Flame::m_pFlameBitmap, m_pLevel);
		pFlame->SetNumFrames(3);
		pFlame->SetPositionFromCenter(GetPositionFromCenter().x - GetVelocity().x, GetPositionFromCenter().y - GetVelocity().y);
		pFlame->SetVelocity((-GetVelocity().x / 3) + (rand() % 40) - 20, (-GetVelocity().y / 3) + (rand() % 40) - 20);
		_pGame->AddSprite(pFlame);
		pFlame = new Flame(Flame::m_pFlameBitmap, m_pLevel);
		pFlame->SetNumFrames(3);
		pFlame->SetPositionFromCenter(GetPositionFromCenter().x - GetVelocity().x, GetPositionFromCenter().y - GetVelocity().y);
		pFlame->SetVelocity((-GetVelocity().x / 3) + (rand() % 40) - 20, (-GetVelocity().y / 3) + (rand() % 40) - 20);
		_pGame->AddSprite(pFlame);
		return SA_KILL;		
	}

	return out;
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

	float multiplier = 8 / sqrt((m_ptVelocity.x * m_ptVelocity.x) + (m_ptVelocity.y * m_ptVelocity.y));

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

	for (Sprite* sprite : *(_pGame->GetSpritesListPointer())) {
		if (sprite == this) continue;

		Enemy* enemy = dynamic_cast<Enemy*>(sprite);
		if (enemy) {
			if (enemy->TestCollision(this)) {
				enemy->Kill();
			}
			continue;
		}
		Flame* flame = dynamic_cast<Flame*>(sprite);
		if (flame) {
			if (flame->TestCollision(this)) {
				if (!flame->IsCloned() && flame->GetCloneDepth() < 4) {
					
					Flame* pFlame = new Flame(flame->GetBitmap(), m_pLevel);
					flame->SetCloneDepth(flame->GetCloneDepth() + 1);
					pFlame->SetCloneDepth(flame->GetCloneDepth());
					flame->SetCloned(true);
					pFlame->SetCloned(true);
					pFlame->SetNumFrames(3);
					if (m_ptVelocity.x == 0) {
						pFlame->SetPositionFromCenter(flame->GetPositionFromCenter().x - 40, flame->GetPositionFromCenter().y);
						flame->SetPositionFromCenter(flame->GetPositionFromCenter().x + 40, flame->GetPositionFromCenter().y);
					}
					else {
						pFlame->SetPositionFromCenter(flame->GetPositionFromCenter().x, flame->GetPositionFromCenter().y - 40);
						flame->SetPositionFromCenter(flame->GetPositionFromCenter().x, flame->GetPositionFromCenter().y + 40);
					}
					_pGame->AddSprite(pFlame);
				}

				if (m_ptVelocity.x == 0) {
					flame->SetVelocity(m_ptVelocity.x - ((GetPositionFromCenter().x - flame->GetPositionFromCenter().x) / 6), (m_ptVelocity.y / 2) + flame->GetVelocity().y);
				}
				else {
					flame->SetVelocity((m_ptVelocity.x / 2) + flame->GetVelocity().x, m_ptVelocity.y - ((GetPositionFromCenter().y - flame->GetPositionFromCenter().y) / 6));
				}
				flame->SetTime((rand() % 40) + 80);
			}
			continue;
		}
		Rock* rock = dynamic_cast<Rock*>(sprite);
		if (rock) {
			if (rock->TestCollision(this)) {
				if (m_ptVelocity.x == 0) {
					rock->SetVelocity(m_ptVelocity.x - ((GetPositionFromCenter().x - rock->GetPositionFromCenter().x) / 6), ((3 * m_ptVelocity.y) / 2) + rock->GetVelocity().y);
				}
				else {
					rock->SetVelocity(((3 * m_ptVelocity.x) / 2) + rock->GetVelocity().x, m_ptVelocity.y - ((GetPositionFromCenter().y - rock->GetPositionFromCenter().y) / 6));
				}
			}
			continue;
		}
	}

	return out;
}


