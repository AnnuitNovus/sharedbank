#ifndef SHAREDBANKMODULE_H
#define SHAREDBANKMODULE_H

#include "Entities/Item.h"
#include "Entities/Player.h"
#include "Module.h"
#include "SharedBankModuleConfig.h"
#include "Spells/Spell.h"
#include <cstdint>

namespace cmangos_module
{

    class SharedBankModule : public Module
    {
    public:
        SharedBankModule();
        const SharedBankModuleConfig* GetConfig() const;

        bool OnCastSpell(Player* player, Spell* spell);
        void OnItemMove(Player* player, Item* item, uint8 srcBag, uint8 srcSlot, uint8 dstBag, uint8 dstSlot);
    private:
        void OpenSharedBank(Player* player);
    };

    extern SharedBankModule sharedBankModule;

} // namespace cmangos_module
#endif
