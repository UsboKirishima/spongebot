## SpongeBot Basic Idea

SpongeBot was conceived as an educational experiment to test the effectiveness of distributed systems in conducting DDoS attacks, managed by a centralized server utilizing the latest HTTP technologies.  
The primary focus is on creating the server module, also known as the Command and Control (C&C) server, which manages a vast array of bots (also known as zombies) installed on numerous infected devices that, in turn, attempt to infect others recursively.  
By delving into the topic of botnets and observing their behaviors, some characteristic differences between them can be identified.

---

### 1. Network Type
The type of network they use can generally be categorized into three types:
- **C&C Server**: A single centralized server on which the bots rely. In this case, the bots act as clients waiting for commands through a polling process.
- **Push**: A persistent connection is established via WebSocket, allowing the C&C to send commands to the bots in real-time without them requesting them.
- **Peer-to-Peer**: Bots communicate with each other, receiving and sending commands amongst themselves.

---

### 2. Propagation Methods:
- **Spam or Phishing**: Sending emails or messages containing the virus.
- **Network Scanning**: Involves continuously searching for vulnerable IP addresses using a short list of commonly used credentials.
- **0-Day Vulnerabilities**: Extremely rare as they exploit undiscovered vulnerabilities, but they are among the most dangerous.

---

### 3. Architecture of SpongeBot
SpongeBot is designed to be a botnet with three main modules:
1. **Server**: The core of the network, responsible for sending commands, receiving logs, and managing users.
2. **Bot**: Malicious programs that are installed on devices and connect back to the server.
3. **Client**: The "Command Center" where bot administrators can control the bots and view logs.

SpongeBot is engineered to be a highly professional and performant backend, keeping track of Logs, Bots, and Customers stored in dedicated databases.

---

### Overview Diagram

Here is a diagram illustrating how the ecosystem works in summary:

![main_botnet](/assets/main_botnet.jpg)
