import { env } from "@/common/utils/envConfig";
import { app, logger } from "@/server";
import { Hierarchy } from "./structures/hierarchy";
import { Database } from "./database";

const server = app.listen(env.PORT, async () => {
  const { NODE_ENV, HOST, PORT } = env;
  logger.info(`Server (${NODE_ENV}) running on port http://${HOST}:${PORT}`);
  

  if(process.env.OWNER_TOKEN !== '' && !await Database.isTokenRecognized(process.env.OWNER_TOKEN as string)) {
    await Database.forceInsertToken(process.env.OWNER_TOKEN as string, 'admin');
  }
});

const onCloseSignal = () => {
  logger.info("sigint received, shutting down");
  server.close(() => {
    logger.info("server closed");
    process.exit();
  });
  setTimeout(() => process.exit(1), 10000).unref(); // Force shutdown after 10s
};

process.on("SIGINT", onCloseSignal);
process.on("SIGTERM", onCloseSignal);
