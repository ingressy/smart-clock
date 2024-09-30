import socket, subprocess
from sys import platform
from time import sleep


def start():
    print("m~cat v0.4 DEV")
    print("booting ...")

    print(socket.gethostbyname(socket.gethostname()))
    result = subprocess.run('iwgetid -r', capture_output=True, text=True, shell=True)
    ssid = result.stdout.strip()

    print(f"{ssid}")
    sleep(5)

def main():
    start()

if __name__ == "__main__":
    main()
