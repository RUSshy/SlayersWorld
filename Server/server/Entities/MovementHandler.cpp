#include "MovementHandler.hpp"

MovementHandler::MovementHandler()
{
    m_Speed = 1.0f;
    m_InMovement = false;
    m_Orientation = Orientation::Down;
    m_PosX = 0;
    m_PosY = 0;
    m_DiffTime = 0;
}


MovementHandler::~MovementHandler()
{
}

void MovementHandler::Update(sf::Time p_Diff)
{
    if (!m_InMovement)
        return;

    m_DiffTime += p_Diff.asMicroseconds();

    while (m_DiffTime > (uint64)((UPDATE_TIME_MOVEMENT / STEP_SIZE) * 1000)) ///< 1000 because microsecond
    {
        /// UPDATE POSITION
        switch (m_Orientation)
        {
        case Orientation::Down:
            m_PosY += (uint32)((STEP_SIZE / STEP_SIZE) * m_Speed);
            break;
        case Orientation::Left:
            m_PosX -= (uint32)((STEP_SIZE / STEP_SIZE) * m_Speed);
            break;
        case Orientation::Right:
            m_PosX += (uint32)((STEP_SIZE / STEP_SIZE) * m_Speed);
            break;
        case Orientation::Up:
            m_PosY -= (uint32)((STEP_SIZE / STEP_SIZE) * m_Speed);
            break;
        default:
            break;
        }
        m_DiffTime -= (uint64)((UPDATE_TIME_MOVEMENT / STEP_SIZE) * 1000);
    }
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