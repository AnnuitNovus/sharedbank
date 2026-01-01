#include "SharedBankModule.h"
#include "Config/Config.h"
#include "Database/DatabaseEnv.h"
#include "Server/WorldPacket.h"
#include "Server/Opcodes.h"
#include <cstdint>

namespace cmangos_module
{

    constexpr uint8 BANK_BAG_SHARED = 100;

    SharedBankModule::SharedBankModule()
        : Module("SharedBank", new SharedBankModuleConfig()) {}

    const SharedBankModuleConfig* SharedBankModule::GetConfig() const
    {
        return static_cast<const SharedBankModuleConfig*>(Module::GetConfig());
    }

    bool SharedBankModule::OnCastSpell(Player* player, Spell* spell)
    {
        // implementation
        return false;
    }

    void SharedBankModule::OnItemMove(Player* player, Item* item, uint8 srcBag, uint8 srcSlot, uint8 dstBag, uint8 dstSlot)
    {
        // implementation
    }

    void SharedBankModule::OpenSharedBank(Player* player)
    {
        WorldPacket data(SMSG_SHOW_BANK);
        data << uint64(0xDEADBEEFDEADBEEFULL);
        player->GetSession()->SendPacket(data);
    }

} // namespace cmangos_module

cmangos_module::SharedBankModule sharedBankModule;
