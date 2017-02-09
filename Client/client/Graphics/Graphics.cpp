#include "Graphics.hpp"
#include "../Global.hpp"

Graphics::Graphics(MapManager* p_MapManager, InterfaceManager* p_InterfaceManager, Events* p_Events) :
    m_MapManager(p_MapManager),
    m_InterfaceManager(p_InterfaceManager),
    m_Events(p_Events)
{
	m_TileSet = nullptr;
    m_VisualManager = nullptr;
    m_Clock = new ClockHandler();
}

Graphics::~Graphics()
{
}

bool Graphics::LoadFont()
{
    std::string l_FontName = "arial.ttf";
    g_Font = new sf::Font();
    if (!g_Font->loadFromFile(FONT_FOLDER + l_FontName))
        return false;
    return true;
}

bool Graphics::CreateWindow(uint32 p_X, uint32 p_Y, float p_Zoom)
{
	m_Window.create(sf::VideoMode(p_X, p_Y), NAME_WINDOW);
	m_View = m_Window.getDefaultView();
    m_ViewFont = m_Window.getDefaultView();
    m_ViewInterface = m_Window.getDefaultView();
	m_View.zoom(p_Zoom);
	m_Window.setView(m_View);
    //m_Window.setFramerateLimit(40);
    m_Window.setVerticalSyncEnabled(true);

	m_TileSet = new TileSet();
	m_TileSet->BuildSprites();

    m_VisualManager = new VisualManager();
    if (!m_VisualManager->LoadSkins())
        return false;
    if (!m_VisualManager->LoadVisuals())
        return false;
    return true;
}

void Graphics::CheckEvent()
{
	sf::Event l_Event;
    m_Events->Update();
	while (m_Window.pollEvent(l_Event))
	{
        switch(l_Event.type)
        {
            case sf::Event::Closed: ///< Request for closing the window
                m_Window.close();
                break;
           
            case sf::Event::Resized: ///< Resize Window
                /*
                std::cout << "new width: " << event.size.width << std::endl;
                std::cout << "new height: " << event.size.height << std::endl;
                */
                break;
                /// Reset KeyPress queue when lost focus
            default:
                m_InterfaceManager->ManageEvent(l_Event);
                break;
        }
	}
}

bool Graphics::WindowIsOpen() const
{
	return m_Window.isOpen();
}

Position Graphics::GetCenterPositionOnUnit(Unit* p_Unit, sf::Sprite* p_Sprite)
{
    int16 l_DiffSizeX = (p_Sprite->getTextureRect().width - p_Unit->GetSizeX()) / 2;
    int16 l_DiffSizeY = (p_Sprite->getTextureRect().height - p_Unit->GetSizeY()) / 2;

    Position l_Pos;
    l_Pos.x = p_Unit->GetPosX() - l_DiffSizeX;
    l_Pos.y = p_Unit->GetPosY() - l_DiffSizeY;
    return l_Pos;
}

void Graphics::DrawUnitDetails(Unit* p_Unit)
{
    if (p_Unit == nullptr)
        return;

    std::vector<VisualEffect>  *l_VisualsEffect = p_Unit->GetVisualsEffect();

    /// VISUAL EFFECT
    for (std::vector<VisualEffect>::iterator l_It = l_VisualsEffect->begin(); l_It != l_VisualsEffect->end(); ++l_It)
    {
        SkinSprite* l_SkinSprite = m_VisualManager->GetVisualSprite((*l_It).GetType(), (*l_It).GetID(), (*l_It).GetFrame());
        if (l_SkinSprite)
        {
            l_SkinSprite->setScale(sf::Vector2f(p_Unit->GetSkinZoomFactor(), p_Unit->GetSkinZoomFactor()));
            Position l_Pos = GetCenterPositionOnUnit(p_Unit, l_SkinSprite);
            l_SkinSprite->setPosition(l_Pos.x, l_Pos.y - p_Unit->GetSizeY());
            m_Window.draw(*l_SkinSprite);
        }
    }

    /// Set view to don t have a zoom on text
    m_Window.setView(m_ViewFont);

    std::vector<std::pair<DamageInfo, uint32>> l_DamageLogHistory = p_Unit->GetDamageLog();
    /// LOG DAMAGE
    for (std::pair<DamageInfo, uint32> l_DamageLog : l_DamageLogHistory)
    {
        int8 l_LogToShow = l_DamageLog.first.m_Damage;
        if (l_LogToShow < 0)
            l_LogToShow *= -1;
        std::string l_DmgStr = l_DamageLog.first.m_Result == DamageResult::Miss ? "Miss" : std::to_string(l_LogToShow);
        sf::Text l_Text(l_DmgStr, *g_Font, SIZE_TALK_FONT);

        if (l_DamageLog.first.m_Damage >= 0)
            l_Text.setColor(sf::Color(255, 66, 66));
        else
            l_Text.setColor(sf::Color(164, 255, 6));
        sf::Vector2f v1(p_Unit->GetPosX() + (p_Unit->GetSizeX() / 2), p_Unit->GetPosY() - p_Unit->GetSizeY() - 6 - 4 - ((MAX_HISTORY_LOG_TIME - l_DamageLog.second) / 100000));
        sf::Vector2f l_Coord = m_Window.mapCoordsToPixelFloat(v1, m_View);
        l_Text.setPosition((l_Coord.x - (l_Text.getGlobalBounds().width / 2)), l_Coord.y);
        m_Window.draw(l_Text);
    }
    /// TALK
    if (!p_Unit->GetTalk().empty())
    {
        sf::Text l_Text(p_Unit->GetTalk(), *g_Font, SIZE_TALK_FONT);

        TileSprite l_Sprite = m_InterfaceManager->GetField(l_Text.getGlobalBounds().width + 8, (float)g_Font->getLineSpacing(l_Text.getCharacterSize()) + 8);
        sf::Vector2f v1(p_Unit->GetPosX() + (p_Unit->GetSizeX() / 2), p_Unit->GetPosY() - p_Unit->GetSizeY() - 6 - 4);
        sf::Vector2f l_Coord = m_Window.mapCoordsToPixelFloat(v1, m_View);
        l_Sprite.setPosition((l_Coord.x - ((l_Text.getGlobalBounds().width + 8) / 2)), l_Coord.y);
        m_Window.draw(l_Sprite);

        l_Text.setColor(sf::Color::White);
        sf::Vector2f v12(p_Unit->GetPosX() + (p_Unit->GetSizeX() / 2), p_Unit->GetPosY() - p_Unit->GetSizeY());
        l_Coord = m_Window.mapCoordsToPixelFloat(v1, m_View);
        l_Text.setPosition((l_Coord.x - (l_Text.getGlobalBounds().width / 2)), l_Coord.y);
        m_Window.draw(l_Text);
    }

    /// NAME
    if (p_Unit->IsPlayer())
    {
        sf::Text l_Name(p_Unit->GetName(), *g_Font, SIZE_NAME_FONT);
        l_Name.setColor(sf::Color::White);
        sf::Vector2f l_View(p_Unit->GetPosX() + (p_Unit->GetSizeX() / 2), p_Unit->GetPosY());
        sf::Vector2f l_Coord = m_Window.mapCoordsToPixelFloat(l_View, m_View);
        l_Name.setPosition((l_Coord.x - (l_Name.getGlobalBounds().width / 2.0f)), l_Coord.y);

        m_Window.draw(l_Name);
    }

    /// Reset the view
    m_Window.setView(m_View);
}

void Graphics::DrawWorldObjects(std::map<uint32, std::vector<WorldObject*> > *p_ListWorldObjectsByZ)
{
    Map* l_Map = m_MapManager->GetActualMap();

    for (std::map<uint32, std::vector<WorldObject*> >::iterator l_It = p_ListWorldObjectsByZ->begin(); l_It != p_ListWorldObjectsByZ->end(); ++l_It)
    {
        for (auto l_WorldObject : (*l_It).second)
        {
            if (l_WorldObject == nullptr)
                continue;

            if (l_WorldObject->GetType() == TypeWorldObject::UNIT)
                l_WorldObject->GetSprite()->setPosition((float)l_WorldObject->GetPosX(), (float)l_WorldObject->GetPosY() - l_WorldObject->GetSizeY());
            else
                l_WorldObject->GetSprite()->setPosition((float)l_WorldObject->GetPosX(), (float)l_WorldObject->GetPosY());
            m_Window.draw(*l_WorldObject->GetSprite());

            /// Draw specificity of Unit (name, text, dmg ...)
            if (l_WorldObject->GetType() == TypeWorldObject::UNIT)
                DrawUnitDetails(l_WorldObject->ToUnit());
        }
    }
}

void Graphics::DrawMap()
{
	if (!m_Window.isOpen() || !m_MapManager->HasMap() || g_Player == nullptr)
		return;

	Map* l_Map = m_MapManager->GetActualMap();
    std::map<TypeUnit, std::map<uint16, Unit*> >* l_ListUnitZone = l_Map->GetListUnitZone();
    std::map<uint32, std::vector<WorldObject*> > l_ListWorldObjectByZ;

	std::vector<std::vector<Case*>> l_SquareZone = l_Map->GetSquareZone(l_Map->GetSquareID(g_Player->GetCasePosX(), g_Player->GetCasePosY()));
	//printf("Square Acutal = %d\n", l_Map->GetSquareID(m_MapManager->GetPosX() / TILE_SIZE, m_MapManager->GetPosY() / TILE_SIZE));
	if (l_SquareZone.empty())
		return;

    /// Level 1 and 2
    /// Level 5 and 6
	for (std::vector<std::vector<Case*>>::iterator l_It = l_SquareZone.begin(); l_It != l_SquareZone.end(); ++l_It)
	{
		std::vector<Case*> l_Square = (*l_It);
		for (std::vector<Case*>::iterator l_It2 = l_Square.begin(); l_It2 != l_Square.end(); ++l_It2)
		{
            for (uint8 l_LevelNb = 0; l_LevelNb < 2; ++l_LevelNb)
            {
                int16 l_TileID = (*l_It2)->GetTile(l_LevelNb);

                if (l_TileID < 0)
                    continue;

                TileSprite* l_TileSprite = m_TileSet->GetTileSprite(l_TileID);
                l_TileSprite->setPosition((float)(*l_It2)->GetPosX(), (float)(*l_It2)->GetPosY());
                m_Window.draw(*l_TileSprite);
            }
            for (uint8 l_LevelNb = 4; l_LevelNb < 6; ++l_LevelNb) ///< Couches specials
            {
                int16 l_TileID = (*l_It2)->GetTile(l_LevelNb);

                if (l_TileID < 0)
                    continue;

                TileSprite* l_TileSprite = m_TileSet->GetTileSprite(l_TileID);
                l_TileSprite->setPosition((float)(*l_It2)->GetPosX(), (float)(*l_It2)->GetPosY());
                (*l_It2)->SetSprite(l_TileSprite);
                if (l_LevelNb == 4) ///< + GetSizeY because tile of Unit are set to be 0 at foot cf : DrawWorldObjects UNIT
                    l_ListWorldObjectByZ[(*l_It2)->GetPosY() + (*l_It2)->GetSizeY()].push_back(*l_It2);
                else
                    l_ListWorldObjectByZ[(*l_It2)->GetPosY() + ((*l_It2)->GetSizeY() * 2)].push_back(*l_It2);
            }
		}
	}
    
    std::map<uint32, std::vector<WorldObject*> > l_ListUnitByZ;
    for (std::pair<TypeUnit, std::map<uint16, Unit*>> l_MapListUnit : (*l_ListUnitZone))
    {
        for (std::pair<uint16, Unit*> l_UnitPair : l_MapListUnit.second)
        {
            Unit* l_Unit = l_UnitPair.second;

            if (l_Unit == nullptr)
                continue;

            MovementHandler* l_MovementHandler = l_Unit->GetMovementHandler();
            uint8 l_SpriteNb = (l_Unit->GetOrientation() * MAX_MOVEMENT_POSITION) + l_MovementHandler->GetMovementPosition();
            if (l_MovementHandler->IsInAttack())
                l_SpriteNb += (MAX_MOVEMENT_POSITION * Orientation::MAX);

            SkinSprite* l_SkinSprite = m_VisualManager->GetVisualSprite(eVisualType::VisualSkin, l_Unit->GetSkinID(), l_SpriteNb);
            l_SkinSprite->setScale(sf::Vector2f(l_Unit->GetSkinZoomFactor(), l_Unit->GetSkinZoomFactor()));
            if (l_Unit->IsPlayer())
                l_SkinSprite->setColor(sf::Color(255, 255, 255, l_Unit->GetOpacity()));
            else
                l_SkinSprite->setColor(sf::Color(255, 255, 255, 255));

            l_Unit->SetSprite(l_SkinSprite);

            l_ListWorldObjectByZ[l_Unit->GetPosY()].push_back(l_Unit);
        }
    }

    DrawWorldObjects(&l_ListWorldObjectByZ);

    /// Level 3 and 4
    for (std::vector<std::vector<Case*>>::iterator l_It = l_SquareZone.begin(); l_It != l_SquareZone.end(); ++l_It)
    {
        std::vector<Case*> l_Square = (*l_It);
        for (std::vector<Case*>::iterator l_It2 = l_Square.begin(); l_It2 != l_Square.end(); ++l_It2)
        {
            for (uint8 l_LevelNb = 2; l_LevelNb < 4; ++l_LevelNb)
            {
                int16 l_TileID = (*l_It2)->GetTile(l_LevelNb);

                if (l_TileID < 0)
                    continue;

                TileSprite* l_TileSprite = m_TileSet->GetTileSprite(l_TileID);
                l_TileSprite->setPosition((float)(*l_It2)->GetPosX(), (float)(*l_It2)->GetPosY());
                m_Window.draw(*l_TileSprite);
            }
        }
    }
}

void Graphics::DrawInterface()
{
    if (!m_Window.isOpen())
        return;

    m_InterfaceManager->Draw(m_Window);
}

void Graphics::UpdateInterface(sf::Time p_Diff)
{
    if (!m_Window.isOpen() || !m_MapManager->HasMap() || g_Player == nullptr)
        return;

    m_InterfaceManager->Update(p_Diff);
}

void Graphics::UpdateWindow(sf::Time p_Diff)
{
    Clear();
    if (g_Player != nullptr)
        m_View.setCenter((float)g_Player->GetPosX() + (g_Player->GetSizeX() / 2), (float)g_Player->GetPosY() - (g_Player->GetSizeY() / 2));
    m_Window.setView(m_View);
    DrawMap();
    m_Window.setView(m_ViewInterface);
    UpdateInterface(p_Diff);
    DrawInterface();
    Display();
}

void Graphics::Clear()
{
	m_Window.clear();
}

void Graphics::Display()
{
	m_Window.display();
}
