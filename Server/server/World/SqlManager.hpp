#pragma once

#ifdef WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

#include <mysql.h>
#include <string>
#include "../Define.hpp"
#include "../Entities/Player.hpp"
#include "../System/WorldPosition.hpp"
#include "../Map/MapManager.hpp"
#include "CreatureManager.hpp"

class SqlManager
{
public:
	SqlManager();
	~SqlManager();
	bool InitializeCharacters(std::string, std::string, std::string, std::string, std::string);
    bool InitializeWorld(std::string, std::string, std::string, std::string, std::string);

    int32 GetIDLogin(std::string, std::string);
    int32 GetIDCharacter(uint16 p_AccountID);
    void AddNewAccount(std::string, std::string);
    bool IsExistingAccound(std::string);
    Player* GetNewPlayer(uint32);
    void AddNewPlayer(uint32);
    std::string GetLoginName(uint32);
    int16 GetLevel(const std::string &);
    WorldPosition GetPosition(const std::string &);
    WorldPosition GetRespawnPositionForPlayer(uint32);
    void AddNewRespawnPositionForPlayer(uint32);
	PointsSet GetPointsSetForPlayer(uint32);
	void AddNewPointsSetForPlayer(uint32);
    void SavePlayer(Player const*);
	void UpdatePointsSet(Player const*);
    void AddConnectionLogin(uint32);

    bool InitializeCreatureTemplate(CreatureManager*);
    bool InitializeCreature(CreatureManager*);
    std::map<uint8, uint16> GetXpLevel();
    int32 GetPlayerID(const std::string &);

private:
	MYSQL m_MysqlCharacters;
    MYSQL m_MysqlWorld;
};

