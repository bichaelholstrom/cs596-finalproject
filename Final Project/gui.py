import pymysql
import os
import time

db = pymysql.connect(host="database-1.clcq5zh2vv4n.us-east-2.rds.amazonaws.com",user='admin',password="supersecretpassword", database='healthmonitoring', autocommit=True)
cursor = db.cursor()
cursor.execute("select version()")
flag= False

def retrieve_latest_data():
    global flag
    command = "Select * from HISTORY ORDER BY Date DESC LIMIT 1;"
    cursor.execute(command)
    result = cursor.fetchall()
    print("Time: ", result[0][0])
    print("Temperature: ", result[0][4])
    print("HeartRate: ", result[0][5])
    fall_detected = result[0][7]

    if fall_detected:
        flag = True

    if flag:
        print("*****FALL DETECTED********")

    time.sleep(1)

    os.system('cls' if os.name == 'nt' else 'clear')

while True:
    retrieve_latest_data()