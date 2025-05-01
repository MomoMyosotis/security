import socket

HOST = '0.0.0.0'
PORT = 12345

def start_server():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind((HOST, PORT))
    s.listen(1)
    print(f"Listening on {HOST}:{PORT}...")

    conn, addr = s.accept()
    print(f"Connection from {addr} established!")

    while True:
        command = input("Enter command: ")
        if command.lower() == 'exit':
            conn.send(command.encode())
            break
        conn.send(command.encode())
        output = conn.recv(4096).decode()
        print(output)
    
    conn.close()

if __name__ == "__main__":
    start_server()
