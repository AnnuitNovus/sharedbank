#ifndef SHAREDBANKMODULECONFIG_H
#define SHAREDBANKMODULECONFIG_H

#include "ModuleConfig.h"

namespace cmangos_module
{

    class SharedBankModuleConfig : public ModuleConfig
    {
    public:
        bool Enabled = true;
        uint32 SpellId = 90000;
        uint8 BankSlots = 28;

        SharedBankModuleConfig();
        bool OnLoad() override;
    };

} // namespace cmangos_module

#endif
