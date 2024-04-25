import serial
import requests
import time

arduino_port = 'COM3'
baud_rate = 9600

arduino = serial.Serial(arduino_port, baud_rate, timeout=1)

while True:
    arduino_data = arduino.readline().decode().strip()
    # print(arduino_data)
    try:
        x, y, z = map(int, [value.replace('G', '')
                      for value in arduino_data.split()])
    except ValueError:
        print("Invalid data format received. Skipping this iteration.")
        continue
    backend_url = 'https://harlequin-gharial-slip.cyclic.app/sendData'
    payload = {'x': x, 'y': y, 'z': z}
    print(payload)
    try:
        response = requests.post(backend_url, json=payload)
        print(
            f'Successfully sent ADXL data to backend. Response: {response.text}')
    except Exception as e:
        print(f'Error sending ADXL data to backend: {str(e)}')

    # time.sleep(1)

arduino.close()
