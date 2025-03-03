import cors from "cors";
import express, { type Express } from "express";
import helmet from "helmet";
import { pino } from "pino";

import { healthCheckRouter } from "@/api/healthCheck/healthCheckRouter";
import errorHandler from "@/common/middleware/errorHandler";
import rateLimiter from "@/common/middleware/rateLimiter";
import requestLogger from "@/common/middleware/requestLogger";
import { env } from "@/common/utils/envConfig";
import { tokenRouter } from "./api/token/tokenRouter";
import { pollingRouter } from "@/api/polling/pollingRouter";
import { queueRouter } from "./api/queue/queueRouter";

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
app.use("/health-check", healthCheckRouter);
app.use("/token", tokenRouter);
app.use("/polling", pollingRouter);
app.use("/queue", queueRouter);

app.use(errorHandler());

export { app, logger };
