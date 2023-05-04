from flask import Flask
from flask import request
import math
import pymysql
from datetime import datetime

app = Flask(__name__)

db = pymysql.connect(host="database-1.clcq5zh2vv4n.us-east-2.rds.amazonaws.com",user='admin',password="supersecretpassword", database='healthmonitoring', autocommit = True)
cursor = db.cursor()
cursor.execute("select version()")

@app.route("/")
def hello():
    date = "'"+ datetime.now().strftime("%Y-%m-%d %H:%M:%S") + "'"
    temp = float(request.args.get("temp"))
    X = float(request.args.get("x"))
    Y = float(request.args.get("y"))
    Z = float(request.args.get("z"))
    heart = request.args.get("heart")
    spo2 = request.args.get("SpO2")
    print(date)
    print("Temperature: "+ str(temp))
    print("X: "+ str(X))
    print("Y: "+ str(Y))
    print("Z: "+ str(Z))
    print("HeartRate: "+str(heart))
    print("SPO2: "+str(spo2))
    print("\n")
    total_acceleration = math.sqrt((X*X)+(Y*Y)+(Z*Z))
    print("Total Acceleration: ", total_acceleration)
    if total_acceleration > 0.5:
        fall_detected = True
        print("****************FALL********************")
    else:
        fall_detected = False
    command = 'Insert into HISTORY(Date, X, Y, Z, TEMP, HEART, TOTAL_ACCEL, FALL_DETECTED) VALUES ({date},{X},{Y},{Z},{temp},{heart},{total_acceleration}, {fall_detected})'.format(date=date, X=X, Y=Y, Z=Z, temp=temp, heart=heart, total_acceleration=total_acceleration, fall_detected=fall_detected)
    print(cursor.execute(command))
    print(command)
    if fall_detected:
        return "true"
    else:
        return "false"