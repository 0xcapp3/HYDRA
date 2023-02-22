import paho.mqtt.client as mqtt
from AWSIoTPythonSDK.MQTTLib import AWSIoTMQTTClient
import json
import paho.mqtt.publish as publish


# TOPIC_IN = "topic_in"
TOPIC_OUT = "hydra_uplink"
# TOPIC_OUT2 = "box"

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    client.subscribe(TOPIC_OUT)
    # client.subscribe(TOPIC_OUT2)

def on_message(client, userdata, msg):
    print(msg.topic+" "+str(msg.payload))
    message = {}
    message['message'] = str(msg.payload)[2:-1]
    messageJson = json.dumps(message)
    myAWSIoTMQTTClient.publish(msg.topic, messageJson, 1)

# def customCallback(client, userdata, message):
#     jsonmessage = json.loads(message.payload)
#     publish.single(TOPIC_IN, jsonmessage["message"], hostname="localhost", port=1886, client_id="2")

host = "a2kt8wlie9xut1-ats.iot.eu-west-1.amazonaws.com"
rootCAPath = "/home/riot/Desktop/dev/HYDRA/bridge/root-CA.crt"
certificatePath = "/home/riot/Desktop/dev/HYDRA/bridge/hydra-thing.cert.pem"
privateKeyPath = "/home/riot/Desktop/dev/HYDRA/bridge/hydra-thing.private.key"
clientId = "hydra_bridge"
port=8883

myAWSIoTMQTTClient = None
myAWSIoTMQTTClient = AWSIoTMQTTClient(clientId)
myAWSIoTMQTTClient.configureEndpoint(host, port)
myAWSIoTMQTTClient.configureCredentials(rootCAPath, privateKeyPath, certificatePath)
myAWSIoTMQTTClient.connect()

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

# myAWSIoTMQTTClient.subscribe(TOPIC_IN, 1, customCallback)

client.connect("localhost", 1886, 60)
client.loop_forever()