#=================================#
# [ OWNER ]
#     CREATOR  : Vladislav Khudash
#     AGE      : 18
#     LOCATION : Ukraine
#
# [ PINFO ]
#     DATE     : 10.11.2026
#     PROJECT  : LINUX-BOT (SESSION)
#     PLATFORM : LINUX, WIN32
#=================================#




import sys    as libSys
import os     as libOs
import socket as libSock


BOT_SERVER_IPV4 : str = "127.0.0.1"
BOT_SERVER_PORT : int = 12701


SOCK_ENCODING : str = "ASCII"
SOCK_TIMEOUT  : int = 30
SOCK_KEY_SIZE : int = 16



LB_CMD_SYSINFO : int = 201




def start_server():
    with libSock.socket(
        libSock.AF_INET,
        libSock.SOCK_STREAM,
        libSock.IPPROTO_TCP
    ) as sock:
        sock.setsockopt(libSock.SOL_SOCKET, libSock.SO_REUSEADDR, 1)
        sock.bind((BOT_SERVER_IPV4, BOT_SERVER_PORT))
        sock.listen()

        conn, addr = sock.accept()

        with conn:
            conn.sendall(int(0x04082008).to_bytes(4, 'little') +
                         int(0x20080804).to_bytes(4, 'little'))



def main():
    start_server()




if (__name__ == '__main__'): main()
