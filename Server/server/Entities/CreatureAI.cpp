#include "Creature.hpp"
#include "../Map/Map.hpp"
#include "../World/PacketDefine.hpp"

void Creature::OutOfCombat()
{
    Unit::OutOfCombat();
    m_PathToTargetPosition.clear();
}

void Creature::UpdateAI(sf::Time p_Diff)
{
    if (!IsInWorld())
        return;

    if (IsFollowingPath())
    {
        Orientation l_Orientation = GetOrientationByPath(m_PathToTargetPosition);
        if (IsFollowingPath() && (GetOrientation() != l_Orientation || !IsInMovement()))
        {
            //printf("Change Orientation [%d] to %d\n", GetOrientation(), l_Orientation);
            StartMovement(l_Orientation);
        }
    }
    switch (m_CreatureTemplate->m_AiType)
    {
        case eAiType::PASSIVE:
            UpdatePassive(p_Diff);
            break;
        case eAiType::DEFENSIVE:
            UpdateDefensive(p_Diff);
            break;
        case eAiType::AGRESIVE:
            UpdateAgresive(p_Diff);
            break;
        case eAiType::PASSIVE_ATTACK_EFFECT:
            UpdatePassiveAttackEffect(p_Diff);
            break;
        default:
            break;
    }
    if (!m_Static)
        m_DiffMovementTime += p_Diff.asMicroseconds();
}

void Creature::UpdatePassiveAttackEffect(sf::Time p_Diff)
{
    if (GetMovementHandler()->IsInAttack() && m_DiffAttackEffect > 3 * IN_MICROSECOND)
    {
        GetMovementHandler()->StopAttack();
        m_DiffAttackEffect = 0;
    }
    else if (m_DiffAttackEffect > 10 * IN_MICROSECOND)
    {
        GetMovementHandler()->StartAttack();
        m_DiffAttackEffect = 0;
    }
    m_DiffAttackEffect += p_Diff.asMicroseconds();
    UpdatePassive(p_Diff);
}

void Creature::UpdatePassive(sf::Time p_Diff)
{
    if (!IsInCombat())
    {
        while (m_DiffMovementTime > m_RandMovementTime * IN_MICROSECOND) ///< 1000 because microsecond
        {
            if (m_MovementHandler->IsInMovement())
            {
                ResetRandMovementTime(false);
                StopMovement();
            }
            else
                RandMoving();
        }
    }
    else
    {
        if (!IsInMovement()) ///< Go Run
        {
            RandMoving();
            return;
        }

        while (m_DiffMovementTime > m_RandMovementTime * IN_MICROSECOND) ///< 1000 because microsecond
            RandMoving();
    }
}

void Creature::UpdateDefensive(sf::Time p_Diff)
{
    if (!IsInCombat())
    {
        if (m_MovementHandler->IsInAttack() && !m_MovementHandler->IsStopingAttack()) ///< If in combat and still atacking
            StopAttack();

        if (m_MovementHandler->IsInAttack() || m_MovementHandler->IsStopingAttack())
            return;

        ReturnInRay();
        if (GetDistance(m_RespawnPosition.GetPosition()) <= CaseToPixel(m_CreatureTemplate->m_MaxRay) && m_MovementHandler->IsInMovement() && IsInEvade())
        {
            OutOfEvade();
            ResetRandMovementTime(false);
            StopMovement();
        }
        while (m_DiffMovementTime > m_RandMovementTime * IN_MICROSECOND) ///< 1000 because microsecond
        {
            if (m_MovementHandler->IsInMovement())
            {
                OutOfEvade();
                ResetRandMovementTime(false);
                StopMovement();
            }
            else
                RandMoving();
        }
    }
    else
    {
        if (GetNbAttacker() == 0)
        {
            OutOfCombat();
            return;
        }

        EnterInCombat(GetMaxThreatAttacker());

        if (GetVictim() == nullptr || !GetVictim()->IsInWorld())
        {
            OutOfCombat();
            return;
        }

        if (GetDistance(m_RespawnPosition.GetPosition()) > CaseToPixel(m_CreatureTemplate->m_MaxVision + m_CreatureTemplate->m_MaxRay))
        {
            EnterInEvade();
            OutOfCombat();
            return;
        }

        if (GetDistance(GetVictim()) > MELEE_RANGE)
        {
            if (m_MovementHandler->IsInAttack() && !m_MovementHandler->IsStopingAttack())
                StopAttack();
            if (!m_MovementHandler->IsInAttack())
            {
                m_RandMovementTime = 0;
                if (PositionToCasePosition(GetVictim()->GetPosition()) != PositionToCasePosition(GetPosition()))
                    GoToCase(GetVictim()->GetPositionCentered());
                else
                {
                    Orientation l_Orientation = GetOrientationToPoint(GetVictim()->GetPosition());
                    if (GetOrientation() != l_Orientation || !IsInMovement())
                        StartMovement(l_Orientation);
                }
            }
        }
        else
        {
            m_PathToTargetPosition.clear();
            if (m_MovementHandler->IsInMovement())
            {
                StopMovement();
            }

            if (m_MovementHandler->IsInAttack() && GetOrientationToPoint(GetVictim()) != GetOrientation())
                UpdateOrientation(GetOrientationToPoint(GetVictim()));

            if (!m_MovementHandler->IsInAttack())
            {
                SetOrientation(GetOrientationToPoint(GetVictim()));
                StartAttack(GetVictim());
            }
        }
    }
}

void Creature::UpdateAgresive(sf::Time p_Diff)
{
    if (GetVictim() == nullptr/*!IsInCombat()*/)
    {
        if (m_MovementHandler->IsInAttack() && !m_MovementHandler->IsStopingAttack()) ///< If in combat and still atacking
            StopAttack();

        if (m_MovementHandler->IsInAttack() || m_MovementHandler->IsStopingAttack())
            return;

        ReturnInRay();
        if (GetDistance(m_RespawnPosition.GetPosition()) <= CaseToPixel(m_CreatureTemplate->m_MaxRay))
        {
            if (m_MovementHandler->IsInMovement() && IsInEvade())
            {
                OutOfEvade();
                ResetRandMovementTime(false);
                StopMovement();
            }
            if (Unit* l_Victim = m_Map->GetCloserUnit(this, m_CreatureTemplate->m_MaxVision, true, false, true))
            {
                EnterInCombat(l_Victim);
                Position p_Pos = l_Victim->GetPositionAtDistance(l_Victim->GetDistance(this), l_Victim->GetAngle(GetPosition()));
                printf("POSITION = %d-%d   MINE %d-%d  IS %d-%d\n", p_Pos.m_X, p_Pos.m_Y, l_Victim->GetPosition().m_X, l_Victim->GetPosition().m_Y, GetPosX(), GetPosY());
                return;
            }
        }
        while (m_DiffMovementTime > m_RandMovementTime * IN_MICROSECOND) ///< 1000 because microsecond
        {
            if (m_MovementHandler->IsInMovement())
            {
                OutOfEvade();
                ResetRandMovementTime(false);
                StopMovement();
            }
            else
                RandMoving();
        }
    }
    else
    {
        if (GetDistance(m_RespawnPosition.GetPosition()) > CaseToPixel(m_CreatureTemplate->m_MaxVision + m_CreatureTemplate->m_MaxRay))
        {
            EnterInEvade();
            OutOfCombat();
            return;
        }

        if (GetVictim() == nullptr || !CanAttack(GetVictim()))
            EnterInCombat(GetMaxThreatAttacker());
        if (GetVictim() == nullptr /*|| !GetVictim()->IsInWorld()*/)
        {
            EnterInEvade();
            OutOfCombat();
            return;
        }

        if (GetDistance(GetVictim()) > MELEE_RANGE)
        {
            if (m_MovementHandler->IsInAttack() && !m_MovementHandler->IsStopingAttack())
                StopAttack();
            if (!m_MovementHandler->IsInAttack())
            {
                m_RandMovementTime = 0;
                GoToCase(GetVictim()->GetPositionCentered());
                /*if (!IsValidOrientationToPoint((Orientation)GetOrientation(), GetVictim()) || !IsInMovement())
                {
                    Orientation l_Orientation = GetOrientationToPoint(GetVictim());
                    if (GetOrientation() != l_Orientation || !IsInMovement())
                        StartMovement(l_Orientation);
                }*/
                if (m_PathToTargetPosition.empty())
                    SetVictim(nullptr);
            }
        }
        else
        {
            if (!IsInCombat())
                EnterInCombat(GetVictim());
            m_PathToTargetPosition.clear();
            if (m_MovementHandler->IsInMovement())
            {
                StopMovement();
            }

            if (m_MovementHandler->IsInAttack() && GetOrientationToPoint(GetVictim()) != GetOrientation())
                UpdateOrientation(GetOrientationToPoint(GetVictim()));

            if (!m_MovementHandler->IsInAttack())
            {
                SetOrientation(GetOrientationToPoint(GetVictim()));
                StartAttack(GetVictim());
            }
        }
    }
}