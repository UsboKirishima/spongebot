import express, { type Request, type Response, type Router } from "express";
import { Database } from "@/database";
import { Queue, QueueStatus } from "@/queue";
import { TokenManager } from "@/tokens";
import { SocketManager } from "@/clients";

export const commandRouter: Router = express.Router();


commandRouter.post("/set/:command", async (req: Request, res: Response) => {
    const authHeader = req.headers.authorization;
    const command = req.params.command;

    if (!authHeader) {
        return res.status(401).json({ error: "Missing Authorization header" });
    }

    if(!command) {
        return res.status(401).json({ error: "Missing command param" });
    }

    if (!await TokenManager.tokenHasRole(authHeader, 'admin')) {
        return res.status(403).json({ error: "Token or role not valid for this action." });
    }

    await SocketManager.broadcastMessage("\nciao\n");

    return res.status(200).json({ message: 'Successfully set command' });
});