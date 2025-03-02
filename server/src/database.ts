import { QuickDB } from "quick.db";
import { HierarchyRole } from "./structures/hierarchy";
import { Hierarchy } from "./structures/hierarchy";

export interface ILogin {
    token: string;
    role: HierarchyRole;
}

export class Database {
    private static db: QuickDB = new QuickDB();

    public static async forceInsertToken(token: string, role: HierarchyRole): Promise<void> {
        await this.db.push("logins", { token, role });
    }

    public static async isTokenRecognized(token: string): Promise<boolean> {
        const logins: ILogin[] = (await this.db.get("logins")) || [];
        return logins.some((login) => login.token === token);
    }

    public static async tokenHasRole(token: string, role: HierarchyRole): Promise<boolean> {
        if (!Hierarchy.validateToken(token)) return false;

        const logins: ILogin[] = (await this.db.get("logins")) || [];
        return logins.some((login) => login.token === token && login.role === role);
    }

    public static async createAndSaveLogin(role: HierarchyRole): Promise<ILogin | null> {
        const token = Hierarchy.generateToken(role);
        if (!token) return null;

        const login: ILogin = { token, role };
        await this.db.push("logins", login);

        const logins: ILogin[] = (await this.db.get("logins")) || [];
        return logins.find((l) => l.token === token) || null;
    }

    public static async getAllTokens(): Promise<ILogin[]> {
        return (await this.db.get("logins")) || [];
    }

    public static async getTokensByRole(role: HierarchyRole): Promise<ILogin[]> {
        const logins: ILogin[] = (await this.db.get("logins")) || [];
        return logins.filter((login) => login.role === role);
    }

    public static async deleteToken(token: string): Promise<boolean> {
        const logins: ILogin[] = (await this.db.get("logins")) || [];
        const updatedLogins = logins.filter((login) => login.token !== token);
        
        await this.db.set("logins", updatedLogins);
        return logins.length !== updatedLogins.length; 
    }

    public static async deleteTokensByRole(role: HierarchyRole): Promise<void> {
        const logins: ILogin[] = (await this.db.get("logins")) || [];
        const updatedLogins = logins.filter((login) => login.role !== role);

        await this.db.set("logins", updatedLogins);
    }

    public static async clearAllTokens(): Promise<void> {
        await this.db.delete("logins");
    }
}
