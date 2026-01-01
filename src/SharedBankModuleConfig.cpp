#include "SharedBankModuleConfig.h"
#include "Config/Config.h"

namespace cmangos_module
{

    SharedBankModuleConfig::SharedBankModuleConfig()
        : ModuleConfig("sharedbank.conf") {}

    bool SharedBankModuleConfig::OnLoad()
    {
        Enabled = config.GetBoolDefault("SharedBank.Enabled", true);
        SpellId = config.GetIntDefault("SharedBank.SpellId", 90000);
        BankSlots = config.GetIntDefault("SharedBank.Slots", 28);
        return true;
    }

} // namespace cmangos_module
