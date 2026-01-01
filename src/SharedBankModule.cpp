#include "Module.h"
#include "Player.h"
#include "DatabaseEnv.h"
#include "WorldPacket.h"

class SharedBankModule : public Module {
public:
    SharedBankModule() : Module("SharedBank") {}

    // Hook: On spell cast (assume hook exists; adapt to actual OnCastSpell).
    bool OnCastSpell(Player* player, Spell* spell) override {
        if (spell->GetSpellInfo()->Id == 90000) {
            OpenSharedBank(player);
            return true;  // Consume cast.
        }
        return false;
    }

    // Hook: On item move (adapt for inventory <-> bank).
    void OnItemMove(Player* player, Item* item, uint8 srcBag, uint8 srcSlot, uint8 dstBag, uint8 dstSlot) override {
        if (dstBag == BANK_BAG_SHARED) {  // Custom bag ID.
            SaveItemToAccountBank(player->GetSession()->GetAccountId(), dstSlot, item->GetGUIDLow());
        } else if (srcBag == BANK_BAG_SHARED) {
            RemoveItemFromAccountBank(player->GetSession()->GetAccountId(), srcSlot);
        }
    }

private:
    void OpenSharedBank(Player* player) {
        // Send bank open packet with custom slots (use SMSG_SHOW_BANK, custom GUID for "banker").
        WorldPacket data(SMSG_SHOW_BANK, 8);
        data << uint64(0xDEADBEEF);  // Custom GUID.
        player->GetSession()->SendPacket(&data);

        // Load items from DB and send to client (SMSG_BANK_UPDATE).
        QueryResult result = CharacterDatabase.PQuery("SELECT item_guid FROM account_bank WHERE account_id = %u", player->GetSession()->GetAccountId());
        if (result) {
            do {
                uint32 slot = result->Fetch()[0].GetUInt32();  // Adapt.
                // Send item info packet.
            } while (result->NextRow());
        }
    }

    void SaveItemToAccountBank(uint32 accountId, uint8 slot, uint32 itemGuid) {
        CharacterDatabase.PExecute("REPLACE INTO account_bank (account_id, slot, item_guid) VALUES (%u, %u, %u)", accountId, slot, itemGuid);
    }

    void RemoveItemFromAccountBank(uint32 accountId, uint8 slot) {
        CharacterDatabase.PExecute("UPDATE account_bank SET item_guid = NULL WHERE account_id = %u AND slot = %u", accountId, slot);
    }
};

static SharedBankModule sharedBankModule;