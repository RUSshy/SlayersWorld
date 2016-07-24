#include "MovementHandler.hpp"



MovementHandler::MovementHandler()
{
    m_Speed = 1.0f;
    m_InMovement = false;
    m_MovementPosition = 0;
    m_Orientation = Orientation::Down;
    m_PosX = 0;
    m_PosY = 0;
    m_DiffTime = 0;
    m_DiffTimeUpdateAnimation = 0;
}


MovementHandler::~MovementHandler()
{
}

void MovementHandler::Update(sf::Time p_Diff)
{   
    UpdateAnimationWalk(p_Diff);
    if (!m_InMovement)
        return;

    m_DiffTime += p_Diff.asMilliseconds();

   /* while (m_DiffTime > (UPDATE_TIME_MOVEMENT / STEP_SIZE) * m_Speed)
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

        m_DiffTime -= (uint32)((UPDATE_TIME_MOVEMENT / STEP_SIZE) * m_Speed);
    }*/
}

void MovementHandler::UpdateAnimationWalk(sf::Time p_Diff)
{
    if (!m_InMovement)
        return;

    m_DiffTimeUpdateAnimation += p_Diff.asMilliseconds();

    if (m_DiffTimeUpdateAnimation > UPDATE_TIME_MOVEMENT * m_Speed)
    {
        /// UPDATE ANIMATION
        m_MovementPosition++;
        if (m_MovementPosition >= MAX_MOVEMENT_POSITION)
            m_MovementPosition = 0;

        /// UPDATE POSITION
       /*switch (m_Orientation)
        {
        case Orientation::Down:
            m_PosY += (uint32)(STEP_SIZE * m_Speed);
            break;
        case Orientation::Left:
            m_PosX -= (uint32)(STEP_SIZE * m_Speed);
            break;
        case Orientation::Right:
            m_PosX += (uint32)(STEP_SIZE * m_Speed);
            break;
        case Orientation::Up:
            m_PosY -= (uint32)(STEP_SIZE * m_Speed);
            break;
        default:
            break;
        }*/

        m_DiffTimeUpdateAnimation -= (uint32)(UPDATE_TIME_MOVEMENT * m_Speed);
    }
}


bool MovementHandler::IsInMovement() const
{
    return m_InMovement;
}

void MovementHandler::StartMovement(Orientation p_Orientation)
{
    m_InMovement = true;
    m_MovementPosition = 2;
}

void MovementHandler::StopMovement()
{
    m_InMovement = false;
    m_MovementPosition = 1;
}

void MovementHandler::SetOrientation(Orientation p_Orientation)
{
    m_Orientation = p_Orientation;
}

Orientation MovementHandler::GetOrientation() const
{
    return m_Orientation;
}


uint8 MovementHandler::GetMovementPosition()
{
    if (!m_InMovement)
        return m_MovementPosition;

    return m_MovementPosition;
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