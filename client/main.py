import requests
import time

SERVER_URL = "http://localhost"
SERVER_PORT = 8080
ADMIN_TOKEN = "AD2442e9471b2ff5376b5a70486379f7"

def print_banner():
    ascii_art = """
    [0] Tokens
            """

    print(ascii_art)

def token_choose(): 
    print(chr(27) + "[2J")
    print("""
    [0] Create new token
    [1] Revocate token
    [2] Validate token
    """)

    token_opt = int(input("-> "))
    token = ""

    if token_opt != 0:
        token = input("[?] Insert token: ")

    headers = {
        "Authorization": ADMIN_TOKEN
    }

    if token_opt == 0:
        roles = ['admin', 'operator', 'supervisor', 'bot', 'customer']
        print(chr(27) + "[2J")
        for role in range(len(roles)):
            print(f"[{role}] {roles[role]}")

        role_selected = int(input('[?] Select token role: '))


        req = requests.post(f'{SERVER_URL}:{SERVER_PORT}/token/new/{roles[role_selected]}', headers=headers)

        if req.status_code != 200:
            print("[-] Error: " + req.text)
            time.sleep(2)
            return

        print("[+] Token created: " + req.text)
    elif token_opt == 1:
        req = requests.post(f'{SERVER_URL}:{SERVER_PORT}/token/delete/{token}', headers=headers)

        if req.status_code != 200:
            print("[-] Error: " + req.text)
            time.sleep(2)
            return

        print("[+] Token deleted: " + req.text)
    elif token_opt == 2:
        req = requests.post(f'{SERVER_URL}:{SERVER_PORT}/token/{token}', headers=headers)

        if req.status_code != 200:
            print("[-] Error: " + req.text)
            time.sleep(2)
            return

        print("[+] Token validated: " + req.text)

def main():
    while(True):
        print(chr(27) + "[2J")
        print_banner()

        opt = int(input("select -> "))
        

        match(opt):
            case 0:
                token_choose()
                break
            case 1:
                break
            

    

if __name__ == "__main__":
    main()
    exit(0)