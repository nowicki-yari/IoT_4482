import pandas as pd
from flask import request, redirect, url_for, Flask, render_template

app = Flask(__name__, template_folder='templates')

@app.route("/")
def main_page():
    return render_template('index.html')


if __name__ == '__main__':
   app.run(debug = True)

