import express, { type Request, type Response, type Router } from "express";
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

    if(!(/^[0-9]{1}$/.test(command)))
        return res.status(401).json({ error: "Command must be number 0-9" });

    if (!await TokenManager.tokenHasRole(authHeader, 'admin')) {
        return res.status(403).json({ error: "Token or role not valid for this action." });
    }

    await SocketManager.broadcastMessage(`${command}\n`);

    return res.status(200).json({ message: 'Successfully set command' });
});