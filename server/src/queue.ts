import { QuickDB } from "quick.db";
import { Database } from "./database";

export enum QueueStatus {
    STOP = 2 << 0,
    TCP_ATTACK = 2 << 1,
    ACK_ATTACK = 2 << 2,
    //...
    IDLE = 2 << 5,
    EXIT = 2 << 6
}


export class Queue {
    private static db: QuickDB = Database.db;

    public static async setQueueStatus(status: QueueStatus) {
        await this.db.set(`queue`, status);
    }

    public static async getQueueStatus(): Promise<QueueStatus> {
        return await this.db.get(`queue`) as QueueStatus;
    }

}