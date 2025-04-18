import { env } from "@/common/utils/envConfig";
import { app, logger } from "@/server";

import { Hierarchy } from "@/hierarchy";
import { Database } from "@/database";
import { TokenManager } from "@/tokens";

import { tcpServer } from '@tcp/index';

// HTTP Server
const server = app.listen(env.HTTP_PORT, async () => {
  const { NODE_ENV, HOST, HTTP_PORT } = env;
  logger.info(`Server (${NODE_ENV}) running on port http://${HOST}:${HTTP_PORT}`);


  if (process.env.OWNER_TOKEN !== '' && !await TokenManager.isTokenRecognized(process.env.OWNER_TOKEN as string)) {
    await TokenManager.forceInsertToken(process.env.OWNER_TOKEN as string, 'admin');
  }
});


// TCP Server
const tcp = tcpServer.listen(env.TCP_PORT, env.TCP_HOST, () => {
  logger.info(`TCP Server listening on ${env.TCP_HOST}:${env.TCP_PORT}`);
});

const onCloseSignal = () => {
  logger.info("sigint received, shutting down");
  server.close(() => {
    logger.info("server closed");
    process.exit();
  });
  tcp.on('close', () => {
    logger.info("tcp closed");
    process.exit();
  })
  setTimeout(() => process.exit(1), 10000).unref(); // Force shutdown after 10s
};

process.on("SIGINT", onCloseSignal);
process.on("SIGTERM", onCloseSignal);
