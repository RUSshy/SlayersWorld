#include "Aura.hpp"
#include "../../World/PacketDefine.hpp"
#include "../../Map/Map.hpp"
#include "SpellTemplate.hpp"

Aura::Aura(Unit* p_Caster, Unit* p_Target, SpellTemplate* p_SpellTemplate) :
    m_SpellTemplate(p_SpellTemplate),
    m_Caster(p_Caster),
    m_Target(p_Target),
    m_ID(p_SpellTemplate->GetID()),
    m_Duration((uint64)p_SpellTemplate->GetDuration() * 1000) ///< Millisecond to microsecond
{
    if (m_Caster != nullptr)
    {
        m_CasterID = m_Caster->GetID();
        m_CasterType = m_Caster->GetType();
    }

    m_AuraEffectsMap[eTypeAuraEffect::UPDATE_SPEED] = &Aura::AuraEffectUpdateSpeed;
    m_AuraEffectsMap[eTypeAuraEffect::PERIODIC_HEAL] = &Aura::AuraEffectPeriodicHeal;
    m_AuraEffectsMap[eTypeAuraEffect::PERIODIC_DAMAGE] = &Aura::AuraEffectPeriodicDamage;
    m_AuraEffectsMap[eTypeAuraEffect::MODIFY_DAMAGE_PCT] = &Aura::AuraEffectModifyDamagePct;
    m_AuraEffectsMap[eTypeAuraEffect::MOUNT] = &Aura::AuraEffectMount;

    if (m_Target != nullptr && (m_SpellTemplate->GetVisualIDTarget() >= 0 || m_SpellTemplate->GetVisualIDTargetUnder() >= 0))
    {
        Map* l_Map = m_Target->GetMap();

        if (l_Map == nullptr)
            return;

        if (m_SpellTemplate->GetVisualIDTarget() >= 0)
        {
            PacketPlayAuraVisual l_Packet;
            l_Packet.BuildPacket(true, m_Target->GetType(), m_Target->GetID(), m_CasterType, m_CasterID, false, m_SpellTemplate->GetVisualIDTarget());
            l_Map->SendToSet(l_Packet.m_Packet, m_Target);
        }
        if (m_SpellTemplate->GetVisualIDTargetUnder() >= 0)
        {
            PacketPlayAuraVisual l_Packet;
            l_Packet.BuildPacket(true, m_Target->GetType(), m_Target->GetID(), m_CasterType, m_CasterID, true, m_SpellTemplate->GetVisualIDTargetUnder());
            l_Map->SendToSet(l_Packet.m_Packet, m_Target);
        }
    }
    if (m_Target != nullptr)
    {
        Map* l_Map = m_Target->GetMap();

        if (l_Map != nullptr)
            l_Map->UnitAddaura(m_Target, m_SpellTemplate->GetID(), this);
    }
}

Aura::~Aura()
{
    for (std::map<uint8, AuraEffect*>::iterator l_It = m_AuraEffectList.begin(); l_It != m_AuraEffectList.end(); l_It++)
    {
        m_AuraEffectFunc l_Fun = m_AuraEffectsMap[(*l_It).second->GetType()];
        (this->*(l_Fun))((*l_It).second, false);
        delete (*l_It).second;
    }
    if (m_Target != nullptr)
    {
        Map* l_Map = m_Target->GetMap();

        if (l_Map != nullptr)
            l_Map->UnitUnaura(m_Target, m_SpellTemplate->GetID());
    }

    if (m_Target != nullptr && (m_SpellTemplate->GetVisualIDTarget() >= 0 || m_SpellTemplate->GetVisualIDTargetUnder() >= 0))
    {
        Map* l_Map = m_Target->GetMap();

        if (l_Map == nullptr)
            return;

        if (m_SpellTemplate->GetVisualIDTarget() >= 0)
        {
            PacketPlayAuraVisual l_Packet;
            l_Packet.BuildPacket(false, m_Target->GetType(), m_Target->GetID(), m_CasterType, m_CasterID, false, m_SpellTemplate->GetVisualIDTarget());
            l_Map->SendToSet(l_Packet.m_Packet, m_Target);
        }
        if (m_SpellTemplate->GetVisualIDTargetUnder() >= 0)
        {
            PacketPlayAuraVisual l_Packet;
            l_Packet.BuildPacket(false, m_Target->GetType(), m_Target->GetID(), m_CasterType, m_CasterID, true, m_SpellTemplate->GetVisualIDTargetUnder());
            l_Map->SendToSet(l_Packet.m_Packet, m_Target);
        }
    }
}

bool Aura::HasEffect(const eTypeAuraEffect & p_AuraEffect)
{
    for (auto l_AuraEffect : m_AuraEffectList)
    {
        if (l_AuraEffect.second->GetType() == p_AuraEffect)
            return true;
    }
    return false;
}

int64 Aura::GetDuration() const
{
    return m_Duration;
}

Unit* Aura::GetCaster() const
{
    return m_Caster;
}

SpellTemplate* Aura::GetSpellTemplate() const
{
    return m_SpellTemplate;
}

int64 Aura::GetMaxDuration() const
{
    return (int64)m_SpellTemplate->GetDuration() * 1000;
}

void Aura::Update(sf::Time p_Time)
{
    if (GetDuration() < 0)
        return;

    uint64 l_OldDuration = GetDuration();
    uint64 l_NewDuration = l_OldDuration;
    if (p_Time.asMicroseconds() > GetDuration())
        SetDuration(0);
    else
        SetDuration(m_Duration - p_Time.asMicroseconds(), true);
    l_NewDuration = GetDuration();
    for (std::map<uint8, AuraEffect*>::iterator l_It = m_AuraEffectList.begin(); l_It != m_AuraEffectList.end(); l_It++)
    {
        (*l_It).second->Update(l_OldDuration, l_NewDuration);
    }
}

void Aura::SetDuration(const uint64 & p_NewDuration, bool p_Natual /* = false*/ )
{
    if (!p_Natual)
        ; /// Sebd to player to show it
    m_Duration = p_NewDuration;
}

void Aura::AddDuration(const uint64 & p_Duration)
{
    SetDuration(m_Duration + p_Duration);
}

void Aura::SubDuration(const uint64 & p_Duration)
{
    if (p_Duration > m_Duration)
        SetDuration(0);
    else
        SetDuration(m_Duration - p_Duration);
}

AuraEffect* Aura::AddAuraEffect(const uint8 & p_ID, const eTypeAuraEffect & p_AuraEffectTpe, const int32 & p_Data0, const int32 & p_Data1, const int32 & p_Data2)
{
    AuraEffect* l_AuraEffect = new AuraEffect(this, m_SpellTemplate, m_Caster, m_Target, p_AuraEffectTpe, p_Data0, p_Data1, p_Data2);
    m_AuraEffectFunc l_Fun = m_AuraEffectsMap[p_AuraEffectTpe];
    if (m_AuraEffectList.find(p_ID) != m_AuraEffectList.end())
    {
        (this->*(l_Fun))(m_AuraEffectList[p_ID], false);
        delete m_AuraEffectList[p_ID];
    }
    m_AuraEffectList[p_ID] = l_AuraEffect;

    (this->*(l_Fun))(l_AuraEffect, true);

    return l_AuraEffect;
}

std::vector<AuraEffect*> Aura::GetAuraEffectType(const eTypeAuraEffect & p_TypeAura)
{
    std::vector<AuraEffect*> l_ListAuraEffect;
    for (std::map<uint8, AuraEffect*>::iterator l_It = m_AuraEffectList.begin(); l_It != m_AuraEffectList.end(); l_It++)
    {
        if ((*l_It).second->GetType() == p_TypeAura)
            l_ListAuraEffect.push_back((*l_It).second);
    }
    return l_ListAuraEffect;
}

void Aura::AuraEffectUpdateSpeed(AuraEffect* p_AuraEffect, bool p_Apply)
{
    int32 l_Amount = p_AuraEffect->GetAmount();
    Unit* l_Target = p_AuraEffect->GetTarget();

    if (l_Target == nullptr)
        return;

    if (p_Apply)
        l_Target->SetSpeed(l_Target->GetSpeed() + (float)l_Amount / 10);
    else
        l_Target->SetSpeed(l_Target->GetSpeed() - (float)l_Amount / 10);
}

void Aura::AuraEffectPeriodicHeal(AuraEffect* p_AuraEffect, bool p_Apply)
{
    ;
}

void Aura::AuraEffectPeriodicDamage(AuraEffect* p_AuraEffect, bool p_Apply)
{
    ;
}

void Aura::AuraEffectModifyDamagePct(AuraEffect* p_AuraEffect, bool p_Apply)
{
    ;
}

void Aura::AuraEffectMount(AuraEffect* p_AuraEffect, bool p_Apply)
{
    Unit* l_Target = p_AuraEffect->GetTarget();
    if (l_Target == nullptr)
        return;


    int16 m_SkinMount = p_AuraEffect->GetAmount();
    if (!p_Apply)
        m_SkinMount = -1;

    PacketUnitMount l_Packet;
    l_Packet.BuildPacket(l_Target->GetType(), l_Target->GetID(), m_SkinMount);
    l_Target->GetMap()->SendToSet(l_Packet.m_Packet, l_Target);
}

TypeUnit Aura::GetCasterType() const
{
    return m_CasterType;
}


uint16 Aura::GetCasterID() const
{
    return m_CasterID;
}
