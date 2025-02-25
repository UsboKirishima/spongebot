import { HttpMethod } from "@/common/utils/httpHandlers";
import { HierarchyRole } from "./hierarchy";

export interface CommandOptions {
    requiredPermissions: Array<HierarchyRole>;     // Permissions required to execute command
    method: HttpMethod;                            // HTTP Method (e.g. GET, POST)
    path: string;                                  // URL Path (e.g. /api/auth)
    tags: Array<string>;
    description: string;
}

export interface ICommand {
    run(): Promise<void>;
}

export abstract class Command implements ICommand {
    protected options: CommandOptions;

    public constructor(options: CommandOptions) {
        this.options = options;
    }

    public abstract run(): Promise<void>;
}