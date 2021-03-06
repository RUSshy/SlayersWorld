#pragma once
#include "Unit.hpp"

class Creature : public Unit
{
public:
    Creature(uint16, CreatureTemplate*, uint16, uint32, uint32);
    ~Creature();

    void Update(sf::Time);
    void RandMoving();
    void StartMovement(Orientation);
    void StopMovement();
    void StartAttack(Unit*);
    void StopAttack();
    void UpdateOrientation(Orientation);
    void Respawn();
    void Unspawn();
    void SetResourceNb(eResourceType, uint8);
    void AddResourceNb(eResourceType, uint8);
    void ResetRandMovementTime(bool);
    uint32 GetXpEarn() const;
    Orientation GetOrientationByPath(Path &);
    bool IsFollowingPath() const;
    void ReturnToRespawnPoint();
    void ReturnInRay();
    void GoToCase(const Position &);
    void OutOfCombat();
    uint16 GetEntry() const;
    int32 GetRewardID() const;

    /// CREATURE AI
    void UpdateAI(sf::Time);
    void UpdatePassive(sf::Time);
    void UpdateDefensive(sf::Time);
    void UpdateAgresive(sf::Time);
    void UpdatePassiveAttackEffect(sf::Time);

private:
    uint16 m_Entry;
    uint64 m_DiffMovementTime;
    uint64 m_DiffAttackEffect;
    CreatureTemplate* m_CreatureTemplate;
    float m_RandMovementTime;
    bool m_Evade;
    bool m_Static;

    Path m_PathToTargetPosition;
};

