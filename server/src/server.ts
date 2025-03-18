import cors from "cors";
import express, { type Express } from "express";
import helmet from "helmet";
import { pino } from "pino";

import errorHandler from "@/common/middleware/errorHandler";
import rateLimiter from "@/common/middleware/rateLimiter";
import requestLogger from "@/common/middleware/requestLogger";
import { env } from "@/common/utils/envConfig";
import { tokenRouter } from "./api/token/tokenRouter";
import { queueRouter } from "./api/queue/queueRouter";
import { commandRouter } from "./api/command/commandRouter";
import { infoRouter } from "./api/info/infoRouter";
import { versionRouter } from "./api/version/versionRouter";
import { updateRouter } from "./api/update/updateRouter";

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
app.use("/queue", queueRouter);
app.use('/command', commandRouter);
app.use('/info', infoRouter);
app.use('/version', versionRouter);
app.use('/update', updateRouter);

app.use(errorHandler());

export { app, logger };
