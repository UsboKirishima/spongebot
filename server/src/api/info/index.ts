import express, { type Request, type Response, type Router } from "express";
import { Database } from "@/database";
import { TokenManager } from "@/tokens";
import { Info } from "@/info";

export const infoRouter: Router = express.Router();


infoRouter.get("/get", async (req: Request, res: Response) => {
    const authHeader = req.headers.authorization;

    if (!authHeader) {
        return res.status(401).json({ error: "Missing Authorization header" });
    }

    if (!await TokenManager.tokenHasRole(authHeader, 'admin')) {
        return res.status(403).json({ error: "Token or role not valid for this action." });
    }

    const info = new Info();
    await info.init();

    return res.status(200).json(info.toJSON());
});