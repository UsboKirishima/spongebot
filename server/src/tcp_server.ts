import net from 'net';
import { logger } from './server';
import { Database } from './database';
import { SocketManager } from './clients';

export const tcpServer = net.createServer(async (socket) => {

    const ipAdrr = socket.remoteAddress?.toString();

    logger.info("New Client connected: " + ipAdrr);

    if (ipAdrr === undefined || !ipAdrr)
        return logger.info('Failed to fetch ip address');

    await SocketManager.addSocket(ipAdrr, socket).then(() => {
        logger.info("New ip address added to database: " + ipAdrr);
    });


    socket.on("data", (data) => {
        console.log(`Received from client: ${data.toString()}`);
    })

    socket.on("end", async () => {
        await SocketManager.removeSocket(ipAdrr);
        logger.info("TCP Client disconnected");
    });

    socket.on("error", (err) => {
        logger.error("TCP Socket error:", err.message);
    });
});
