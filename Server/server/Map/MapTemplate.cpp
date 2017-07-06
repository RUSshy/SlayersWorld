﻿#include "MapTemplate.hpp"
#include <ctime>
#include <cstdlib>

MapTemplate::MapTemplate() :
    m_ID(0),
    m_TypeMap(eTypeMap::MapWorld),
    m_MaxPlayer(0),
    m_Name("Default"),
    m_FileName("Default"),
    m_FileChipset("Default")
{
    m_SizeX = 0;
    m_SizeY = 0;
    printf("Enter 2\n");
}

MapTemplate::MapTemplate(uint16 p_ID, eTypeMap p_Type, uint16 p_MaxPlayer, const std::string & p_Name, const std::string & p_FileName, const std::string & p_FileChipset) :
    m_ID(p_ID),
    m_TypeMap(p_Type),
    m_MaxPlayer(p_MaxPlayer),
    m_Name(p_Name),
    m_FileName(p_FileName),
    m_FileChipset(p_FileChipset)
{
    m_SizeX = 0;
    m_SizeY = 0;
}

MapTemplate::~MapTemplate()
{
}

eTypeMap MapTemplate::GetType() const
{
    return m_TypeMap;
}

std::string MapTemplate::GetFileName() const
{
    return m_FileName;
}

std::string MapTemplate::GetFileChipset() const
{
    return m_FileChipset;
}

std::string MapTemplate::GetName() const
{
    return m_Name;
}

uint16 MapTemplate::GetMaxPlayer() const
{
    return m_MaxPlayer;
}

uint16 MapTemplate::GetID() const
{
    return m_ID;
}

void MapTemplate::AddGobMapTemplate(GobMapTemplate p_Gob)
{
    m_ListGoB.push_back(p_Gob);
}

void MapTemplate::AddAreaTriggerMapTemplate(AreaTriggerMapTemplate p_Area)
{
    m_ListArea.push_back(p_Area);
}

void MapTemplate::AddCreatureMapTemplate(CreatureMapTemplate p_Creature)
{
    m_ListCreature.push_back(p_Creature);
}

void MapTemplate::AddZone(Zone p_Zone)
{
    m_ListZone.push_back(p_Zone);
}

uint16 MapTemplate::GetSizeX() const
{
    return m_SizeX;
}

uint16 MapTemplate::GetSizeY() const
{
    return m_SizeY;
}

std::vector<Case>*	MapTemplate::GetListCase()
{
    return &m_ListCase;
}

std::vector<Zone>*	MapTemplate::GetListZone()
{
    return &m_ListZone;
}

std::vector<GobMapTemplate>* MapTemplate::GetListGob()
{
    return &m_ListGoB;
}

std::vector<AreaTriggerMapTemplate>* MapTemplate::GetListArea()
{
    return &m_ListArea;
}

std::vector<CreatureMapTemplate>* MapTemplate::GetListCreature()
{
    return &m_ListCreature;
}

bool MapTemplate::InitializeMap()
{
    FILE * l_File;
    std::string l_MapPath = MAP_PATH + m_FileName;

    l_File = fopen(l_MapPath.c_str(), "r");
    if (l_File == nullptr)
    {
        printf("Map File %s did not open\n", l_MapPath.c_str());
        return false;
    }

    t_MapParam l_Param;
    fread(&l_Param, sizeof(l_Param), 1, l_File);
    m_ID = l_Param.l_ID;
    m_SizeX = l_Param.l_Size[0];
    m_SizeY = l_Param.l_Size[1];

    /// Read Map
    for (uint32 i = 0; i < (m_SizeX * m_SizeY); ++i)
    {
        t_Case l_FluxCase;
        fread(&l_FluxCase, sizeof(l_FluxCase), 1, l_File);
        Case l_Case = Case(i, i % m_SizeX, i / m_SizeX);

        l_Case.SetMapID(m_ID);

        bool l_Block = l_FluxCase.l_Block;
        if (l_Block)
            l_Case.SetBlock(true);
        m_ListCase.push_back(l_Case);
    }

    return true;
}

Case* MapTemplate::GetCase(uint32 p_ID)
{
    if (p_ID >= m_ListCase.size())
        return nullptr;

    return &m_ListCase[p_ID];
}