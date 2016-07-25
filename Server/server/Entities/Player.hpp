#pragma once
#include "Unit.hpp"

class WorldSocket;

class Player : public Unit
{
public:
    Player(int32, std::string, uint8, uint8, uint16, uint32, uint32, Orientation);
    ~Player();
    uint32 GetID() const;
    WorldSocket* GetSession() const;
    void SetSession(WorldSocket*);

private:
    uint32 m_ID;
    WorldSocket* m_Session;
};

