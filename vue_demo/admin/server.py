from flask import Flask

app = Flask(__name__)

@app.route('/')
@app.route('/<id>')
def index(id=None):
    return open('index.html').read()

app.run('0.0.0.0', 8000)
