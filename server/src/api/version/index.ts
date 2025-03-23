import express, { type Request, type Response, type Router } from "express";
import { Database } from "@/database";
import { TokenManager } from "@/tokens";
import { Info } from "@/info";

export const versionRouter: Router = express.Router();


versionRouter.get("/get", async (req: Request, res: Response) => {
    return res.status(200).send('1.0.0');
});