#pragma once
#include <string>

#define MAP_PATH "map/"
#define CONF_FILE "server.conf"

#define GRID_SIZE 12
#define TILE_SIZE 16

#define IN_MILLISECOND 1000
#define IN_MICROSECOND 1000 * IN_MILLISECOND

#define PLAYER_TIME_RESPAWN 9
#define MAX_HEALTH 100

#define UPDATE_TIME_MOVEMENT 250
#define STEP_SIZE 8 /// In pixel
#define MAX_MOVEMENT_POSITION 3

typedef char int8;
typedef short int16;
typedef int int32;
typedef long long int64;
typedef unsigned long long uint64;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;

enum Orientation
{
    Up = 0,
    Right = 1,
    Down = 2,
    Left = 3
};

enum TypeUnit
{
    CREATURE = 0,
    PLAYER = 1
};

enum eActionType
{
    Go = 0,
    Attack = 1,
    Stop = 2,
    StopAttack = 3
};

struct Position 
{
    uint32 m_X;
    uint32 m_Y;

    Position() :
    m_X(0), m_Y(0) {}

    Position(uint32 p_X, uint32 p_Y) :
    m_X(p_X), m_Y(p_Y) {}

    Position(const Position & p_Position) :
    m_X(p_Position.m_X), m_Y(p_Position.m_Y) {}
};

struct CreatureTemplate
{
    uint32 m_Entry;
    uint8 m_SkinID;
    std::string m_Name;
    uint8 m_Level;
    uint8 m_Force;
    uint8 m_Stamina;
    uint8 m_Dexterity;
    uint8 m_Xp;
    uint8 m_State;
    uint16 m_MaxRay;
    uint16 m_RespawnTime;

    CreatureTemplate() :
    m_Entry(0), m_SkinID(0), m_Name(""), m_Level(0), m_Force(0), m_Stamina(0), m_Dexterity(0), m_Xp(0), m_State(0) {}

    CreatureTemplate(uint32 p_Entry, uint8 p_SkinID, std::string p_Name, uint8 p_Level, uint8 p_Force, uint8 p_Stamina, uint8 p_Dexterity, uint8 p_Xp, uint8 p_State, uint16 p_MaxRay, uint16 p_RespawnTime) :
    m_Entry(p_Entry), m_SkinID(p_SkinID), m_Name(p_Name), m_Level(p_Level), m_Force(p_Force), m_Stamina(p_Stamina), m_Dexterity(p_Dexterity), m_Xp(p_Xp), m_State(p_State), m_MaxRay(p_MaxRay), m_RespawnTime(p_RespawnTime) {}
};