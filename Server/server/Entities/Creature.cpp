#include "Creature.hpp"
#include "../Map/Map.hpp"
#include "../World/PacketDefine.hpp"

Creature::Creature(uint16 p_ID, uint16 p_Entry, CreatureTemplate p_CreatureTemplate, uint16 p_MapID, uint32 p_PosX, uint32 p_PosY) :
    Unit(p_ID, TypeUnit::CREATURE)
{
    m_Entry = p_Entry;
    m_MapID = p_MapID;
    SetPosX(p_PosX);
    SetPosY(p_PosY);
    m_SkinID = p_CreatureTemplate.m_SkinID;
    m_Level = p_CreatureTemplate.m_Level;
    m_Name = p_CreatureTemplate.m_Name;
    m_CreatureTemplate = p_CreatureTemplate;
    m_DiffMovementTime = 0;

    m_RespawnPosition.SetMapID(p_MapID);
    m_RespawnPosition.SetPosX(p_PosX);
    m_RespawnPosition.SetPosY(p_PosY);
    m_MovementHandler->SetSpeed(0.5f);
    m_RandMovementTime = rand() % 100;

    m_RespawnTime = m_CreatureTemplate.m_RespawnTime * IN_MILLISECOND;
}


Creature::~Creature()
{
}

void Creature::RandMoving()
{
    uint8 l_Orientation = rand() % 4;
    if (GetDistance(m_RespawnPosition.GetPosition()) > CaseToPixel(m_CreatureTemplate.m_MaxRay))
        l_Orientation = GetOrientationToPoint(m_RespawnPosition.GetPosition());
    m_MovementHandler->StartMovement((Orientation)l_Orientation);
    PacketGoDirection l_Packet;
    l_Packet.BuildPacket((uint8)TypeUnit::CREATURE, GetID(), GetPosition(), GetOrientation());
    m_Map->SendToSet(l_Packet.m_Packet, this);
}

void Creature::StopMovement()
{
    if (!m_MovementHandler->IsInMovement())
        return;

    m_MovementHandler->StopMovement();
    PacketStopMovement l_Packet;
    l_Packet.BuildPacket((uint8)TypeUnit::CREATURE, GetID(), GetPosition(), GetOrientation());
    m_Map->SendToSet(l_Packet.m_Packet, this);
}


void Creature::UpdatePassive(sf::Time p_Diff)
{
    if (!IsInCombat())
    {
        while (m_DiffMovementTime > (1.5f * IN_MICROSECOND) + (((1.5f * IN_MICROSECOND) / 100.0f) * m_RandMovementTime)) ///< 1000 because microsecond
        {
            if (m_MovementHandler->IsInMovement())
                StopMovement();
            else
                RandMoving();
            m_DiffMovementTime = 0;
            m_RandMovementTime = rand() % 100;
        }
    }
    else
    {
        while (m_DiffMovementTime > (1.5f * IN_MICROSECOND)) ///< 1000 because microsecond
        {
            RandMoving();
            m_DiffMovementTime = 0;
        }
    }
}

void Creature::Update(sf::Time p_Diff)
{
    Unit::Update(p_Diff);

    if (!IsInWorld())
        return;

    switch (m_CreatureTemplate.m_AiType)
    {
        case eAiType::PASSIVE:
            UpdatePassive(p_Diff);
            break;
        default:
            break;
    }
    m_DiffMovementTime += p_Diff.asMicroseconds();
}

void Creature::SetHealth(const uint8 & p_Health)
{
    Unit::SetHealth(p_Health);
    if (IsDeath())
        Unspawn();
}

void Creature::Unspawn()
{
    m_MovementHandler->StopMovement();
    m_MovementHandler->StopAttack();
    m_DiffMovementTime = 0;
    m_RandMovementTime = rand() % 100;

    /// Unspawn for players
    PacketUnitRemove l_Packet;
    l_Packet.BuildPacket((uint8)TypeUnit::CREATURE, GetID());
    m_Map->SendToSet(l_Packet.m_Packet, this);

    SetInWorld(false);
}

void Creature::Respawn()
{
    Unit::Respawn();
    m_DiffMovementTime = 0;
    SetInWorld(true);

    SetHealth(MAX_HEALTH);
    m_ResTimer = 0;

    /// Respawn creature for players
    PacketUnitCreate l_Packet;
    l_Packet.BuildPacket((uint8)TypeUnit::CREATURE, GetID(), GetName(), GetLevel(), GetHealth(), GetSkinID(), GetMapID(), GetPosition(), GetOrientation(), m_MovementHandler->IsInMovement());
    m_Map->SendToSet(l_Packet.m_Packet, this);
}