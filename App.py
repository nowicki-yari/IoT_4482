import eventlet
import json
from flask import Flask, render_template
from flask_mqtt import Mqtt
from flask_socketio import SocketIO
from flask_bootstrap import Bootstrap
from flask_cors import CORS, cross_origin
import ssl
from datetime import datetime
import firebase_admin
import json
from flask import request

cred_obj = firebase_admin.credentials.Certificate('weatherstation-iot-8558e-firebase-adminsdk-boway-7b9758599c.json')
databaseURL = "https://weatherstation-iot-8558e-default-rtdb.firebaseio.com/"
default_app = firebase_admin.initialize_app(cred_obj, {'databaseURL':databaseURL})
from firebase_admin import db

eventlet.monkey_patch()

app = Flask(__name__)
app.config['SECRET'] = 'my secret key'
app.config['TEMPLATES_AUTO_RELOAD'] = False
app.config['MQTT_BROKER_URL'] = 'e83b7b9f6de84440b629c443336adf69.s1.eu.hivemq.cloud'
app.config['MQTT_USERNAME'] = 'psoc6_1'
app.config['MQTT_PASSWORD'] = 'IoT_4482'
app.config['MQTT_KEEPALIVE'] = 5

# Parameters for SSL enabled
app.config['MQTT_BROKER_PORT'] = 8883
app.config['MQTT_TLS_ENABLED'] = True
app.config['MQTT_TLS_INSECURE'] = False
app.config['MQTT_TLS_CA_CERTS'] = 'cacert.pem'
app.config['CLIENT_ID'] = 'Rocky'

app.config['MQTT_TLS_VERSION'] = ssl.PROTOCOL_TLSv1_2

cors = CORS(app, resources={r"*": {"origins": "*"}})
mqtt = Mqtt(app)
bootstrap = Bootstrap(app)
socketio = SocketIO(app,cors_allowed_origins='*')

@app.route('/')
def index():
	mqtt.subscribe('ledstatus')
	mqtt.subscribe('moist')
	return render_template('dashboard.html')

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
	
	#Upload data to firebase
	now = datetime.now()
	timeString = now.strftime("%d,%m,%Y,%H,%M,%S")
	timeSplit = timeString.split(",")
	timeMap = map(int, timeSplit)
	timeList = list(timeMap)
	print(str(timeList))
	"""
	for value in range(0, 6):
		string = str(timeList[value])
		if string[0:1] == 0:
			timeList[value] = int(string[1:2])
	print(str(timeList))
	"""
	print(str(data))

	#Upload temperature data
	if data["topic"] == "ledstatus":
		ref = db.reference("/Temperature")
		print("Time: " + str(timeList) + ",	Temp: " + str(data["payload"]))

		json_date = "\"date\": " + str((timeList))
		json_data = "\"data\": " + str(data["payload"])

		json_string = str("{" + json_date + ", " + json_data + "}")

		ref.push().set(json_string) #Enable!!

	#Upload moist data
	elif data["topic"] == "moist":
		ref = db.reference("/Moist")
		print("Time: " + str(timeList) + ",	Moist: " + str(data["payload"]))

		json_date = "\"date\": " + str((timeList))
		json_data = "\"data\": " + str(data["payload"])

		json_string = str("{" + json_date + ", " + json_data + "}")

		ref.push().set(json_string) #Enable!!
	else:
		print("Unknown topic!")

	socketio.emit('mqtt_message', data=data)

@app.route('/data_today')
def data_today():
	#Get the temperature data
	print("Getting temperature history data ...")
	ref = db.reference("/Temperature")
	data = ref.get()
	data_json = json.dumps(data)
	data_json = json.loads(data_json)

	#Get the values
	time = []
	value = []
	for dataPoint in data_json.values():
		json_string = json.loads(dataPoint)
		time.append(json_string["date"])
		value.append(json_string["data"])

	#Get data of today
	day = time[len(time)-1][0]
	times = []
	values = []
	for counter in range(0, len(value)):
		if time[counter][0] == day:
			data = dict(
				time = time[counter][3:6],
				value = value[counter]
			)
			socketio.emit('historical_data', data=data)

	#Get the moist data
	print("Getting moist history data ...")
	ref = db.reference("/Moist")
	data = ref.get()
	data_json = json.dumps(data)
	data_json = json.loads(data_json)

	#Get the values
	time = []
	value = []
	for dataPoint in data_json.values():
		json_string = json.loads(dataPoint)
		time.append(json_string["date"])
		value.append(json_string["data"])

	#Get data of today
	day = time[len(time)-1][0]
	times = []
	values = []
	for counter in range(0, len(value)):
		if time[counter][0] == day:
			data = dict(
				time = time[counter][3:6],
				value = value[counter]
			)
			socketio.emit('moist_Historical_Data', data=data)

	return ("Not needed")

@app.route('/data_yesterday')
def data_yesterday():
	#Get the temperature data
	ref = db.reference("/Temperature")
	data = ref.get()
	data_json = json.dumps(data)
	data_json = json.loads(data_json)

	#Get the values
	time = []
	value = []
	for dataPoint in data_json.values():
		json_string = json.loads(dataPoint)
		time.append(json_string["date"])
		value.append(json_string["data"])

	#Get data of yesterday
	day = time[len(time)-1][0] - 1
	for counter in range(0, len(value)):
		if time[counter][0] == day:
			data = dict(
				time = time[counter][3:6],
				value = value[counter]
			)
			socketio.emit('historical_data', data=data)

	#Get the moist data
	ref = db.reference("/Moist")
	data = ref.get()
	data_json = json.dumps(data)
	data_json = json.loads(data_json)

	#Get the values
	time = []
	value = []
	for dataPoint in data_json.values():
		json_string = json.loads(dataPoint)
		time.append(json_string["date"])
		value.append(json_string["data"])

	#Get data of yesterday
	day = time[len(time)-1][0] - 1
	for counter in range(0, len(value)):
		if time[counter][0] == day:
			data = dict(
				time = time[counter][3:6],
				value = value[counter]
			)
			socketio.emit('moist_Historical_Data', data=data)

	return ("Not needed")

@app.route('/calendarInput', methods=['POST'])
def calendarInput():
	date = request.form['date']
	print("Choosen date: " + str(date))

	#Get the temperature data
	ref = db.reference("/Temperature")
	data = ref.get()
	data_json = json.dumps(data)
	data_json = json.loads(data_json)

	#Get the values
	time = []
	value = []
	for dataPoint in data_json.values():
		json_string = json.loads(dataPoint)
		time.append(json_string["date"])
		value.append(json_string["data"])

	#Get data of input date
	year = str(date[0:4])
	month = str(date[5:7])
	day = str(date[8:10])

	if int(day) < 10:
		day = str(day[1])
	if int(month) < 10:
		month = str(month[1])

	times = []
	values = []
	for counter in range(0, len(value)):
		if str(time[counter][0]) == day and str(time[counter][1]) == month and str(time[counter][2]) == year:
			values.append(value[counter])
			times.append(time[counter][3:6])

	data = dict(
		time = times,
		value = values
	)

	socketio.emit('historical_data_cal', data=data)

	#Get the moist data
	ref = db.reference("/Moist")
	data = ref.get()
	data_json = json.dumps(data)
	data_json = json.loads(data_json)

	#Get the values
	time = []
	value = []
	for dataPoint in data_json.values():
		json_string = json.loads(dataPoint)
		time.append(json_string["date"])
		value.append(json_string["data"])

	#Get data of input date
	year = str(date[0:4])
	month = str(date[5:7])
	day = str(date[8:10])

	if int(day) < 10:
		day = str(day[1])
	if int(month) < 10:
		month = str(month[1])

	times = []
	values = []
	for counter in range(0, len(value)):
		if str(time[counter][0]) == day and str(time[counter][1]) == month and str(time[counter][2]) == year:
			values.append(value[counter])
			times.append(time[counter][3:6])

	data = dict(
		time = times,
		value = values
	)

	socketio.emit('moist_Historical_Data_cal', data=data)

	return ('', 204)

@mqtt.on_log()
def handle_logging(client, userdata, level, buf):
    print(level, buf)

def reWriteDigit(digit):
	return 0

if __name__ == '__main__':
    socketio.run(app, host='127.0.0.1', port=80, use_reloader=False, debug=True)