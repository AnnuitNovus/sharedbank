#ifndef SHAREDBANKMODULECONFIG_H
#define SHAREDBANKMODULECONFIG_H

#include "ModuleConfig.h"

class SharedBankModuleConfig : public ModuleConfig {
public:
    bool Enabled;
    uint32 SpellId;
    uint8 BankSlots;

    SharedBankModuleConfig();
    void LoadConfig() override;
};

#endif