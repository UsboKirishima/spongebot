import net from 'net';
import { logger } from '../server';
import { Database } from '../database';
import { SocketManager } from '../clients';
import { Command, CommandType } from '../command';

export const tcpServer = net.createServer(async (socket) => {

    const ipAdrr = socket.remoteAddress?.toString();

    //const commandBuffer = Buffer.from([1 << 0]);
    //const commandBuffer = Buffer.from([1 << 2, 0x0c, 0x51, 0x5d, 0x2b, 0xb6, 0x50, 0x0A])

    //let bufferToSend = [1 << 2, 30, 81, 93, 43, 182, 80];
    //const commandBuffer = Buffer.from([...bufferToSend, 0x0A]) // Push new line char '\n': 0x0A
    //socket.write(commandBuffer);

    new Command(CommandType.HELLO, 0, '0.0.0.0', 1, socket).send();

    logger.info("New Client connected: " + ipAdrr);

    if (ipAdrr === undefined || !ipAdrr)
        return logger.info('Failed to fetch ip address');

    await SocketManager.addSocket(ipAdrr, socket).then(() => {
        logger.info("New ip address added to database: " + ipAdrr);
    });


    socket.on("data", (data) => {
        logger.info(`Received from client: ${data.toString()}`);
    })

    socket.on("end", async () => {
        await SocketManager.removeSocket(ipAdrr);
        logger.info("TCP Client disconnected");
    });

    socket.on("error", (err) => {
        logger.error("TCP Socket error:", err.message);
    });
});
