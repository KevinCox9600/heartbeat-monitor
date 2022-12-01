# uploaded on pythonanywhere and hosted at clasersohn.pythonanywhere.com

from flask import Flask, request, Response

app = Flask(__name__)
heartrate = "10"

@app.route('/')
def hello_world():
    return 'welcome %s' % heartrate

@app.route('/post', methods=["POST"])
def post():
    global heartrate
    message = request.get_json()
    heartrate = message['heartrate']
    return Response(status=200)

@app.route('/get', methods=["GET"])
def get():
    global heartrate
    return 'HEART%sQ' % heartrate