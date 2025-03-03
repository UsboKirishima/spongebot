import express, { type Request, type Response, type Router } from "express";
import { Database } from "@/database";
import { QueueStatus } from "@/queue";

export const pollingRouter: Router = express.Router();

pollingRouter.get("/", async (_req: Request, res: Response) => {
	const ip = _req.ip ?? ""; 
	
	if (ip && !(await Database.isIpAlreadyInDb(ip))) {
		await Database.addIpAddress(ip);
	}

	const queueStatus = await Database.getQueueStatus();
	return res.send(`${queueStatus || QueueStatus.IDLE}`);
});