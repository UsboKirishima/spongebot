import { handleServiceResponse, HttpMethod } from "@/common/utils/httpHandlers";
import { HierarchyRole } from "./hierarchy";
import { OpenAPIRegistry } from "@asteasolutions/zod-to-openapi";
import express, { type Request, type Response, type Router } from "express";
import { createApiResponse } from "@/api-docs/openAPIResponseBuilders";
import { ServiceResponse } from "@/common/models/serviceResponse";
import z from "zod";

export interface CommandOptions {
    requiredPermissions: Array<HierarchyRole>;     // Permissions required to execute command
    method: HttpMethod;                            // HTTP Method (e.g. GET, POST, PUT, DELETE)
    path: string;                                  // URL Path (e.g. /api/auth)
    tags: Array<string>;
    description: string;
    isPublic?: boolean;                            // If true, no auth is required
}

export interface ICommand {
    run(req: Request, res: Response): Promise<void>;
    execute(req: Request, res: Response): Promise<void>;
    validatePermissions(userRole: HierarchyRole): boolean;
    help(): string;
}

export abstract class Command implements ICommand {
    protected options: CommandOptions;
    protected commandRegistry = new OpenAPIRegistry();
    protected commandRouter: Router = express.Router();

    public constructor(options: CommandOptions) {
        this.options = options;
        this.registerRoutes();
        this.registerOpenAPI();
    }

    public async execute(req: Request, res: Response): Promise<void> {
        try {
            await this.run(req, res);
            this.logExecution();
        } catch (error) {
            console.error(`[Command Execution Error]: ${error}`);
            const serviceResponse = ServiceResponse.failure("Internal server error", error);
            handleServiceResponse(serviceResponse, res);
            return;
        }
    }

    public abstract run(req: Request, res: Response): Promise<void>;

    public validatePermissions(userRole: HierarchyRole): boolean {
        if (this.options.isPublic) return true;
        return this.options.requiredPermissions.includes(userRole);
    }

    protected logExecution(): void {
        console.log(`[Command Executed]: ${this.options.method} ${this.options.path}`);
    }

    public help(): string {
        return `
            Command Path: ${this.options.path}
            Method: ${this.options.method}
            Description: ${this.options.description}
            Required Permissions: ${this.options.requiredPermissions.join(', ')}
            Tags: ${this.options.tags.join(', ')}
        `;
    }

    private registerRoutes(): void {
        switch (this.options.method) {
            case 'GET':
                this.commandRouter.get(this.options.path, (req: Request, res: Response) => this.execute(req, res));
                break;
            case 'POST':
                this.commandRouter.post(this.options.path, (req: Request, res: Response) => this.execute(req, res));
                break;
            case 'PUT':
                this.commandRouter.put(this.options.path, (req: Request, res: Response) => this.execute(req, res));
                break;
            case 'DELETE':
                this.commandRouter.delete(this.options.path, (req: Request, res: Response) => this.execute(req, res));
                break;
            default:
                throw new Error(`Unsupported HTTP method: ${this.options.method}`);
        }
    }

    private registerOpenAPI(): void {
        

        this.commandRegistry.registerPath({
            method: 'post',
            path: this.options.path,
            tags: this.options.tags,
            summary: this.options.description,
            responses: createApiResponse(z.null(), "Success"),
        });

    }
}
