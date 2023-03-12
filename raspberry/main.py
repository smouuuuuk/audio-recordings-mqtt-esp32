#Using code from the paho-mqtt documentation (https://pypi.org/project/paho-mqtt)

import paho.mqtt.client as mqtt
from datetime import datetime

import secrets

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    client.subscribe("esp1/testTopic")
    client.subscribe("esp2/testTopic")

def on_message(client, userdata, msg):
    message = msg.payload.decode()
    dt_string = datetime.now().strftime("%Y%m%d-%H%M%S")
    print("From " + msg.topic + ": " + message)
    if (msg.topic == "esp1/testTopic"):
        print("MQTT post from ESP1")
        f = open("esp1/{}.txt".format(dt_string), 'w')
        f.write(message)
        f.close()
    elif (msg.topic == "esp2/testTopic"):
        print("MQTT post from ESP2")
        f = open("esp2/{}.txt".format(dt_string), 'w')
        f.write(message)
        f.close()

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.username_pw_set(secrets.username, secrets.password)
client.connect(secrets.mqtt_server, 1883)

client.loop_forever()