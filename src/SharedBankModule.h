class SharedBankModule : public Module {
public:
    SharedBankModule();
    // Override hooks here, e.g.:
    bool OnCastSpell(Player* player, Spell* spell) override;
    void OnItemMove(Player* player, Item* item, uint8 srcBag, uint8 srcSlot, uint8 dstBag, uint8 dstSlot) override;
};
// At end:
static SharedBankModule sharedBankModule;