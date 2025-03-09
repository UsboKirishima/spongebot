import { QuickDB } from "quick.db";
import { HierarchyRole } from "./hierarchy";
import { Hierarchy } from "./hierarchy";
import { Queue, QueueStatus } from "./queue";

/**
 * Differences between Login and Token:
 *  - Login: is an object that implements ILogin type 
 *          so it contains a token and a role
 *  - Token: is the authentication string contained in Login 
 *          and it has a role 
 */


export interface ILogin {
    token: string;
    role: HierarchyRole;
}

export class Database {
    public static db: QuickDB = new QuickDB();
    

    public static async deleteAllData(): Promise<void> {
        await this.db.deleteAll();
    }
}
