import dash
from dash import dcc
from dash import html
import plotly.express as px
import pandas as pd
import flask
from flask import request, redirect, url_for




#Comment
server = flask.Flask(__name__)

df = pd.read_csv('weather.csv')
df = df.drop(['Pressure3pm', 'Pressure9am'], axis=1)
pd.options.plotting.backend = "plotly"
external_stylesheets = ['https://codepen.io/chriddyp/pen/bWLwgP.css']

app = dash.Dash(__name__, external_stylesheets=external_stylesheets, server=server)


layout_page_1 = html.Div([
    html.H2('Weather App prototype Siemen test'),
    dcc.Dropdown(
        id='dropdown-time',
        options=[{'label': i, 'value': i} for i in ['9am', '3pm']],
        value='9am'
    ),
    dcc.Graph(id='display-value')
])

# index layout
app.layout = layout_page_1


# Flask route (GET)
@server.route('/hello')
def hello():
    return "Hello world"


@server.route('/post', methods=['POST'])
def post():
    if request.method == 'POST':
        temp = request.form['temp']
        return "Hello, it is {} degrees".format(temp)
    return "post gets here"


if __name__ == '__main__':
   app.run(debug = True)



# This function is triggered when the Input changes (so the dropdown menu in this case)
# This function returns the figure to the output (dcc.Graph in this case)
@app.callback(dash.dependencies.Output('display-value', 'figure'),
                [dash.dependencies.Input('dropdown-time', 'value')])
def display_value(value):
    # Next 2 lines are for data filtering, next 2 are for updating the graph
    df_filtered = df.filter(regex='{}$'.format(value), axis=1)
    df_filtered = df_filtered.drop(['WindDir{}'.format(value)], axis=1)
    fig = px.line(df_filtered)
    fig.update_layout()
    return fig


if __name__ == '__main__':
    app.run_server(debug=True, port=8080)
