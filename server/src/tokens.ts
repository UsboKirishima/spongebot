import { QuickDB } from "quick.db";
import { Database } from "./database";
import { Hierarchy, HierarchyRole } from "./hierarchy";

/**
 * Differences between Login and Token:
 *  - Login: is an object that implements ILogin type 
 *          so it contains a token and a role
 *  - Token: is the authentication string contained in Login 
 *          and it has a role 
 */


export interface ILogin {
    token: string;
    role: HierarchyRole;
}

export class TokenManager {
    private static db: QuickDB = Database.db;


    /**
         * Used to add token to database without any validation
         * @param token 
         * @param role 
         */
    public static async forceInsertToken(token: string, role: HierarchyRole): Promise<void> {
        await this.db.push("logins", { token, role });
    }

    /**
     * Get if database contains a provided token 
     * @param token 
     * @returns Promise<boolean>
     */
    public static async isTokenRecognized(token: string): Promise<boolean> {
        const logins: ILogin[] = (await this.db.get("logins")) || [];
        return logins.some((login) => login.token === token);
    }


    /**
     * Check if the token provided has a specified role
     * @param token 
     * @param role 
     * @returns Promise<boolean>
     */
    public static async tokenHasRole(token: string, role: HierarchyRole): Promise<boolean> {
        if (!Hierarchy.validateToken(token)) return false;

        const logins: ILogin[] = (await this.db.get("logins")) || [];
        return logins.some((login) => login.token === token && login.role === role);
    }


    /**
     * Create a new token and add it to database
     * @param role 
     * @returns 
     */
    public static async createAndSaveLogin(role: HierarchyRole): Promise<ILogin | null> {
        const token = Hierarchy.generateToken(role);
        if (!token) return null;

        const login: ILogin = { token, role };
        await this.db.push("logins", login);

        const logins: ILogin[] = (await this.db.get("logins")) || [];
        return logins.find((l) => l.token === token) || null;
    }

    /**
     * Get all tookens in database, else empty array
     * @returns ILogin[] | []
     */
    public static async getAllTokens(): Promise<ILogin[]> {
        return (await this.db.get("logins")) || [];
    }

    // Some others shitty mothods for future purposes?

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