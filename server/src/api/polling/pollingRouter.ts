import { OpenAPIRegistry } from "@asteasolutions/zod-to-openapi";
import express, { type Request, type Response, type Router } from "express";
import { z } from "zod";

import { createApiResponse } from "@/api-docs/openAPIResponseBuilders";
import { ServiceResponse } from "@/common/models/serviceResponse";
import { handleServiceResponse } from "@/common/utils/httpHandlers";

export const pollingRegistry = new OpenAPIRegistry();
export const pollingRouter: Router = express.Router();

pollingRegistry.registerPath({
  method: "get",
  path: "/polling",
  tags: ["Polling Server"],
  responses: createApiResponse(z.null(), "Success"),
});

pollingRouter.get("/", (_req: Request, res: Response) => {
  return res.send("do this!");
});
