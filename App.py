import eventlet
import json
from flask import Flask, render_template
from flask_mqtt import Mqtt
from flask_socketio import SocketIO
from flask_bootstrap import Bootstrap
from flask_cors import CORS, cross_origin
import ssl

eventlet.monkey_patch()

app = Flask(__name__)
app.config['SECRET'] = 'my secret key'
app.config['TEMPLATES_AUTO_RELOAD'] = False
app.config['MQTT_BROKER_URL'] = 'e83b7b9f6de84440b629c443336adf69.s1.eu.hivemq.cloud'
#app.config['MQTT_BROKER_URL'] = '192.168.1.5'
#app.config['MQTT_BROKER_PORT'] = 1883
app.config['MQTT_USERNAME'] = 'psoc6_1'
app.config['MQTT_PASSWORD'] = 'IoT_4482'
app.config['MQTT_KEEPALIVE'] = 5

# Parameters for SSL enabled
app.config['MQTT_BROKER_PORT'] = 8883
app.config['MQTT_TLS_ENABLED'] = True
app.config['MQTT_TLS_INSECURE'] = False
#app.config['MQTT_TLS_CA_CERTS'] = 'server.pem'
app.config['MQTT_TLS_CA_CERTS'] = 'cacert.pem'
#app.config['MQTT_TLS_CERTFILE'] = 'client.crt'
#app.config['MQTT_TLS_KEYFILE'] = 'client.key'
app.config['CLIENT_ID'] = 'Rocky'

app.config['MQTT_TLS_VERSION'] = ssl.PROTOCOL_TLSv1_2

cors = CORS(app, resources={r"*": {"origins": "*"}})
mqtt = Mqtt(app)
#socketio = SocketIO(app)
bootstrap = Bootstrap(app)
socketio = SocketIO(app,cors_allowed_origins='*')
#socketio.init_app(app, cors_allowed_origins="*")


@app.route('/dashboard')
def index_dashboard():
    mqtt.subscribe('ledstatus')
    return render_template('dashboard.html')

@app.route('/dashboard2')
def index_dashboard2():
    mqtt.subscribe('ledstatus')
    return render_template('test.html')

@app.route('/')
def index():
    return render_template('index.html')

@socketio.on('publish')
def handle_publish(json_str):
    data = json.loads(json_str)
    mqtt.publish(data['topic'], data['message'])

@socketio.on('subscribe')
def handle_subscribe(json_str):
    data = json.loads(json_str)
    mqtt.subscribe(data['topic'])

@socketio.on('unsubscribe_all')
def handle_unsubscribe_all():
    mqtt.unsubscribe_all()

@mqtt.on_message()
def handle_mqtt_message(client, userdata, message):
    data = dict(
        topic=message.topic,
        payload=message.payload.decode()
    )
    socketio.emit('mqtt_message', data=data)

@mqtt.on_log()
def handle_logging(client, userdata, level, buf):
    print(level, buf)

if __name__ == '__main__':
    socketio.run(app, host='127.0.0.1', port=80, use_reloader=False, debug=True)