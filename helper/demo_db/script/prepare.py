import pandas as pd
import sqlalchemy

DELIMITER = ';'
THOUSANDS = "."
DECIMAL = ","

# CSV-Datei einlesen
# Root Tabelle
dataframes = [];
col = ["id", "database_name", "currency", "created_at"]
file = pd.read_csv("/Users/sebastian/Desktop/Shared/Projekt_Software_Engineering/PennyWise/PennyWise_v0.1/helper/demo_db/data/root.csv", delimiter=DELIMITER, thousands=THOUSANDS, decimal=DECIMAL, names=col, parse_dates=["created_at"], dayfirst=True, index_col=0)
dataframes.append(file)

# Icons Tabelle 
col = ["id", "name", "icon", "color_code", "Color_code_name"] 
file = pd.read_csv("/Users/sebastian/Desktop/Shared/Projekt_Software_Engineering/PennyWise/PennyWise_v0.1/helper/demo_db/data/icons.csv", delimiter=DELIMITER, thousands=THOUSANDS, decimal=DECIMAL, names=col, index_col=0)
dataframes.append(file)

# Accounts Tabelle
col = ["id","icon","name","account_holder","currency_id","account_number","iban","bank_code","bic","bank_name","opening_balance","created_at"]
file = pd.read_csv("/Users/sebastian/Desktop/Shared/Projekt_Software_Engineering/PennyWise/PennyWise_v0.1/helper/demo_db/data/accounts.csv", delimiter=DELIMITER, thousands=THOUSANDS, decimal=DECIMAL, names=col, parse_dates=["created_at"], dayfirst=True, index_col=0)
dataframes.append(file)

# Kategoriegruppe Tabelle
col = ["id","icon","name","type"]
file = pd.read_csv("/Users/sebastian/Desktop/Shared/Projekt_Software_Engineering/PennyWise/PennyWise_v0.1/helper/demo_db/data/categories_group.csv", delimiter=DELIMITER, thousands=THOUSANDS, decimal=DECIMAL, names=col, index_col=0)
dataframes.append(file)

# Kategoerien Tabelle
col = ["id","name","parent_group_id"]
file = pd.read_csv("/Users/sebastian/Desktop/Shared/Projekt_Software_Engineering/PennyWise/PennyWise_v0.1/helper/demo_db/data/categories.csv", delimiter=DELIMITER, thousands=THOUSANDS, decimal=DECIMAL, names=col, index_col=0)
dataframes.append(file)

#Transaktionen Tabelle
col = ["id","transaction_date","amount","recipient","purpose","category_id","category_group_id","account_id","currency_id","created_at"]
file = pd.read_csv("/Users/sebastian/Desktop/Shared/Projekt_Software_Engineering/PennyWise/PennyWise_v0.1/helper/demo_db/data/transactions.csv", delimiter=DELIMITER, thousands=THOUSANDS, decimal=DECIMAL, names=col, parse_dates=["transaction_date", "created_at"], dayfirst=True, index_col=0)
dataframes.append(file)


db = sqlalchemy.create_engine('sqlite:////Users/sebastian/Desktop/Shared/Projekt_Software_Engineering/PennyWise/PennyWise_v0.1/db/demo/demo.pwdb')

with db.connect() as conn:
		dataframes[0].to_sql('Root', conn, if_exists='append', index=False)
		# dataframes[1].to_sql('Icons', conn, if_exists='append', index=False)
		dataframes[2].to_sql('Accounts', conn, if_exists='append', index=False)
		dataframes[3].to_sql('Categories_group', conn, if_exists='append', index=False)
		dataframes[4].to_sql('Categories', conn, if_exists='append', index=False)
		dataframes[5].to_sql('Transactions', conn, if_exists='append', index=False)

print("finished")


