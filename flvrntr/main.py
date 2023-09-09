import sqlite3
import time
import pandas as pd

import paho.mqtt.client as mqtt
from .lookup import get_recipe, get_flavor_info, lookup_flavors

quantity_multiplier = 4
topic = "FLVR"
conn = sqlite3.connect('flvr.db')
cursor = conn.cursor()

# mqtt code
client = mqtt.Client()
client.username_pw_set("espresso", "coffee")
client.connect("192.168.1.143", port=1883)


def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    client.subscribe("FLVR")


def on_message(client, userdata, msg):
    print(msg.topic + " " + str(msg.payload))


# not really sure what this does 
# gpio.setmode(gpio.BCM)

while True: 
    client.on_connect = on_connect
    client.on_message = on_message
    client.loop_start()
    # import ipdb; ipdb.set_trace() 
    barcode = input("S C A N  B A R C O D E")
    flavors = get_recipe(barcode)
    print(get_flavor_info(flavors))
    signals = lookup_flavors(flavors)
    for signal in signals:
        # import ipdb; ipdb.set_trace()
        topic = "BOX" + str(signal['box'])
        payload = str({'box': signal['box'],
                   'pod': signal['pod'],
                   'amount': signal['value']})
        client.publish(topic, payload=payload)
        time.sleep(2)
