#include "MovementHandler.hpp"
#include "../Map/Map.hpp"

MovementHandler::MovementHandler(uint8 p_SizeX, uint8 p_SizeY) :
    m_SizeX(p_SizeX),
    m_SizeY(p_SizeY)
{
    m_Speed = 1.0f;
    m_InMovement = false;
    m_Orientation = Orientation::Down;
    m_PosX = 0;
    m_PosY = 0;
    m_DiffTime = 0;
    m_Map = nullptr;
}

MovementHandler::~MovementHandler()
{
}

bool MovementHandler::IsInColision(int64 p_PosX, int64 p_PosY) const
{
    if (m_Map == nullptr)
        return true;

    if (p_PosX < 0 || p_PosY < 0)
        return true;

    if (p_PosX + m_SizeX > m_Map->GetSizeX() * TILE_SIZE || p_PosY > m_Map->GetSizeY() * TILE_SIZE)
        return true;


    std::vector<Case*> l_CaseList;
    uint8 i = 0;
    for (uint8 i = 0; i < m_SizeX; i += TILE_SIZE)
        l_CaseList.push_back(m_Map->GetCase((uint32)p_PosX + i, (uint32)p_PosY));
    l_CaseList.push_back(m_Map->GetCase((uint32)p_PosX + m_SizeX, (uint32)p_PosY));

    for (Case* l_Case : l_CaseList)
    {
        if (l_Case == nullptr)
            return true;

        if (l_Case->IsBlocking())
            return true;
    }

    return false;
}

void MovementHandler::Update(sf::Time p_Diff)
{
    if (!m_InMovement)
        return;

    int64 l_PosX = m_PosX;
    int64 l_PosY = m_PosY;

    m_DiffTime += p_Diff.asMicroseconds();

    while (m_DiffTime > (uint64)((UPDATE_TIME_MOVEMENT / STEP_SIZE) * 1000)) ///< 1000 because microsecond
    {
        /// UPDATE POSITION
        switch (m_Orientation)
        {
        case Orientation::Down:
            l_PosY += (uint32)((STEP_SIZE / STEP_SIZE) * m_Speed);
            break;
        case Orientation::Left:
            l_PosX -= (uint32)((STEP_SIZE / STEP_SIZE) * m_Speed);
            break;
        case Orientation::Right:
            l_PosX += (uint32)((STEP_SIZE / STEP_SIZE) * m_Speed);
            break;
        case Orientation::Up:
            l_PosY -= (uint32)((STEP_SIZE / STEP_SIZE) * m_Speed);
            break;
        default:
            break;
        }
        m_DiffTime -= (uint64)((UPDATE_TIME_MOVEMENT / STEP_SIZE) * 1000);

        if (!IsInColision(l_PosX, l_PosY))
        {
            m_PosX = (uint32)l_PosX;
            m_PosY = (uint32)l_PosY;
        }
        else
            StopMovement();
    }
}

void MovementHandler::SetMap(Map* p_Map)
{
    m_Map = p_Map;
}

bool MovementHandler::IsInMovement() const
{
    return m_InMovement;
}

void MovementHandler::StartMovement(Orientation p_Orientation)
{
    m_InMovement = true;
    m_Orientation = p_Orientation;
}

void MovementHandler::StopMovement()
{
    m_InMovement = false;
    m_DiffTime = 0;
}

void MovementHandler::SetOrientation(Orientation p_Orientation)
{
    m_Orientation = p_Orientation;
}

Orientation MovementHandler::GetOrientation() const
{
    return m_Orientation;
}

void MovementHandler::SetPosX(uint32 p_PosX)
{
    m_PosX = p_PosX;
}

void MovementHandler::SetPosY(uint32 p_PosY)
{
    m_PosY = p_PosY;
}

uint32 MovementHandler::GetPosX() const
{
    return m_PosX;
}

uint32 MovementHandler::GetPosY() const
{
    return m_PosY;
}