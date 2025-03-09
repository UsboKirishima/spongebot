import * as net from "net";
import { QuickDB } from "quick.db";

export class SocketManager {
    private static db = new QuickDB();
    private static sockets: Map<string, net.Socket> = new Map();

    /**
     * Add provided socket to the database, ensuring uniqueness
     * @param ip - The IP address of the client
     * @param socket - The socket instance
     */
    public static async addSocket(ip: string, socket: net.Socket): Promise<void> {
        const addresses = await this.db.get("addresses") || [];

        if (!addresses.some((entry: { ip: string }) => entry.ip === ip)) {
            const socketInfo = { ip, port: (socket.remotePort || 0) };
            await this.db.push("addresses", socketInfo);
        }

        this.sockets.set(ip, socket);
    }

    /**
     * Remove a specific socket from the database
     * @param ip - The IP address of the client
     */
    public static async removeSocket(ip: string): Promise<void> {
        if (this.sockets.has(ip)) {
            this.sockets.get(ip)?.destroy();
            this.sockets.delete(ip);
        }

        const addresses = await this.db.get("addresses") || [];
        await this.db.set("addresses", addresses.filter((entry: { ip: string }) => entry.ip !== ip));
    }

    /**
     * Check if a socket with the given IP exists in the database
     * @param ip - The IP address to check
     * @returns Promise<boolean>
     */
    public static async isSocketAlreadyInDb(ip: string): Promise<boolean> {
        const addresses = await this.db.get("addresses") || [];
        return addresses.some((entry: { ip: string }) => entry.ip === ip);
    }

    /**
     * Get all stored sockets as an array of { ip, port }
     */
    public static async getAllStoredSockets(): Promise<Array<{ ip: string, port: number }>> {
        return await this.db.get("addresses") || [];
    }

    /**
     * Get a socket instance by IP
     * @param ip - The IP address of the client
     * @returns net.Socket | undefined
     */
    public static getSocket(ip: string): net.Socket | undefined {
        return this.sockets.get(ip);
    }

    /**
     * Restore sockets from database (Useful after restart)
     * Attempts to reconnect stored sockets
     */
    public static async restoreSockets(): Promise<void> {
        const storedSockets = await this.db.get("addresses") || [];
        for (const { ip, port } of storedSockets) {
            const socket = new net.Socket();
            socket.connect(port, ip, () => {
                console.log(`Reconnected to ${ip}:${port}`);
                this.sockets.set(ip, socket);
            });
        }
    }
}
