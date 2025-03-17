/** 
 * @description ```markdown
 * # Things to know about permission management:
 *      1. Each role also has the permissions of those below it
 *      2. Each role is identified with its own label, so they cannot identify themselves as other roles
 *      3. Each role uses a different type of id and token
 * 
 * # Role hierarchy from the most powerful to the weakest:
 *      1. admin
 *      2. operator
 *      3. supervisor
 *      4. bot
 *      5. customer
 *  ```
*/
export type HierarchyRole = 'admin' // Full access
    | 'operator'                    // Commands execution and operational management (Accounts creations).
    | 'supervisor'                  // Read-only permissions and access to db and analytics
    | 'bot'                         // Communication with the server & data exchange
    | 'customer'                    // Base user (Access to attacking commands and something other)

interface TokenRegex {
    admin: RegExp;
    operator: RegExp;
    supervisor: RegExp;
    bot: RegExp;
    customer: RegExp;
}

export class Hierarchy {

    
    public static rolePrefixMap: Record<HierarchyRole, string> = {
        admin: 'AD',
        operator: 'OP',
        supervisor: 'SV',
        bot: 'BT',
        customer: 'CS'
    };
    
    public static genericTokenExp: RegExp = new RegExp(
        `^(${Object.values(Hierarchy.rolePrefixMap).join("|")})[a-fA-F0-9]{30}$`
    );;

    public static token: Record<HierarchyRole, RegExp> = Object.fromEntries(
        Object.entries(Hierarchy.rolePrefixMap).map(([role, prefix]) => [
            role,
            new RegExp(`^${prefix}[a-fA-F0-9]{30}$`)
        ])
    ) as Record<HierarchyRole, RegExp>;

    /**
     * Generates a token for the given role.
     * 
     * @param {HierarchyRole} role - The role for which to generate the token.
     * @returns {string | null} The generated token or null if the role is invalid.
     */
    public static generateToken(role: HierarchyRole): string | null {
        const prefix = this.rolePrefixMap[role];
        if (!prefix) return null;

        const hexNumber30: string = Array.from({ length: 30 }, () =>
            Math.floor(Math.random() * 16).toString(16)
        ).join('');

        return `${prefix}${hexNumber30}`;
    }

    public static validateToken(token: string): boolean {

        token = token.trim();

        if(token === undefined || token === null)
            return false;
        
        if(!this.genericTokenExp.test(token))
            return false;

        return true;
    }
};