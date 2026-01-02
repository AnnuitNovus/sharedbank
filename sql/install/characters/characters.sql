CREATE TABLE IF NOT EXISTS account_bank (
  account_id INT UNSIGNED NOT NULL,
  slot TINYINT UNSIGNED NOT NULL,
  item_guid INT UNSIGNED DEFAULT NULL,
  PRIMARY KEY (account_id, slot),
  KEY item_guid (item_guid),
  CONSTRAINT fk_account_bank_item FOREIGN KEY (item_guid) REFERENCES item_instance (guid) ON DELETE SET NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='Account Shared Bank';
