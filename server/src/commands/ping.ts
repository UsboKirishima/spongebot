import { HttpMethod } from "@/common/utils/httpHandlers";
import { HierarchyRole } from "../structures/hierarchy";
import { Command, CommandOptions, ICommand } from "../structures/command";

export class PingCommand extends Command implements ICommand {

    public constructor() {
        const options: CommandOptions = {
            requiredPermissions: ['customer', 'bot'], 
            method: 'GET',
            path: '/api/command/ping',
            tags: ['health', 'ping', 'utility'],
            description: 'Check server status and connectivity',
            isPublic: true
        };
        super(options);
    }

    /**
     * @description Executes the Ping Command
     */
    public async run(): Promise<void> {
        console.log('Pong! Server is alive.');
    }
}
