![logo](/assets/log_ansi.png)
## What is Spongebot?
*Spongebot* is a centralized TCP botnet created for research and experiments on how botnets work and to find the best and most efficient ways to perform *Distributed Denial of Services* (DDoS) attacks.

## How it works?
Every feature made for this project was *carefully planned* to be as effective as possible, also taking inspiration from other notable botnets like *Mirai*, which stood out for the incredible number of bots it had during its lifetime.

So, let's explore some parts of Spongebot. The system is split into three main *modules*:

- [Bot](https://github.com/UsboKirishima/spongebot/tree/main/bot)

    - The bot is the malware installed on the victim's machine.

- [Server](https://github.com/UsboKirishima/spongebot/tree/main/server)

    - The server has two main functions. First, it provides a *RESTful* web API used by the Botmaster client (Botmaster → Server) to send commands. Second, it runs a *TCP* server that communicates with the bots (Server → Bots).

- [Client](https://github.com/UsboKirishima/spongebot/tree/main/client)

    - The client is a simple terminal application that makes it easy to manage bot information and send commands to all bots.

## Exploring bot

Now, let's take a look at the module structure.
```bash
.
├── build
│   └── bot
├── build_bot
├── command.c   # Command handler and parser
├── dev.sh
├── dict.c  # XOR'd Dictionary to hide costants
├── httplankton.c   # HTTP Attack
├── include     # Headers
│   ├── command.h 
│   ├── dict.h
│   ├── receiver.h
│   ├── rsa.h
│   ├── synflood.h
│   ├── updater.h
│   └── utils.h
├── main.c
├── receiver.c  # Handles TCP communications
├── rsa.c # Not yet implemented (Future purposes)
├── synflood.c # Syn flood attack
├── tcp_essyn.c # TCP ESSYN attack
├── udp_bluenurse.c # UDP attack
├── updater.c # Auto update malware (Experiment)
└── utils.c # Usefull functions
```

The bot, also called *"Spongebot"*, implements a set of common functionalities used by botnets.

Let's talk about communication with the C2 (Command & Control). The connection is handled by `receiver.c`, which contains basic client functions to connect, read, and send data to the TCP server. It uses raw C sockets and sends as little data as possible to make detection by network sniffing tools more difficult.

For example, when the bot establishes a connection with the server, it sends *"Hello Mr. Krabs"* to identify itself, and the server manages its IP.

When the bot receives data, it is automatically sent to `command.c`, which processes it and executes the corresponding commands.

In `command.h` is defined the command structure
```c
struct target_ip
{
    uint8_t o1;
    uint8_t o2;
    uint8_t o3;
    uint8_t o4;
};

struct command
{
    enum command_type type;
    struct
    {
        uint8_t duration;
        struct target_ip target;
        uint16_t port;
    } data;
};
```

and each types of commands supported

```c
enum command_type
{
    PING = 1 << 0,  // C2 check if bot is alive
    HELLO = 1 << 1, // C2 send this when connection begin
    ATTACK_TCP = 1 << 2,  //
    ATTACK_UDP = 1 << 3,  //
    ATTACK_HTTP = 1 << 4, //
    EXIT = 1 << 5, // self-destroy bot

    HAS_NO_ARGS = PING | HELLO | EXIT // commands with no args
};
```

The raw command byte-per-byte can be for example:
```bash
0x10 # command_type
0x22 # duration (minutes)
0x2d # ip o1 -> 45
0x41 # ip o2 -> 65
0xbd # ip o3 -> 189
0x2b # ip o4 -> 43
0x0d # port low
0x05 # port high
0x0a # terminator (\n)
```
The IP is split into four groups of 8 bits (o1.o2.o3.o4).

The port is split into two groups of 1 byte since the maximum number of available ports is 65,535 (2 bytes).

After parsing commands with `parse_command_from_buffer`, they are executed by creating child processes with `fork()`, allowing the bot to keep listening for new commands.

Information such as the C2 domain and certain strings are stored and XOR-encrypted in `dict.c`, which provides simple functions to add and retrieve data. This method is commonly used to obscure data from decompilers, though it is not entirely secure.

*P.S. The project isn’t focused on advanced malware hiding, just a few tricks to make reverse engineering a bit harder.*

I was particulary focused on *HTTPlankon* attack contained in `httplankton.c` that in substance is a mix of *Slowloris*, *HTTP Flood*, *HTTP Randomized User-Agent Flood*. It also uses techniques to evade firewalls and avoid detection.

## Exploring server

The server is split into two parts, developed in TypeScript with Express.js and Node.js (using node:net for TCP).

The **HTTP side** provides a series of API endpoints used by the Botmaster client to send directives and monitor commands for the bots.

```bash
/command/run/:type    # Send command
/info/get             # Get informations
/ping                 # Ping server
/token/new/:role      # Create a new token
/token/delete/:token  # Delete a token
/token/:token         # Get specific token info
/update/get           # Get latest binary (Experiment) 
/version/get          # Get latest version (Experiment)
```

It also has a hierarchy system because, from a malicious perspective, the system needs different permissions like read-only, admin, etc. However, this feature is currently focused only on the admin role. You can see the entire hierarchy in `hierarchy.ts` and `tokens.ts`

```ts
export type Role = 'admin' // Full access
    | 'operator'                    // Commands execution and operational management (Accounts creations).
    | 'supervisor'                  // Read-only permissions and access to db and analytics
    | 'bot'                         // Communication with the server & data exchange
    | 'customer'                    // Base user (Access to attacking commands and something other)
```

The **TCP side** handles all the bots. \
For example, when a bot connects to the C2, this part saves the IP in a database to reach it when the C2 needs to send a broadcast command. \
When a command is sent to the C2 from the Botmaster, it is processed by `command.ts` and sent to all bots via `clients.ts`. \
If a bot loses connection with the C2, it is automatically removed from the database.

HTTP Server is exposed on port `3000` \
TCP Server is exposed on port `8080` \
Database is a simple wrapper based on *Sqlite3*


## Botmaster client

A simple terminal client that makes HTTP request to rest API server

![client](/assets/client.png)

[//]: # (Other Features)



## Future experiments

- RSA Encryption
- Diffusion methods via IoT
- Easy configuration scripts
- Hide malware as LD_PRELOAD rootkit

## Using

1. Clone the repo
    ```bash
    git clone https://github.com/UsboKirishima/spongebot/
    ```
    ```bash
    cd spongebot
    ```

2. Build modules

    **WARNING**: Edit all the .env files and also put configurations as xored in `bot/dict.c` using `tools/xor.c`
    ```bash
    chmod +x build
    ./build all
    ```

3. Run Modules

    ```bash
    ./bot.sh &
    ./server.sh &
    ./client.sh
    ```

## Notes 

Let's look about [/notes](/notes/), an updated diary about the project path.

## Can I contribute?

Feel free to open a Pull Request or contact me on Discord (username: `usbo`)

## License & Credits

This project is under [Apache License 2.0](LICENSE) \
Copyright 2025 (C) UsboKirishima / 333revenge