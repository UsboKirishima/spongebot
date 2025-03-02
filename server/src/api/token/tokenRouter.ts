import { OpenAPIRegistry } from "@asteasolutions/zod-to-openapi";
import express, { type Request, type Response, type Router } from "express";
import { z } from "zod";

import { createApiResponse } from "@/api-docs/openAPIResponseBuilders";
import { ServiceResponse } from "@/common/models/serviceResponse";
import { handleServiceResponse } from "@/common/utils/httpHandlers";
import { Database, ILogin } from "@/database";
import { HierarchyRole } from "@/structures/hierarchy";

export const tokenRegistry = new OpenAPIRegistry();
export const tokenRouter: Router = express.Router();

tokenRegistry.registerPath({
    method: "post",
    path: "/token/new/{role}",
    tags: ["Create Access"],
    responses: createApiResponse(z.null(), "Success"),
});

tokenRouter.post("/new/:role", async (req: Request, res: Response) => {
    const authHeader = req.headers.authorization;
    const role = req.params.role as HierarchyRole;

    if (!authHeader) {
        return res.status(401).json({ error: "Missing Authorization header" });
    }


    if (!role)
        return res.status(403).json({ error: "Missing parameter `role` e.g. /token/new/customer" })

    if (!await Database.tokenHasRole(authHeader, 'admin')) {
        return res.status(403).json({ error: "Token or role not valid for this action." });
    }

    const login = await Database.createAndSaveLogin(role as HierarchyRole);

    return res.json({ token: login?.token, role: login?.role });
});
