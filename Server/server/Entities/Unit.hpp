#pragma once
#include "MovementHandler.hpp"
#include "../Define.hpp"
#include <string>

class Map;
class Player;

class Unit
{
public:
    Unit(uint16);
    Unit(uint16, TypeUnit);
    ~Unit();

    bool IsPlayer() const;
    std::string GetName() const;
    uint16 GetMapID() const;
    uint32 GetPosX() const;
    uint32 GetPosY() const;
    uint8 GetSizeX() const;
    uint8 GetSizeY() const;
    uint8 GetLevel() const;
    uint8 GetSkinID() const;
    uint8 GetOrientation() const;
    Map* GetMap() const;
    uint16 GetSquareID() const;
    uint16 GetID() const;
    uint8 GetHealth() const;

    void SetName(const std::string &);
    void SetLevel(const uint8 &);
    void SetMapID(const uint16 &);
    void SetPosX(const uint32 &);
    void SetPoxY(const uint32 &);
    void SetSkinID(const uint8 &);
    void SetHealth(const uint8 &);
    void SetOrientation(const Orientation &);
    void SetMap(Map*);
    void SetSquareID(uint16);
    TypeUnit GetType() const;
    bool IsInMovement() const;

    Player* ToPlayer();
    MovementHandler* GetMovementHandler();

    void Update(sf::Time);

protected:
    std::string m_Name;
    uint8 m_Level;
    uint16 m_MapID;

    uint8 m_SizeX;
    uint8 m_SizeY;

    uint32 m_PosX;
    uint32 m_PosY;
    uint8 m_Health;
    uint8 m_SkinID;
    MovementHandler* m_MovementHandler;
    TypeUnit m_Type;
    Map* m_Map;
    uint16 m_SquareID;
    uint16 m_ID;
    uint64 m_DiffUpdatePosTime;
};
