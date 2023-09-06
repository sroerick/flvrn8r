import sqlite3

import sqlite3
import pandas as pd

# Set up SQLite connection and cursor
conn = sqlite3.connect('flvr.db')
cursor = conn.cursor()

# Fetch your data from SQLite into a pandas DataFrame

def get_recipe(barcode):
    # Directly get the row for the given barcode
    df = pd.read_sql_query(f'SELECT * FROM recipes WHERE "Variant Barcode" = ?', conn, params=(barcode,))
    
    if df.empty:
        return None
    
    # Filter the columns where the value is non-zero
    non_zero_data = df.loc[:, (df != 0).any(axis=0)]
    
    # Convert the row to a dictionary and get the first item (as there should only be one row)
    flavors_with_values = non_zero_data.to_dict(orient='records')[0]
    
    return flavors_with_values

def get_flavor_info(result):
    keys_to_print = ['Variant Barcode', 'Name', 'Handle', 'Type', 'Weight', 'Bean', 'Reg-Decaf']
    return {k: result[k] for k in keys_to_print if k in result}

def get_flavors(result):
    keys_to_drop = ['index', 'Name', 'Handle', 'Type', 'Variant SKU', 'Variant Barcode', 'Weight', 'Bean', 'Reg-Decaf']
    filtered_data = {k: v for k, v in result.items() if k not in keys_to_drop}
    return filtered_data

def lookup_flavors(result):
    output = []

    data = get_flavors(result)
    
    # Iterate through the dictionary
    for flavor, value in data.items():
        # Query the database
        cursor.execute("SELECT box, pod FROM lookup WHERE flavor = ?", (flavor,))
        result = cursor.fetchone()
        
        # If a match is found, append to the output list
        if result:
            output.append({'flavor': flavor, 'box': result[0], 'pod': result[1], 'value': value})
    
    return output


    


barcode_to_lookup = 110000000593
flavors = get_recipe(barcode_to_lookup)
print(get_flavor_info(flavors))
print(lookup_flavors(flavors))
