from flask import Flask, url_for
from flask_socketio import SocketIO 
from . import dashboard

socketio = SocketIO()

def create_app():
    """
        app factory
    """
    app = Flask(__name__,instance_relative_config=True)
    app.config.from_mapping(
        SECRET_KEY='dev'
    )
    
    app.register_blueprint(dashboard.bp);

    socketio.init_app(app)
    
    return app;