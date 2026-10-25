

import sys
import socket




def main():
    host = '127.0.0.1'
    port = 2022

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind((host, port))
    s.listen(1)

    print(f"[*] Listening on {host}:{port}...")
    conn, addr = s.accept()
    print(f"[*] Connection established from {addr}")

    data = conn.recv(1024)
    print(f"[*] Received from agent: {data.decode('utf-8', errors='ignore').strip()}")

    conn.close()
    s.close()



if (__name__ == '__main__'): main()
