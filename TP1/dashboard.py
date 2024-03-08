import functools
from flask import (
    Blueprint, flash, g, redirect, render_template, request, session, url_for
)
from .backend.serial_port import usb_port

bp = Blueprint('dashboard', __name__, url_prefix="/tp1")

@bp.route("/dashboard",methods=(["GET"]))
def dashboard():   
    return render_template('dashboard/dashboard.html',
                           conn=usb_port.is_connected(),
                           )
