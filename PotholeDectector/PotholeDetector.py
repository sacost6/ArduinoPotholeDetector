import serial
import mysql.connector

# Initialize the mysql connector to connect to the database
mydb = mysql.connector.connect(
    host ="127.0.0.1",
    user = "root",
    passwd = "yary0119",
    database="Arduino"
    )

mycursor = mydb.cursor(buffered=True)

# Connect the arduino serial monitor to be read by the python script
ser = serial.Serial('/dev/cu.usbmodem1421', 115200, timeout=1)
i = 1
a = 3
b = 3
val = []

# Set up an indefinite loop to read the serial monitor and parse the output
# if the coordinate is in the form 0000.0000bump0000.0000 then the coordinate is
# stored in the mySQL database. If the coordinate is 0000.0000,0000.0000 then the
# coordinate is checked in the mySQL database and if is within range to a pothole then
# a string is sent to the arduino 
while True:
  string= ser.readline()
  print(string)
  if("bump" in string):
    a = string.split("bump")[0]
    b = string.split("bump")[1]
    lat = int(a.split(".")[0])
    lat_dec = int(a.split(".")[1])
    longi = int(b.split(".")[0])
    longi_dec = int(b.split(".")[1])
    sql = """INSERT INTO gps_coordinates (latitude, longitude, lat_two, long_two) VALUES (%s,%s,%s,%s)"""
    val = (lat, longi, lat_dec, longi_dec)
    print("Printing to database")
    print(val)
    mycursor.execute(sql, tuple(val))
    mydb.commit()
    val = []
  elif("," in string):
    a = string.split(",")[0]
    b = string.split(",")[1]
    lat = int(a.split(".")[0])
    lat_dec = int(a.split(".")[1])
    longi = int(b.split(".")[0])
    longi_dec = int(b.split(".")[1])
    query =  ("SELECT lat_two FROM gps_coordinates WHERE latitude=%s")
    mycursor.execute(query, (lat,))
    
    if(mycursor.rowcount!=0):
      for (lat_two) in mycursor:
    
        temp1 = lat_two[0] + 14
        temp2 = lat_two[0] - 14;
        query =  ("SELECT latitude, longitude FROM gps_coordinates "
                "WHERE lat_two BETWEEN %s AND %s")
        val = (temp2, temp1)
        print(val)
        mycursor.execute(query, val)
  
        if(mycursor.rowcount != 0):
          ser.write(b'Bump')
          print("you are nearing a bump")

    else:
     query =  ("SELECT latitude, longitude,lat_two, long_two FROM gps_coordinates WHERE longitude=%s")
     mycursor.execute(query, (long,))
     if(mycursor.rowcount != 0):
       for (lat_two) in mycursor:
         temp1 = lat_two + 14
         temp2 = lat_two - 14;
         query =  ("SELECT latitude, longitude FROM gps_coordinates "
                   "WHERE long_two BETWEEN %s AND %s")
         val = (temp1, temp2)
         mycursor.execute(query, val)
         if(mycursor.rowcount != 0):
          print("you are nearing a bump")






    
        
