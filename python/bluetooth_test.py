import bluetooth

bd_addr = "24:6F:28:AD:D4:CA"
port = 1

sock = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
try:
    sock.connect((bd_addr, port))
    print("Connected. Type messages to send. Type 'exit' to quit.")
    while True:
        message = input("Enter message: ")
        if message.lower() == 'exit':
            break
        sock.send(message + "\n")
        print("Message sent.")
finally:
    sock.close()