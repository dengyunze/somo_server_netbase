import socket

s=socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
s.sendto("hello udp".encode(),("127.0.0.1",8000))
