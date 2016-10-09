#include "SqlManager.hpp"
#include "../Define.hpp"
#include "../Entities/Creature.hpp"
#include <cstdlib>

SqlManager::SqlManager()
{
	mysql_init(&m_MysqlCharacters);
    mysql_init(&m_MysqlWorld);
}


SqlManager::~SqlManager()
{
    mysql_close(&m_MysqlCharacters);
    mysql_close(&m_MysqlWorld);
}

bool SqlManager::InitializeCharacters(std::string p_Host, std::string p_User, std::string p_PassWord, std::string p_DB, std::string p_Port)
{
	if (!mysql_real_connect(&m_MysqlCharacters, p_Host.c_str(), p_User.c_str(), p_PassWord.c_str(), p_DB.c_str(), std::stoi(p_Port), NULL, 0))
		return false;
    my_bool l_AutoRecoonect = true; ///< To auto reconnect if it get disconnected
    mysql_options(&m_MysqlCharacters, MYSQL_OPT_RECONNECT, &l_AutoRecoonect);
	return true;
}

bool SqlManager::InitializeWorld(std::string p_Host, std::string p_User, std::string p_PassWord, std::string p_DB, std::string p_Port)
{
    if (!mysql_real_connect(&m_MysqlWorld, p_Host.c_str(), p_User.c_str(), p_PassWord.c_str(), p_DB.c_str(), std::stoi(p_Port), NULL, 0))
        return false;
    my_bool l_AutoRecoonect = true; ///< To auto reconnect if it get disconnected
    mysql_options(&m_MysqlWorld, MYSQL_OPT_RECONNECT, &l_AutoRecoonect);
    return true;
}

int32 SqlManager::GetIDLogin(std::string p_Login, std::string p_Password)
{
    std::string l_Query = "SELECT `id` FROM `login` WHERE `login` = '" + p_Login + "' AND `password` = MD5('" + p_Password + "')";
    mysql_query(&m_MysqlCharacters, l_Query.c_str());

    int32 l_ID = 0;
    MYSQL_RES *l_Result = NULL;
    MYSQL_ROW l_Row;

    l_Result = mysql_use_result(&m_MysqlCharacters);
    while ((l_Row = mysql_fetch_row(l_Result)))
        l_ID = atoi(l_Row[0]);

    mysql_free_result(l_Result);
    return l_ID;
}

Player* SqlManager::GetNewPlayer(uint32 p_AccountID)
{
    std::string l_Query = "SELECT characterID, name, level, health, alignment, skinID, mapID, posX, posY, orientation FROM characters WHERE accountID = '" + std::to_string(p_AccountID) + "'";
    mysql_query(&m_MysqlCharacters, l_Query.c_str());

    uint32 l_ID = 0;
    std::string l_Name = "";
    uint8 l_Lvl = 0;
    uint8 l_Health = 0;
    uint8 l_Alignment = 0;
    uint8 l_SkinID = 0;
    uint16 l_MapID = 0;
    uint32 l_PosX = 0;
    uint32 l_PosY = 0;
    uint8 l_Orientation = 0;

    MYSQL_RES *l_Result = NULL;
    MYSQL_ROW l_Row;
    Player* l_Player = nullptr;

    l_Result = mysql_use_result(&m_MysqlCharacters);
    while ((l_Row = mysql_fetch_row(l_Result)))
    {
        l_ID = atoi(l_Row[0]);
        l_Name = std::string(l_Row[1]);
        l_Lvl = atoi(l_Row[2]);
        l_Health = atoi(l_Row[3]);
        l_Alignment = atoi(l_Row[4]);
        l_SkinID = atoi(l_Row[5]);
        l_MapID = atoi(l_Row[6]);
        l_PosX = atoi(l_Row[7]);
        l_PosY = atoi(l_Row[8]);
        l_Orientation = atoi(l_Row[9]);
    }
    mysql_free_result(l_Result);

    l_Player = new Player(l_ID, l_Name, l_Lvl, l_Health, l_SkinID, l_MapID, l_PosX, l_PosY, (Orientation)l_Orientation);
    l_Player->SetAlignment(l_Alignment);
    l_Player->SetRespawnPosition(GetRespawnPositionForPlayer(l_ID));

    return l_Player;
}

WorldPosition SqlManager::GetRespawnPositionForPlayer(uint32 p_PlayerID)
{
    std::string l_Query = "SELECT posX, posY, mapID, orientation FROM characters_respawn WHERE characterID = '" + std::to_string(p_PlayerID) + "'";
    mysql_query(&m_MysqlCharacters, l_Query.c_str());
 
    uint32 l_PosX = 0;
    uint32 l_PosY = 0;
    uint16 l_MapID = 0;
    uint8 l_Orientation = 0;

    MYSQL_RES *l_Result = NULL;
    MYSQL_ROW l_Row;
    l_Result = mysql_use_result(&m_MysqlCharacters);
    while ((l_Row = mysql_fetch_row(l_Result)))
    {
        l_PosX = atoi(l_Row[0]);
        l_PosY = atoi(l_Row[1]);
        l_MapID = atoi(l_Row[2]);
        l_Orientation = atoi(l_Row[3]);
    }
    mysql_free_result(l_Result);

    WorldPosition l_Position(l_PosX, l_PosY, l_MapID, (Orientation)l_Orientation);
    return l_Position;
}

void SqlManager::SavePlayer(Player const* p_Player)
{
    std::string l_Query = "UPDATE characters SET posX = '" + std::to_string(p_Player->GetPosX())  + "', posY = '" + std::to_string(p_Player->GetPosY()) + "', mapID = '" + std::to_string(p_Player->GetMapID()) + "', orientation = '" + std::to_string(p_Player->GetOrientation()) + "', health = '" + std::to_string(p_Player->GetHealth()) + "', alignment = '" + std::to_string(p_Player->GetAlignment()) + "' WHERE characterID = '" + std::to_string(p_Player->GetID()) + "'";
    mysql_query(&m_MysqlCharacters, l_Query.c_str());
}

bool SqlManager::InitializeCreatureTemplate(CreatureManager* p_CreatureManager)
{
    std::string l_Query = "SELECT `entry`, `skinID`, `name`, `level`, `force`, `stamina`, `dexterity`, `xp`, `state`, `maxRay`, `respawnTime`, `rank`, `aiType` FROM creature_template";
    mysql_query(&m_MysqlWorld, l_Query.c_str());

    uint32 l_Entry = 0;
    uint8 l_SkinID = 0;
    std::string l_Name = "";
    uint8 l_Lvl = 0;
    uint8 l_Force = 0;
    uint8 l_Stamina = 0;
    uint8 l_Dexterity = 0;
    uint8 l_Xp = 0;
    uint8 l_State = 0;
    uint16 l_MaxRay = 0;
    uint16 l_RespawnTime = 0;
    uint8 l_Rank = 0;
    uint8 l_AiType = 0;

    MYSQL_RES *l_Result = NULL;
    MYSQL_ROW l_Row;
    l_Result = mysql_use_result(&m_MysqlWorld);
    while ((l_Row = mysql_fetch_row(l_Result)))
    {
        l_Entry = atoi(l_Row[0]);
        l_SkinID = atoi(l_Row[1]);
        l_Name = std::string(l_Row[2]);
        l_Lvl = atoi(l_Row[3]);
        l_Force = atoi(l_Row[4]);
        l_Stamina = atoi(l_Row[5]);
        l_Dexterity = atoi(l_Row[6]);
        l_Xp = atoi(l_Row[7]);
        l_State = atoi(l_Row[8]);
        l_MaxRay = atoi(l_Row[9]);
        l_RespawnTime = atoi(l_Row[10]);
        l_Rank = atoi(l_Row[11]);
        l_AiType = atoi(l_Row[12]);
        p_CreatureManager->AddCreatureTemplate(CreatureTemplate(l_Entry, l_SkinID, l_Name, l_Lvl, l_Force, l_Stamina, l_Dexterity, l_Xp, l_State, l_MaxRay, l_RespawnTime, l_Rank, l_AiType));
    }
    mysql_free_result(l_Result);

    return true;
}

bool SqlManager::InitializeCreature(MapManager* p_MapManager, CreatureManager* p_CreatureManager)
{
    std::string l_Query = "SELECT `id`, `entry`, `mapID`, `posX`, `posY` FROM creature";
    mysql_query(&m_MysqlWorld, l_Query.c_str());

    uint16 l_Id = 0;
    uint16 l_Entry = 0;
    uint16 l_MapID = 0;
    uint32 l_PosX = 0;
    uint32 l_PosY = 0;

    MYSQL_RES *l_Result = NULL;
    MYSQL_ROW l_Row;
    l_Result = mysql_use_result(&m_MysqlWorld);
    while ((l_Row = mysql_fetch_row(l_Result)))
    {
        l_Id = atoi(l_Row[0]);
        l_Entry = atoi(l_Row[1]);
        l_MapID = atoi(l_Row[2]);
        l_PosX = atoi(l_Row[3]);
        l_PosY = atoi(l_Row[4]);

        Creature* l_Creature = new Creature(l_Id, l_Entry, p_CreatureManager->GetCreatureTemplate(l_Entry), l_MapID, l_PosX, l_PosY);
        Map* l_Map = p_MapManager->GetMap(l_MapID);
        l_Map->AddUnit(l_Creature);
    }
    mysql_free_result(l_Result);

    return true;
}