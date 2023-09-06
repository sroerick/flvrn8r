import pandas as pd
import sqlite3

df = pd.read_csv('./Recipes 20230710.csv')
conn = sqlite3.connect('flvr.db')
df.to_sql('recipes', conn, if_exists='replace')

print(df.columns)

flavors = ['Almond', 'Amaretto', 'Angel\'s Kiss',
       'Apple Strudel', 'Banana Nut Creme',
       'Blueberry Cinnamon Crumble',
       'Butter Pecan', 'Butter Rum', 'Butterscotch Toffee', 'Candy Cane',
       'Caramel', 'Carrot Cake', 'Chocolate Rum Truffle', 'Cherry',
       'Chocolate Almond', 'Chocolate Avalanche', 'Chocolate Cream Puff',
       'Chocolate Mint', 'Chocolate Pretzel', 'Cinnamon', 'Cinnamon Rum Pecan',
       'City', 'Cookies and Cream', 'Costa Rica', 'Cranberry',
       'Cranberry Creme', 'Cream', 'Creme Brulee', 'Danish Pastry',
       'Dark Chocolate', 'Deck the Halls', 'Eggnog',
       'French Caramel Cream', 'French Toast',
       'French Vanilla', 'Frosty\'s Favorites', 'Gavins Grogg',
       'Graham Cracker', 'Hawaiian Macadamia Nut', 'Hazelnut',
       'Hazelnut Toffee', 'Irish Cream', 'Italian', 'Jamaican Me Crazy',
       'Jingle Bell Java', 'Karnival Kreme', 'Kenya', 'Lingonberry',
       'Macadamia', 'Mistletoe Delight', 'Nutcracker Sweet',
       'Peanut Butter', 'Pumpkin Pecan', 'Pumpkin Spice', 'Raspberry Creme',
       'Reindeer Rum', 'Roasted Chestnut', 'Salted Caramel',
       'Special Bourbon Brulee', 'Spiced Butter',
       'Tiramisu', 'Toasted Coconut', 'Toasted Marshmallow', 'Toasted Praline',
       'Vanilla Cupcake', 'Vanilla Nut Creme', 'Vermont Maple Nut Crunch',
       'White Chocolate']

lookup_table = pd.DataFrame(columns=['Flavor', 'Box', 'Pod'])


for i, flavor in enumerate(flavors):
    # Box is 1 + (current index // 8) 
    # because every 8 flavors, the Box value should increase by 1
    box_value = 1 + (i // 8)
    
    # Pod is 1 + (current index % 8) 
    # because Pod value should reset after every 8 flavors
    pod_value = 1 + (i % 8)
    
    lookup_table = lookup_table.append({'Flavor': flavor, 'Box': box_value, 'Pod': pod_value}, ignore_index=True)



lookup_table.to_sql('lookup', conn, if_exists='replace')




