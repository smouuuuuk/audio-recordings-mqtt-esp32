#Using code from the paho-mqtt documentation (https://pypi.org/project/paho-mqtt)

import paho.mqtt.client as mqtt
from datetime import datetime
import os

import secrets

script_dir = os.path.realpath(__file__).split("main.py")[0]

def get_files(path):
    for file in os.listdir(path):
        if os.path.isfile(os.path.join(path, file)):
            yield file

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    client.subscribe("esp1/sendData")
    client.subscribe("esp1/receiveData")
    client.subscribe("esp2/sendData")
    client.subscribe("esp2/receiveData")

def on_message(client, userdata, msg):
    message = msg.payload.decode()
    dt_string = datetime.now().strftime("%Y%m%d-%H%M%S")
    print("MQTT Post in " + msg.topic + "\n" + message + "\n")

    if (msg.topic == "esp1/sendData"):
        f = open("esp1/{}.txt".format(dt_string), 'w')
        f.write(message)
        f.close()

    elif (msg.topic == "esp2/sendData"):
        f = open("esp2/{}.txt".format(dt_string), 'w')
        f.write(message)
        f.close()

    elif (msg.topic == "esp1/receiveData"):
        fileToSend = os.listdir(os.getcwd()+"/esp1")[0]
        f = open(fileToSend, "r")
        data = f.read()
        client.publish("esp1/readData", data)

    elif (msg.topic == "esp2/receiveData"):
        fileToSend = os.listdir(os.getcwd()+"/esp2")[0]
        f = open(fileToSend, "r")
        data = f.read()
        client.publish("esp2/readData", data)

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.username_pw_set(secrets.username, secrets.password)
client.connect(secrets.mqtt_server, 1883)

client.loop_forever()