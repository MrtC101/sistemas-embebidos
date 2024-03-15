from datetime import datetime,timedelta

def toTimeStamp(seconds):
    return (datetime(2000,1,1) + timedelta(seconds=seconds)).strftime("%y/%m/%d-%H:%M:%S")

def toSeconds(timeStamp):
    #print(timeStamp)
    total_time = datetime.strptime(timeStamp,"%y/%m/%d-%H:%M:%S") - datetime(2000,1,1)
    return total_time.total_seconds()