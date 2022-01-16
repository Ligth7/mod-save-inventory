/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */

#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"
#include "Tokenize.h"

// Add player scripts
class ModSaveInventoryPlayerScript : public PlayerScript
{
public:
    ModSaveInventoryPlayerScript() : PlayerScript("ModSaveInventoryPlayerScript") { }

    void OnLootItem(Player* player, Item* item, uint32 /*count*/, ObjectGuid /*lootguid*/)
    {
        if (ShouldSaveItem(item))
        {
            CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
            player->SaveInventoryAndGoldToDB(trans);
            CharacterDatabase.CommitTransaction(trans);
        }
    }

    bool ShouldSaveItem(Item* item)
    {
        if (!sConfigMgr->GetOption<bool>("ModSaveInventory.Enable", true))
        {
            return false;
        }

        if (item->GetTemplate()->Quality >= sConfigMgr->GetOption<uint8>("ModSaveInventory.MinItemQuality", ITEM_QUALITY_UNCOMMON))
        {
            return true;
        }

        std::string exceptions = sConfigMgr->GetOption<std::string>("ModSaveInventory.AlwaysSaveList", "");

        std::vector<std::string_view> tokens = Acore::Tokenize(exceptions, ' ', false);

        for (auto token : tokens)
        {
            if (token.empty())
            {
                continue;
            }

            if (item->GetEntry() == Acore::StringTo<uint32>(token).value())
            {
                return true;
            }
        }

        return false;
    }
};

void AddSaveInventoryScript()
{
    new ModSaveInventoryPlayerScript();
}