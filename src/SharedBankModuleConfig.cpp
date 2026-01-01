#include "SharedBankModuleConfig.h"
#include "Config.h"

SharedBankModuleConfig::SharedBankModuleConfig()
    : ModuleConfig("SharedBank"),
      Enabled(true),
      SpellId(90000),
      BankSlots(28) {}

void SharedBankModuleConfig::LoadConfig() {
    Enabled = sConfigMgr->GetBoolDefault("SharedBank.Enabled", true);
    SpellId = sConfigMgr->GetIntDefault("SharedBank.SpellId", 90000);
    BankSlots = sConfigMgr->GetIntDefault("SharedBank.Slots", 28);
}