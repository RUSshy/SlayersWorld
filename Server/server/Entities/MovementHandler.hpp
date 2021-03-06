#pragma once
#include <SFML/System/Clock.hpp>
#include "../Define.hpp"
#include "../World/ClockHandler.hpp"
#include <queue>

class Map;

class MovementHandler
{
public:
    MovementHandler(uint8, uint8, TypeUnit);
    ~MovementHandler();
    virtual void StartMovement(Orientation p_Orientation);
    virtual void StopMovement();
    virtual void StartAttack();
    virtual void StopAttack();

    bool IsInAction() const;
    bool IsInMovement() const;
    bool IsInAttack() const;
    bool IsStopingAttack() const;
    void SetMap(Map*);
    bool IsInColision(int64, int64) const;
    bool CheckNextMovement(uint32, uint32);
    bool IsDamageReady() const;
    void SetDamageDone(bool);

    Orientation GetOrientation() const;
    void SetOrientation(Orientation);

    void SetPosX(uint32);
    void SetPosY(uint32);
    void SetSpeed(float);

    Position GetPos() const;
    uint32 GetPosX() const;
    uint32 GetPosY() const;
    float GetSpeed() const;

    void Update(sf::Time);
    void UpdateAttack(sf::Time);

    void AddMovementToStack(eActionType, Position, Orientation);
    void AddMovementToStack(eActionType);
    void ClearMovementStack();

    void SetStopPoint(bool, Position p_Position = Position(0, 0));

protected:
    bool m_InMovement;
    bool m_InAttack;
    bool m_StopAttack;
    float m_Speed;
    TypeUnit m_Type;
    Orientation m_Orientation;
    
    Position m_Pos;
    uint64 m_DiffTime;
    uint64 m_DiffTimeAttack;
    uint8 m_AttackPosition;
    uint8 m_SizeX;
    uint8 m_SizeY;

    Map* m_Map;
    struct MovementAction
    {
        eActionType m_ActionType;
        bool m_PositionOptions;
        Position m_Pos;
        Orientation m_Orientation;
    };

    std::queue<MovementAction> m_MovementStack;

    struct AttackDamageState
    {
        bool m_DamageReady;
        bool m_DamageDone;
    };

    AttackDamageState m_AttackDamage;

    struct StopPoint
    {
        bool m_Active;
        Position m_Position;
    };

    StopPoint m_StopPoint;
};
