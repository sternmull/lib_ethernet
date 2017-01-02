import socket

# send UDP packets at maximum speed
if __name__ == '__main__':
    with socket.socket(type=socket.SOCK_DGRAM) as s:
        s.connect(('192.168.0.125', 1234))
        msg = bytes(1460)
        while True:
            s.send(msg)
 
