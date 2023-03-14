#Using code from the paho-mqtt documentation (https://pypi.org/project/paho-mqtt)

import paho.mqtt.client as mqtt
from datetime import datetime
import os

import secrets

script_dir = os.path.realpath(__file__).split("main.py")[0]

def remove_readme_from_list(list):
    list.remove("README.md")
    return list

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
        try:
            fileToSend = script_dir+"esp2/"+remove_readme_from_list(os.listdir(os.getcwd()+"/esp2"))[0]
            f = open(fileToSend, "r")
            data = f.read()
            f.close()
        except IndexError:
            data = "No data to send"
        if (not client.publish("esp1/readData", data, qos=2)[0]):
            os.remove(fileToSend)

    elif (msg.topic == "esp2/receiveData"):
        try:
            fileToSend = script_dir+"esp1/"+remove_readme_from_list(os.listdir(os.getcwd()+"/esp1"))[0]
            f = open(fileToSend, "r")
            data = f.read()
            f.close()
        except:
            data = "No data to send"
        if (not client.publish("esp2/readData", data, qos=2)[0]):
            os.remove(fileToSend)

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.username_pw_set(secrets.username, secrets.password)
client.connect(secrets.mqtt_server, 1883)

client.loop_forever()
