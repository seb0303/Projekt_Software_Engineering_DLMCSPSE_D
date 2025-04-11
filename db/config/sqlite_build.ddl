-- Table: Root
CREATE TABLE IF NOT EXISTS Root (
                      id INTEGER PRIMARY KEY AUTOINCREMENT,
                      database_name TEXT NOT NULL,
                      currency TEXT NOT NULL,
                      created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
CREATE TABLE IF NOT EXISTS Icons (
                       id INTEGER PRIMARY KEY AUTOINCREMENT,
                       name TEXT NOT NULL,
                       icon BLOB NOT NULL,
                       color_code TEXT NOT NULL,
                       color_code_name TEXT NOT NULL
);
-- Table: Accounts
CREATE TABLE IF NOT EXISTS Accounts (
                          id INTEGER PRIMARY KEY AUTOINCREMENT,
                          icon INTEGER,
                          name TEXT NOT NULL,
                          account_holder TEXT,
                          currency_id INTEGER NOT NULL,
                          account_number TEXT,
                          iban TEXT,
                          bank_code TEXT,
                          bic TEXT,
                          bank_name TEXT,
                          opening_balance DECIMAL(10,2) DEFAULT 0.00,
                          created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                          FOREIGN KEY (currency_id) REFERENCES Root(id),
                          FOREIGN KEY (icon) REFERENCES Icons(id)
);
-- Table: Categories_group
CREATE TABLE IF NOT EXISTS Categories_group (
                                  id INTEGER PRIMARY KEY AUTOINCREMENT,
                                  icon INTEGER,
                                  name TEXT NOT NULL,
                                  type TEXT NOT NULL CHECK (type IN ('income', 'expenses', 'transfer', 'none')),
                                  FOREIGN KEY (icon) REFERENCES Icons(id)
);
-- Table: Categories
CREATE TABLE IF NOT EXISTS Categories (
                            id INTEGER PRIMARY KEY AUTOINCREMENT,
                            name TEXT NOT NULL,
                            parent_group_id INTEGER,
                            FOREIGN KEY (parent_group_id) REFERENCES Categories_group(id) ON DELETE CASCADE
);
-- Table: Transactions
CREATE TABLE IF NOT EXISTS Transactions (
                              id INTEGER PRIMARY KEY AUTOINCREMENT,
                              transaction_date TIMESTAMP NOT NULL,
                              amount DECIMAL(10,2) NOT NULL,
                              recipient TEXT,
                              purpose TEXT,
                              category_id INTEGER NOT NULL DEFAULT 1,
                              category_group_id INTEGER NOT NULL DEFAULT 1,
                              account_id INTEGER NOT NULL,
                              currency_id INTEGER NOT NULL,
                              created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                              FOREIGN KEY (category_id) REFERENCES Categories(id) ON DELETE SET DEFAULT,
                              FOREIGN KEY (category_group_id) REFERENCES Categories_group(id) ON DELETE SET DEFAULT,
                              FOREIGN KEY (account_id) REFERENCES Accounts(id) ON DELETE CASCADE,
                              FOREIGN KEY (currency_id) REFERENCES Root(id)
);
-- Table: Cache
CREATE TABLE IF NOT EXISTS Cache (
                       id INTEGER PRIMARY KEY AUTOINCREMENT,
                       account_id INTEGER NOT NULL,
                       transaction_date TIMESTAMP NOT NULL,
                       amount DECIMAL(10,2) NOT NULL,
                       running_balance DECIMAL(10,2) NOT NULL,
                       currency_id INTEGER NOT NULL,
                       FOREIGN KEY (account_id) REFERENCES Accounts(id),
                       FOREIGN KEY (currency_id) REFERENCES Root(id)
);