import { OpenAPIRegistry } from "@asteasolutions/zod-to-openapi";
import express, { type Request, type Response, type Router } from "express";
import { z } from "zod";

import { createApiResponse } from "@/api-docs/openAPIResponseBuilders";
import { ServiceResponse } from "@/common/models/serviceResponse";
import { handleServiceResponse } from "@/common/utils/httpHandlers";
import { Database } from "@/database";
import { QueueStatus } from "@/queue";

export const pollingRegistry = new OpenAPIRegistry();
export const pollingRouter: Router = express.Router();

pollingRegistry.registerPath({
	method: "get",
	path: "/polling",
	tags: ["Polling Server"],
	responses: createApiResponse(z.null(), "Success"),
});

pollingRouter.get("/", async (_req: Request, res: Response) => {
	const ip = _req.ip ?? ""; 
	
	if (ip && !(await Database.isIpAlreadyInDb(ip))) {
		await Database.addIpAddress(ip);
	}

	const queueStatus = await Database.getQueueStatus();
	return res.send(`${queueStatus || QueueStatus.IDLE}`);
});