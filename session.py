

import sys    as libSys
import os     as libOs
import socket as libSock


BOT_SERVER_IPV4 : str = "127.0.0.1"
BOT_SERVER_PORT : int = 8080


SOCK_ENCODING : str = "ASCII"
SOCK_TIMEOUT  : int = 60
SOCK_KEY_SIZE : int = 32


HTTP_SERVER_NAME: str = "linbot"

HTTP_HDR_CMD: bytes = (
    "HTTP/1.1 %d OK\r\n" # %d - command
    f"Server: {HTTP_SERVER_NAME}\r\n"
    "Content-Length: 0\r\n"
    "Connection: keep-alive\r\n"
).encode(SOCK_ENCODING)
SIZE_HTTP_HDR_CMD: int = 128


CMD_REG_KEY = 200


def build_response(cmd_code: int) -> bytes:
    target_text_len = SIZE_HTTP_HDR_CMD - 4
    header = (HTTP_HDR_CMD % cmd_code).ljust(target_text_len, b' ') + b"\r\n\r\n"
    assert len(header) == SIZE_HTTP_HDR_CMD, f"Размер хедера поплыл: {len(header)}"
    return header


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
            conn.sendall(build_response(200))



def main():
    start_server()




if (__name__ == '__main__'): main()
