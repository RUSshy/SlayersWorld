#pragma once
#include <SFML/Graphics/Texture.hpp>
#include "../TileSprite.hpp"
#include "../../Define.hpp"
#include "../Window.hpp"
#include "../../World/Events/Events.hpp"
#include "QuestionBox.hpp"
#include "MenuManager.hpp"
#include "WritingField.hpp"
#include "HistoryField.hpp"

class InterfaceManager
{
public:
    InterfaceManager(Events*);
    ~InterfaceManager();

    void Initialize();
    void InitializeWarningMsgs();
    void Update(sf::Time);
    void ManageEvent(sf::Event);
    TileSprite GetField(uint16, uint16);
    void DrawField(Window &, const float &, const float &, uint16, uint16, bool p_WithBorder = true);
    void DrawBorderField(Window &, const float &, const float &, uint16, uint16);
    TileSprite GetLifeBar(uint8, uint8 p_Pct = 100);
    TileSprite GetXpBar();
    TileSprite GetBottomBar();
    TileSprite GetCastBar(bool, uint8 p_Pct = 100);
    void Draw(Window &);
    void DrawStartingPage(Window &);
    void DrawWarnings(Window &);
    void DrawTopRightMessage(Window &);
    void DrawClock(Window &);
    void DrawAlign(Window &);
    void DrawExtraUI(Window &);
    void DrawBGUI(Window &);
    void DrawSpellBind(Window &);

    HistoryField* GetHistoryField() const;
    Menu* GetMenuInterface(const eMenuType &);
    std::vector<Menu*> GetOpenInterfaceMenu();
    void AddWarningMsg(eTypeWarningMsg, const std::string &);
    void AddTopMsg(const std::string &);
    void SetBigMsg(const std::string &);
    void AddSimpleQuestion(const uint16 &, const std::string &);
    void RemoveFirstTopMsg();
    void AddWarningMsg(eTypeWarningMsg, eWarningMsg);
    void AddKeyBind(uint8, uint8);
    int16 GetBindForKey(uint8);
    void SetSystemMsg(const std::string &);
    void AddBlockingBind(uint8, uint16);
    bool IsBlockingBind(uint8);
    sf::Vector2i TextSplitToFit(uint16, sf::Text &);
    void DrawMenu(Window &, Menu *);
    MenuManager* GetMenuManager();
    bool IsLoading() const;
    void SetIsLoading(bool);
    void SetVisualManager(VisualManager*);
    void AddExtraInterface(eExtraInterface);
    void RemoveExtraInterface(eExtraInterface);
    void AddExtraUiData(eExtraInterface, const uint8 &, const uint8 &, const int16 &);
    void AddSpellBind(const uint8 &, const uint16 &);

private:
    sf::Texture                         m_SystemTexture;
    sf::Texture                         m_SepBarTexture;
    sf::Texture                         m_LifeBarTexture;
    sf::Texture                         m_XpTexture;
    sf::Texture                         m_BarBottom;
    sf::Texture                         m_CastBarTexture;
    sf::Texture                         m_Background;
    sf::Texture                         m_BGInterface;
    Events*                             m_Events;
    WritingField*                       m_WritingField;
    HistoryField*                       m_HistoryField;
    sf::Text                            m_SystemMsg;
    sf::Text                            m_ClockTxt;
    sf::Text                            m_Align;
    std::vector<std::string >           m_TopMessages;
    std::map< eTypeWarningMsg, std::vector< std::pair<std::string, uint32> > >     m_WarningMsgs;
    std::map< eTypeWarningMsg, std::vector< std::pair<std::string, uint32> > >     m_TopRightMsgs;
    std::map< eWarningMsg, std::string > m_WarningMsgsEnum;
    std::map< uint8, uint8 >             m_KeyBoardBind;
    std::map< uint8, uint64 >            m_BlockingBinds;
    std::vector<eExtraInterface>         m_ExtraUI;
    std::map < eExtraInterface, std::map< uint8,  std::pair<uint8, uint64>>> m_ExtraUIData;
    std::map<uint16, QuestionBox>        m_ListSimpleQuestion;
    std::pair<std::string , uint64>      m_BigMessage;
    std::map<eMenuType, Menu*>           m_InterfaceMenuList;
    std::map <uint8, uint16>             m_SpellBinding;

    VisualManager*                       m_VisualManager;
    MenuManager                          m_MenuManager;
    bool                                 m_IsLoading;
};