import { OpenAPIRegistry } from "@asteasolutions/zod-to-openapi";
import express, { type Request, type Response, type Router } from "express";
import { z } from "zod";

import { createApiResponse } from "@/api-docs/openAPIResponseBuilders";
import { ServiceResponse } from "@/common/models/serviceResponse";
import { handleServiceResponse } from "@/common/utils/httpHandlers";
import { Database } from "@/database";
import { Queue, QueueStatus } from "@/queue";

export const queueRegistry = new OpenAPIRegistry();
export const queueRouter: Router = express.Router();

queueRegistry.registerPath({
    method: "get",
    path: "/queue",
    tags: ["Queue Server"],
    responses: createApiResponse(z.null(), "Success"),
});

queueRouter.post("/set/:status", async (req: Request, res: Response) => {
    const authHeader = req.headers.authorization;
    const status = parseInt(req.params.status) as QueueStatus;

    if (!authHeader) {
        return res.status(401).json({ error: "Missing Authorization header" });
    }

    if(!status) {
        return res.status(401).json({ error: "Missing status param" });
    }

    if (!await Database.tokenHasRole(authHeader, 'admin')) {
        return res.status(403).json({ error: "Token or role not valid for this action." });
    }

    await Database.setQueueStatus(status);

    return res.status(200).json({ message: 'Successfully set status' });
});