import net from 'net';
import { logger } from './server';

export const tcpServer = net.createServer((socket) => {
    logger.info("New Client connected: " + socket.remoteAddress?.toString());

    socket.on("data", (data) => {
        console.log(`Received from client: ${data.toString()}`);


        socket.write("Message received!\n");
    });

    socket.on("end", () => {
        logger.info("TCP Client disconnected");
    });

    socket.on("error", (err) => {
        logger.error("TCP Socket error:", err.message);
    });
});
