#include "WorldSocket.hpp"
#include "../Map/Map.hpp"
#include "PacketDefine.hpp"

WorldSocket::WorldSocket()
{
    m_Player = nullptr;
}


WorldSocket::~WorldSocket()
{
    if (m_Player == nullptr)
        return;

    /// Save Player
    delete m_Player;
    //m_Player->SetInWorld(false);
    m_Player = nullptr;
}

void WorldSocket::SendAuthResponse(uint8 p_Status)
{
	WorldPacket l_Packet;
	uint8 l_ID = SMSG::S_Connexion;

	l_Packet << l_ID << p_Status;
	send(l_Packet);
	printf("Send Status Auth %d\n", p_Status);
}

void WorldSocket::SendPlayerCreate(uint32 p_ID, std::string p_Name, uint8 p_Level, uint8 p_Health, uint8 p_Mana, uint8 p_Alignment, int16 p_SkinID, uint16 p_MapID, uint32 p_PosX, uint32 p_PosY, uint8 p_Orientation)
{
    WorldPacket l_Packet;
    uint8 l_ID = SMSG::S_PlayerCreate;

    l_Packet << l_ID << p_ID << p_Name << p_Level << p_Health << p_Mana << p_Alignment << p_SkinID << p_MapID << p_PosX << p_PosY << p_Orientation;
    send(l_Packet);
    printf("Send create\n");
}

void WorldSocket::SendUnitCreateToSet(uint8 p_Type, uint32 p_ID, std::string p_Name, uint8 p_Level, uint8 p_Health, uint8 p_Mana, uint8 p_Alignment, uint8 p_SkinID, uint16 p_MapID, uint32 p_PosX, uint32 p_PosY, uint8 p_Orientation, bool p_InMovement, bool p_IsAttacking, bool p_IsBlocking)
{
    WorldPacket l_Packet;
    uint8 l_ID = SMSG::S_UnitCreate;

    if (p_Type < TypeUnit::AREATRIGGER)
        l_Packet << l_ID << p_Type << p_ID << p_Name << p_Level << p_Health << p_Mana << p_Alignment << p_SkinID << p_MapID << p_PosX << p_PosY << p_Orientation << p_InMovement << p_IsAttacking;
    else
        l_Packet << l_ID << p_Type << p_ID << p_Name << p_SkinID << p_MapID << p_PosX << p_PosY << p_Orientation << p_InMovement << p_IsBlocking;

    SendToSet(l_Packet, true);
}

void WorldSocket::SendUnitCreate(uint8 p_Type, uint32 p_ID, std::string p_Name, uint8 p_Level, uint8 p_Health,  uint8 p_Mana, uint8 p_Alignment, uint8 p_SkinID, uint8 p_SizeX, uint8 p_SizeY, uint8 p_Speed, uint16 p_MapID, Position p_Position, uint8 p_Orientation, bool p_InMovement, bool p_IsAttacking, bool p_IsBlocking)
{
     if (p_Type == TypeUnit::PLAYER && p_ID == GetPlayer()->GetID())
        return;

     if (p_SkinID < 0)
         return;

    PacketUnitCreate l_Packet;

    if (p_Type < TypeUnit::AREATRIGGER)
        l_Packet.BuildPacket(p_Type, p_ID, p_Name, p_Level, p_Health, p_Mana, p_Alignment, p_SkinID, p_SizeX, p_SizeY, p_Speed, p_MapID, p_Position, p_Orientation, p_InMovement, p_IsAttacking);
    else
        l_Packet.BuildPacket(p_Type, p_ID, p_Name, p_SkinID, p_SizeX, p_SizeY, p_Speed, p_MapID, p_Position, p_Orientation, p_InMovement, p_IsBlocking);

    send(l_Packet.m_Packet);
}

void WorldSocket::SendUnitGoDirectionToSet(uint8 p_Type, uint16 p_UnitID, const Position & p_Pos, uint8 p_Direction)
{
    WorldPacket l_Packet;
    uint8 l_ID = SMSG::S_UnitGoDirection;

    l_Packet << l_ID << p_Type << p_UnitID << p_Pos.m_X << p_Pos.m_Y << p_Direction;
    SendToSet(l_Packet, true);
}

void WorldSocket::SendUnitTalk(uint8 p_Type, uint16 p_UnitID, const std::string & p_TalkString)
{
    WorldPacket l_Packet;
    uint8 l_ID = SMSG::S_UnitTalk;

    l_Packet << l_ID << p_Type << p_UnitID << p_TalkString;
    SendToSet(l_Packet);
}

void WorldSocket::SendUnitStopMovement(uint8 p_TypeID, uint16 p_ID, const Position & p_Pos, uint8 p_Orientation)
{
    WorldPacket l_Packet;
    uint8 l_ID = SMSG::S_UnitStopMovement;

    l_Packet << l_ID << p_TypeID << p_ID << p_Pos.m_X << p_Pos.m_Y << p_Orientation;
    SendToSet(l_Packet, true);
}

void WorldSocket::SendUnitStartAttack(uint8 p_TypeID, uint16 p_ID, const Position & p_Pos, uint8 p_Orientation)
{
    WorldPacket l_Packet;
    uint8 l_ID = SMSG::S_UnitStartAttack;

    l_Packet << l_ID << p_TypeID << p_ID << p_Pos.m_X << p_Pos.m_Y << p_Orientation;
    SendToSet(l_Packet, true);
}

void WorldSocket::SendUnitStopAttack(uint8 p_TypeID, uint16 p_ID)
{
    WorldPacket l_Packet;
    uint8 l_ID = SMSG::S_UnitStopAttack;

    l_Packet << l_ID << p_TypeID << p_ID;
    SendToSet(l_Packet, true);
}

void WorldSocket::SendLogDamage(uint8 p_TypeID, uint16 p_ID, DamageInfo p_DamageInfo)
{
	PacketLogDamage l_Packet;
	l_Packet.BuildPacket(p_TypeID, p_ID, p_DamageInfo.m_Damage, p_DamageInfo.m_Result);
	SendPacket(l_Packet.m_Packet);
}

void WorldSocket::SendSwitchMap(uint16 p_MapID)
{
    PacketSwitchMap l_Packet;
    l_Packet.BuildPacket(p_MapID);
    send(l_Packet.m_Packet);
}

void WorldSocket::SendKeyBoardBind(eKeyBoardAction p_Type, uint8 p_Key)
{
    PacketKeyBoardBind l_Packet;
    l_Packet.BuildPacket(p_Type, p_Key);
    send(l_Packet.m_Packet);
}


void WorldSocket::SendUpdateUnitResource(uint8 p_TypeID, uint16 p_ID, uint8 p_Resource, uint8 p_NewHealth)
{
    WorldPacket l_Packet;
    uint8 l_ID = SMSG::S_UnitUpdateResource;

    l_Packet << l_ID << p_TypeID << p_ID << p_Resource << p_NewHealth;
    SendToSet(l_Packet);
}

void WorldSocket::SendUpdateXpPct(float p_NewPct)
{
    WorldPacket l_Packet;
    uint8 l_ID = SMSG::S_PlayerUpdateXp;

    l_Packet << l_ID << p_NewPct;
    send(l_Packet);
}

Player* WorldSocket::GetPlayer()
{
    return m_Player;
}

void WorldSocket::SetPlayer(Player* p_Player)
{
    m_Player = p_Player;
}

void WorldSocket::SendPacket(WorldPacket p_Packet)
{
    send(p_Packet);
}

void WorldSocket::SendToSet(WorldPacket p_Packet, bool p_ExcludePlayer /*= false*/)
{
    Map* l_Map = GetPlayer()->GetMap();

    if (l_Map == nullptr)
        return;

    std::vector<Square*> l_SquareSet = l_Map->GetSquareSet(GetPlayer()->GetSquareID());
    for (Square* l_Square : l_SquareSet)
    {
        std::map<uint16, Unit*>* l_SquareList = l_Square->GetList(TypeUnit::PLAYER);
        if (l_SquareList == nullptr)
            continue;

        for (std::pair<uint16, Unit*> l_UnitPair : *l_SquareList)
        {
            Unit* l_Unit = l_UnitPair.second;

            if (l_Unit == nullptr)
                continue;

            if (Player* l_Player = l_Unit->ToPlayer())
            {
                if (p_ExcludePlayer && l_Player->GetID() == GetPlayer()->GetID())
                    continue;

                ///printf("Send to %s\n", l_Player->GetName().c_str());
                l_Player->GetSession()->send(p_Packet);
            }
        }
    }
}
