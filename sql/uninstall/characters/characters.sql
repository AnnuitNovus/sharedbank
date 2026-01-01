CREATE TABLE IF NOT EXISTS account_bank (
  account_id INT UNSIGNED NOT NULL,
  slot TINYINT UNSIGNED NOT NULL,
  item_guid INT UNSIGNED DEFAULT NULL,
  PRIMARY KEY (account_id, slot)
);