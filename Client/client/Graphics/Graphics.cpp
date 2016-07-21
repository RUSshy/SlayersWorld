#include "Graphics.hpp"

Graphics::Graphics(MapManager* p_MapManager, Events* p_Events) :
	m_MapManager(p_MapManager),
    m_Events(p_Events),
	m_ThreadDraw(&Graphics::UpdateWindow, this),
    m_Run(true)
{
	m_TileSet = nullptr;
    m_SkinsManager = nullptr;
}


Graphics::~Graphics()
{
}

void Graphics::CreateWindow(uint32 p_X, uint32 p_Y, float p_Zoom)
{
	m_Window.create(sf::VideoMode(p_X, p_Y), NAME_WINDOW);
	m_View = m_Window.getDefaultView();
	m_View.zoom(p_Zoom);
	m_Window.setView(m_View);

	m_TileSet = new TileSet();
	m_TileSet->BuildSprites();

    m_SkinsManager = new SkinsManager();
    m_SkinsManager->LoadSkins();
}

void Graphics::Run()
{
	m_ThreadDraw.launch();
}

void Graphics::CheckEvent()
{
	sf::Event l_Event;
	while (m_Window.pollEvent(l_Event))
	{
        switch(l_Event.type)
        {
            case sf::Event::Closed: ///< Request for closing the window
                m_Window.close();
                break;
            case sf::Event::KeyPressed: ///< Key Press
                m_Events->NewKeyPressed(l_Event.key.code);
                break;
            case sf::Event::KeyReleased: ///< Key Release
                m_Events->KeyRelease(l_Event.key.code);
                break;
            case sf::Event::Resized: ///< Resize Window
                /*
                std::cout << "new width: " << event.size.width << std::endl;
                std::cout << "new height: " << event.size.height << std::endl;
                */
                break;
            default:
                break;
        }
	}
}

bool Graphics::WindowIsOpen() const
{
	return m_Window.isOpen();
}

void Graphics::DrawMap()
{
	if (!m_Window.isOpen() || !m_MapManager->HasMap())
		return;

	Map* l_Map = m_MapManager->GetActualMap();
    Player* l_MainPlayer = m_MapManager->GetMainPlayer();

	std::vector<std::vector<Case*>> l_SquareZone = l_Map->GetSquareZone(l_Map->GetSquareID(m_MapManager->GetPosX() / TILE_SIZE, m_MapManager->GetPosY() / TILE_SIZE));
	//printf("Square Acutal = %d\n", l_Map->GetSquareID(m_MapManager->GetPosX() / TILE_SIZE, m_MapManager->GetPosY() / TILE_SIZE));
	if (l_SquareZone.empty())
		return;

    /// First two Level
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
                l_TileSprite->setPosition((float)(*l_It2)->GetPosX() * TILE_SIZE, (float)(*l_It2)->GetPosY() * TILE_SIZE);
                m_Window.draw(*l_TileSprite);
            }
		}

        /// Draw Entities
        if (l_MainPlayer != nullptr)
        {
            uint8 l_SpriteNb = (m_Events->GetMovementHandler()->GetActualOrientation() * MAX_MOVEMENT_POSITION) + m_Events->GetMovementHandler()->GetMovementPosition();
            SkinSprite* l_SkinSprite = m_SkinsManager->GetSkinSprite(l_MainPlayer->GetSkinID(), l_SpriteNb);
            l_SkinSprite->setPosition((float)m_MapManager->GetPosX(), (float)m_MapManager->GetPosY());
            m_Window.draw(*l_SkinSprite);
        }
	}
}

void Graphics::UpdateWindow()
{
    while (m_Run)
    {
        m_View.setCenter((float)m_MapManager->GetPosX(), (float)m_MapManager->GetPosY());
        m_Window.setView(m_View);
        Clear();
        DrawMap();
        Display();
    }
}

void Graphics::End()
{
	m_Run = false;
}

void Graphics::Clear()
{
	m_Window.clear();
}

void Graphics::Display()
{
	m_Window.display();
}