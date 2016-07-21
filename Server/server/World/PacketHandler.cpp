#include "PacketHandler.hpp"

PacketHandler::PacketHandler(SqlManager* p_SqlManager, MapManager* p_MapManager) :
    m_SqlManager(p_SqlManager),
    m_MapManager(p_MapManager)
{
}

PacketHandler::~PacketHandler()
{
}

void PacketHandler::LoadPacketHandlerMap()
{
    m_PacketHandleMap[1] = &PacketHandler::HandleConnexion;
}

void PacketHandler::HandleConnexion(WorldPacket &p_Packet, WorldSocket* p_WorldSocket)
{
    if (m_SqlManager == nullptr)
        return;

    std::string l_Login;
    std::string l_Password;
    p_Packet >> l_Login;
    p_Packet >> l_Password;

    if (g_Config->IsPositiveValue("LoginDebug"))
    {
        p_WorldSocket->SendAuthResponse(1);
        return;
    }

    uint32 l_Id = m_SqlManager->GetIDLogin(l_Login, l_Password);
    if (!l_Id)
    {
        p_WorldSocket->SendAuthResponse(0); ///< Auth Failed
        return;
    }
    /// Auth Success
    p_WorldSocket->SendAuthResponse(1); ///< Auth Success

    /// Creation Player
    Player* l_Player = m_SqlManager->GetNewPlayer(l_Id);
    Map* l_Map = m_MapManager->GetMap(l_Player->GetMapID());

    if (l_Player == nullptr || l_Map == nullptr)
    {
        printf("Failed load player");
        if (l_Player != nullptr)
            printf(": Map = %d", l_Player->GetMapID());
        printf("\n");
        return;
    }
    l_Player->SetSession(p_WorldSocket);
    l_Player->SetMap(l_Map);
    printf("Load Player success\n");

    /// Send to Player
    p_WorldSocket->SendPlayerCreate(l_Player->GetID(), l_Player->GetName(), l_Player->GetLevel(), l_Player->GetSkinID(), l_Player->GetMapID(), l_Player->GetPosX(), l_Player->GetPosY(), l_Player->GetOrientation());
    p_WorldSocket->SetPlayer(l_Player);
}

void PacketHandler::OperatePacket(WorldPacket &p_Packet, WorldSocket* p_WorldSocket)
{
    uint8 l_PacketID;
    p_Packet >> l_PacketID;
    printf("Receive Packet %d\n", l_PacketID);
    m_Func l_Fun = m_PacketHandleMap[l_PacketID];
    if (l_Fun != nullptr)
        (this->*(l_Fun))(p_Packet, p_WorldSocket);
    else
        printf("Packet %d Unknow\n", l_PacketID);

}