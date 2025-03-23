import express, { type Request, type Response, type Router } from "express";
import { Database } from "@/database";
import { TokenManager } from "@/tokens";
import { Info } from "@/info";
import path from "path";

export const updateRouter: Router = express.Router();


updateRouter.get("/get", async (req: Request, res: Response) => {
    return res.status(200).sendFile('../../../spongebot')
});