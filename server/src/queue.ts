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
    private status: QueueStatus = QueueStatus.IDLE;

    public constructor() {
        this.init();
    }

    private async init(): Promise<void> {
        await Database.setQueueStatus(this.status);
    }

}