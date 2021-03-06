#pragma once
#include "../TileSprite.hpp"
#include "../../Define.hpp"
#include <string>
#include <vector>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Time.hpp>

class HistoryField
{
public:
    HistoryField();
    ~HistoryField();
    void Update(sf::Time);
    void OpenTemporary(uint32);
    void Open();
    void Close();
    bool IsFieldOpen();
    void AddHistoryLine(const SWText &);
    sf::Text GetText() const;
    std::vector<SWText> GetHistory() const;
    uint8 GetLineHistory() const;
    void SetLineHistory(uint8);

private:
    sf::Text m_Text;
    bool m_IsOpen;
    std::vector<SWText> m_History;
    uint8 m_HistoryMaxLine;
    int64 m_DiffTimeOpen;
};