#include "SharedBankModule.h"
#include "Config/Config.h"
#include "Database/DatabaseEnv.h"
#include "Entities/ItemPrototype.h"
#include "Globals/ObjectMgr.h"
#include "Server/Opcodes.h"
#include "Server/WorldPacket.h"
#include <map>
#include <vector>

namespace cmangos_module
{

    constexpr uint8 BANK_BAG_SHARED = 100;
    const uint64 FAKE_GUID_RAW = 0xDEADBEEFDEADBEEFULL;

    static std::map<uint32, std::vector<Item*>> personalBankItems;

    SharedBankModule::SharedBankModule()
        : Module("SharedBank", new SharedBankModuleConfig()) {}

    const SharedBankModuleConfig* SharedBankModule::GetConfig() const
    {
        return static_cast<const SharedBankModuleConfig*>(Module::GetConfig());
    }

    bool SharedBankModule::OnCastSpell(Player* player, Spell* spell)
    {
        if (!GetConfig()->Enabled || spell->m_spellInfo->Id != GetConfig()->SpellId)
            return false;

        if (personalBankItems.find(player->GetGUIDLow()) != personalBankItems.end())
        {
            SaveSharedBank(player);
            RestorePersonalBank(player);
        }
        else
        {
            SavePersonalBank(player);
            LoadSharedBank(player);
            OpenSharedBank(player);
        }
        return true;
    }

    void SharedBankModule::OnItemMove(Player* player, Item* item, uint8 srcBag, uint8 srcSlot, uint8 dstBag, uint8 dstSlot)
    {
        // No extra logic needed
    }

    void SharedBankModule::OpenSharedBank(Player* player)
    {
        WorldPacket data(SMSG_SHOW_BANK);
        ObjectGuid fake(FAKE_GUID_RAW);
        data << fake;
        player->GetSession()->SendPacket(data);
    }

    void SharedBankModule::SavePersonalBank(Player* player)
    {
        auto& temp = personalBankItems[player->GetGUIDLow()];
        temp.resize(GetConfig()->BankSlots, nullptr);

        for (uint8 slot = BANK_SLOT_ITEM_START; slot < BANK_SLOT_ITEM_START + GetConfig()->BankSlots; ++slot)
        {
            Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
            if (item)
            {
                player->RemoveItem(INVENTORY_SLOT_BAG_0, slot, true);
                item->SetGuidValue(ITEM_FIELD_OWNER, ObjectGuid());
                item->SetGuidValue(ITEM_FIELD_CONTAINED, ObjectGuid());
                item->SetSlot(NULL_SLOT);
            }
            temp[slot - BANK_SLOT_ITEM_START] = item;
        }
    }

    void SharedBankModule::LoadSharedBank(Player* player)
    {
        uint32 accountId = player->GetSession()->GetAccountId();
        auto query = CharacterDatabase.PQuery("SELECT item_guid FROM account_bank WHERE account_id = %u ORDER BY slot", accountId);
        QueryResult* result = query.get();
        if (!result)
            return;

        uint8 slot = BANK_SLOT_ITEM_START;
        do
        {
            Field* fields = result->Fetch();
            uint32 guidLow = fields[0].GetUInt32();
            if (guidLow == 0)
            {
                ++slot;
                continue;
            }

            auto itemQuery = CharacterDatabase.PQuery("SELECT itemEntry, creatorGuid, giftCreatorGuid, count, duration, charges, flags, enchantments, randomPropertyId, durability, itemTextId FROM item_instance WHERE guid = %u", guidLow);
            QueryResult* itemResult = itemQuery.get();
            if (!itemResult)
            {
                ++slot;
                continue;
            }

            Field* itemFields = itemResult->Fetch();

            uint32 entry = itemFields[0].GetUInt32();

            ItemPrototype const* proto = ObjectMgr::GetItemPrototype(entry);
            if (!proto)
            {
                delete itemResult;
                ++slot;
                continue;
            }

            Item* item = NewItemOrBag(proto);
            if (item->LoadFromDB(ObjectGuid(HIGHGUID_ITEM, guidLow), &itemFields[1], ObjectGuid(HIGHGUID_ITEM, entry)))
            {
                uint16 dest = (INVENTORY_SLOT_BAG_0 << 8) | slot;
                ItemPosCountVec destVec;
                destVec.push_back(ItemPosCount(dest, item->GetCount()));
                player->StoreItem(destVec, item, true);
            }
            else
            {
                delete item;
            }
            delete itemResult;
            ++slot;
        } while (result->NextRow() && slot < BANK_SLOT_ITEM_START + GetConfig()->BankSlots);

        delete result;
    }

    void SharedBankModule::SaveSharedBank(Player* player)
    {
        uint32 accountId = player->GetSession()->GetAccountId();
        CharacterDatabase.BeginTransaction();
        CharacterDatabase.PExecute("DELETE FROM account_bank WHERE account_id = %u", accountId);

        uint8 idx = 0;
        for (uint8 slot = BANK_SLOT_ITEM_START; slot < BANK_SLOT_ITEM_START + GetConfig()->BankSlots; ++slot)
        {
            Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
            uint32 guidLow = 0;
            if (item)
            {
                item->SetGuidValue(ITEM_FIELD_OWNER, ObjectGuid());
                item->SaveToDB();
                guidLow = item->GetGUIDLow();
            }
            CharacterDatabase.PExecute("INSERT INTO account_bank (account_id, slot, item_guid) VALUES (%u, %u, %u)", accountId, idx, guidLow);
            ++idx;
        }
        CharacterDatabase.CommitTransaction();
    }

    void SharedBankModule::RestorePersonalBank(Player* player)
    {
        auto it = personalBankItems.find(player->GetGUIDLow());
        if (it == personalBankItems.end())
            return;

        std::vector<Item*>& temp = it->second;
        for (uint8 slot = BANK_SLOT_ITEM_START; slot < BANK_SLOT_ITEM_START + GetConfig()->BankSlots; ++slot)
        {
            player->DestroyItem(INVENTORY_SLOT_BAG_0, slot, true);
            Item* item = temp[slot - BANK_SLOT_ITEM_START];
            if (item)
            {
                item->SetGuidValue(ITEM_FIELD_OWNER, ObjectGuid(HIGHGUID_PLAYER, (uint32)player->GetGUID()));
                item->SetGuidValue(ITEM_FIELD_CONTAINED, ObjectGuid(HIGHGUID_PLAYER, (uint32)player->GetGUID()));
                uint16 dest = (INVENTORY_SLOT_BAG_0 << 8) | slot;
                ItemPosCountVec destVec;
                destVec.push_back(ItemPosCount(dest, item->GetCount()));
                player->StoreItem(destVec, item, true);
            }
        }
        personalBankItems.erase(it);
    }

} // namespace cmangos_module

cmangos_module::SharedBankModule sharedBankModule;
