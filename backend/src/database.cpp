#include "database.h"
#include "auth.h"
#include <iostream>
#include <ctime>
#include <cstdlib>

Database::Database() {
    if (sqlite3_open("fraud.db", &db))
        std::cout << "DB open failed\n";
}

void Database::init() {
    createTables();
    seedData();
    
}

void Database::createTables() {

    const char* users = R"(
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT,
            email TEXT UNIQUE,
            password TEXT,
            country TEXT,
            role TEXT,
            trust_score INTEGER DEFAULT 100,
            is_blocked INTEGER DEFAULT 0,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP
        );
    )";

    const char* transactions = R"(
        CREATE TABLE IF NOT EXISTS transactions (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER,
            amount REAL,
            type TEXT,
            country TEXT,
            device_id TEXT,
            ip_address TEXT,
            risk_score INTEGER,
            risk_level TEXT,
            is_fraud INTEGER,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP
        );
    )";

            sqlite3_exec(db,
        "CREATE TABLE IF NOT EXISTS devices ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "userId INTEGER,"
        "deviceId TEXT);",
        0,0,0);

    

    sqlite3_exec(db, users, 0, 0, 0);
    sqlite3_exec(db, transactions, 0, 0, 0);
}
void Database::seedData() {

    std::string check = "SELECT COUNT(*) FROM users;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, check.c_str(), -1, &stmt, 0);
    sqlite3_step(stmt);
    int count = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);

    if(count > 0) return;

    std::cout << "Full manual seeding started...\n";

    // ================= ADMIN =================
    registerUser("System Admin","admin@fraud.com","admin123","India","ADMIN");

    // ================= USERS =================
    registerUser("Rahul Sharma","rahul@mail.com","rahul123","India","USER");
    registerUser("Ayesha Khan","ayesha@mail.com","ayesha123","UAE","USER");
    registerUser("John Miller","john@mail.com","john123","USA","USER");
    registerUser("Li Wei","li@mail.com","li123","China","USER");
    registerUser("Carlos Diaz","carlos@mail.com","carlos123","Brazil","USER");
    registerUser("Elena Petrova","elena@mail.com","elena123","Russia","USER");
    registerUser("Fatima Noor","fatima@mail.com","fatima123","Pakistan","USER");
    registerUser("David Smith","david@mail.com","david123","UK","USER");
    registerUser("Kenji Tanaka","kenji@mail.com","kenji123","Japan","USER");
    registerUser("Omar Hassan","omar@mail.com","omar123","Egypt","USER");
    registerUser("Maria Gonzalez","maria@mail.com","maria123","Spain","USER");

    // ================= TRUST SCORES =================
    sqlite3_exec(db,"UPDATE users SET trust_score=100 WHERE email='rahul@mail.com';",0,0,0);
    sqlite3_exec(db,"UPDATE users SET trust_score=100 WHERE email='li@mail.com';",0,0,0);
    sqlite3_exec(db,"UPDATE users SET trust_score=100 WHERE email='kenji@mail.com';",0,0,0);

    sqlite3_exec(db,"UPDATE users SET trust_score=78 WHERE email='ayesha@mail.com';",0,0,0);
    sqlite3_exec(db,"UPDATE users SET trust_score=60 WHERE email='john@mail.com';",0,0,0);
    sqlite3_exec(db,"UPDATE users SET trust_score=55 WHERE email='carlos@mail.com';",0,0,0);
    sqlite3_exec(db,"UPDATE users SET trust_score=70 WHERE email='elena@mail.com';",0,0,0);
    sqlite3_exec(db,"UPDATE users SET trust_score=82 WHERE email='fatima@mail.com';",0,0,0);
    sqlite3_exec(db,"UPDATE users SET trust_score=58 WHERE email='david@mail.com';",0,0,0);
    sqlite3_exec(db,"UPDATE users SET trust_score=74 WHERE email='omar@mail.com';",0,0,0);
    sqlite3_exec(db,"UPDATE users SET trust_score=67 WHERE email='maria@mail.com';",0,0,0);

    auto getId = [&](std::string email){
        sqlite3_stmt* s;
        std::string q="SELECT id FROM users WHERE email='"+email+"'";
        sqlite3_prepare_v2(db,q.c_str(),-1,&s,0);
        int id=-1;
        if(sqlite3_step(s)==SQLITE_ROW)
            id=sqlite3_column_int(s,0);
        sqlite3_finalize(s);
        return id;
    };

    int rahul=getId("rahul@mail.com");
    int ayesha=getId("ayesha@mail.com");
    int john=getId("john@mail.com");
    int li=getId("li@mail.com");
    int carlos=getId("carlos@mail.com");
    int elena=getId("elena@mail.com");
    int fatima=getId("fatima@mail.com");
    int david=getId("david@mail.com");
    int kenji=getId("kenji@mail.com");
    int omar=getId("omar@mail.com");
    int maria=getId("maria@mail.com");
    std::cout << "Rahul ID: " << rahul << std::endl;
    std::cout << "Ayesha ID: " << ayesha << std::endl;

    // ================== TRANSACTIONS ==================

    // Rahul
    insertTransaction(rahul,5000,"ONLINE","India","WEB","1.1.1.1",10,"LOW",false);
    insertTransaction(rahul,8000,"ONLINE","India","WEB","1.1.1.1",15,"LOW",false);
    insertTransaction(rahul,12000,"ONLINE","India","MOBILE","1.1.1.2",25,"MEDIUM",false);
    insertTransaction(rahul,15000,"ONLINE","India","WEB","1.1.1.1",30,"MEDIUM",false);
    insertTransaction(rahul,9000,"ONLINE","India","WEB","1.1.1.1",12,"LOW",false);
    insertTransaction(rahul,30000,"ONLINE","India","NEW_DEVICE","1.1.1.3",60,"HIGH",false);
    insertTransaction(rahul,75000,"ONLINE","India","NEW_DEVICE","1.1.1.3",95,"FRAUD",true);
    insertTransaction(rahul,6000,"ONLINE","India","WEB","1.1.1.1",15,"LOW",false);
    insertTransaction(rahul,11000,"ONLINE","India","MOBILE","1.1.1.2",25,"MEDIUM",false);
    insertTransaction(rahul,45000,"ONLINE","India","NEW_DEVICE","1.1.1.3",70,"HIGH",false);

    // Ayesha
    insertTransaction(ayesha,10000,"ONLINE","UAE","WEB","2.2.2.1",15,"LOW",false);
    insertTransaction(ayesha,15000,"ONLINE","UAE","MOBILE","2.2.2.2",20,"LOW",false);
    insertTransaction(ayesha,85000,"ONLINE","Russia","NEW_DEVICE","2.2.2.3",96,"FRAUD",true);
    insertTransaction(ayesha,12000,"ONLINE","UAE","WEB","2.2.2.1",18,"LOW",false);
    insertTransaction(ayesha,18000,"ONLINE","USA","MOBILE","2.2.2.2",55,"HIGH",false);
    insertTransaction(ayesha,14000,"ONLINE","UAE","WEB","2.2.2.1",20,"LOW",false);
    insertTransaction(ayesha,25000,"ONLINE","UAE","WEB","2.2.2.1",40,"MEDIUM",false);
    insertTransaction(ayesha,60000,"ONLINE","China","NEW_DEVICE","2.2.2.4",88,"FRAUD",true);
    insertTransaction(ayesha,13000,"ONLINE","UAE","WEB","2.2.2.1",20,"LOW",false);
    insertTransaction(ayesha,20000,"ONLINE","UAE","MOBILE","2.2.2.2",25,"MEDIUM",false);

        // ================= JOHN =================
    insertTransaction(john,30000,"ONLINE","USA","WEB","3.3.3.1",20,"LOW",false);
    insertTransaction(john,45000,"ONLINE","USA","WEB","3.3.3.1",40,"MEDIUM",false);
    insertTransaction(john,90000,"ONLINE","Brazil","NEW_DEVICE","3.3.3.2",97,"FRAUD",true);
    insertTransaction(john,25000,"ONLINE","USA","MOBILE","3.3.3.3",25,"LOW",false);
    insertTransaction(john,70000,"ONLINE","USA","NEW_DEVICE","3.3.3.4",75,"HIGH",false);
    insertTransaction(john,20000,"ONLINE","USA","WEB","3.3.3.1",18,"LOW",false);
    insertTransaction(john,22000,"ONLINE","USA","MOBILE","3.3.3.3",22,"LOW",false);
    insertTransaction(john,50000,"ONLINE","USA","NEW_DEVICE","3.3.3.4",65,"HIGH",false);
    insertTransaction(john,18000,"ONLINE","USA","WEB","3.3.3.1",15,"LOW",false);
    insertTransaction(john,15000,"ONLINE","USA","WEB","3.3.3.1",10,"LOW",false);

    // ================= LI =================
    insertTransaction(li,6000,"ONLINE","China","WEB","4.4.4.1",10,"LOW",false);
    insertTransaction(li,8000,"ONLINE","China","WEB","4.4.4.1",15,"LOW",false);
    insertTransaction(li,10000,"ONLINE","China","MOBILE","4.4.4.2",20,"LOW",false);
    insertTransaction(li,15000,"ONLINE","China","WEB","4.4.4.1",25,"MEDIUM",false);
    insertTransaction(li,20000,"ONLINE","China","WEB","4.4.4.1",30,"MEDIUM",false);
    insertTransaction(li,7000,"ONLINE","China","WEB","4.4.4.1",12,"LOW",false);
    insertTransaction(li,9000,"ONLINE","China","MOBILE","4.4.4.2",18,"LOW",false);
    insertTransaction(li,25000,"ONLINE","Japan","NEW_DEVICE","4.4.4.3",60,"HIGH",false);
    insertTransaction(li,12000,"ONLINE","China","WEB","4.4.4.1",20,"LOW",false);
    insertTransaction(li,18000,"ONLINE","China","WEB","4.4.4.1",25,"MEDIUM",false);

    // ================= CARLOS =================
    insertTransaction(carlos,15000,"ONLINE","Brazil","WEB","5.5.5.1",20,"LOW",false);
    insertTransaction(carlos,30000,"ONLINE","Brazil","MOBILE","5.5.5.2",40,"MEDIUM",false);
    insertTransaction(carlos,95000,"ONLINE","USA","NEW_DEVICE","5.5.5.3",99,"FRAUD",true);
    insertTransaction(carlos,25000,"ONLINE","Brazil","WEB","5.5.5.1",30,"MEDIUM",false);
    insertTransaction(carlos,40000,"ONLINE","Brazil","NEW_DEVICE","5.5.5.4",70,"HIGH",false);
    insertTransaction(carlos,20000,"ONLINE","Brazil","WEB","5.5.5.1",20,"LOW",false);
    insertTransaction(carlos,18000,"ONLINE","Brazil","WEB","5.5.5.1",15,"LOW",false);
    insertTransaction(carlos,22000,"ONLINE","Brazil","MOBILE","5.5.5.2",25,"LOW",false);
    insertTransaction(carlos,50000,"ONLINE","Brazil","NEW_DEVICE","5.5.5.4",75,"HIGH",false);
    insertTransaction(carlos,16000,"ONLINE","Brazil","WEB","5.5.5.1",18,"LOW",false);

    // ================= ELENA =================
    insertTransaction(elena,12000,"ONLINE","Russia","WEB","6.6.6.1",15,"LOW",false);
    insertTransaction(elena,18000,"ONLINE","Russia","MOBILE","6.6.6.2",25,"MEDIUM",false);
    insertTransaction(elena,60000,"ONLINE","Germany","NEW_DEVICE","6.6.6.3",85,"HIGH",false);
    insertTransaction(elena,11000,"ONLINE","Russia","WEB","6.6.6.1",12,"LOW",false);
    insertTransaction(elena,14000,"ONLINE","Russia","WEB","6.6.6.1",20,"LOW",false);
    insertTransaction(elena,90000,"ONLINE","USA","NEW_DEVICE","6.6.6.4",98,"FRAUD",true);
    insertTransaction(elena,17000,"ONLINE","Russia","MOBILE","6.6.6.2",25,"MEDIUM",false);
    insertTransaction(elena,22000,"ONLINE","Russia","WEB","6.6.6.1",30,"MEDIUM",false);
    insertTransaction(elena,20000,"ONLINE","Russia","WEB","6.6.6.1",25,"LOW",false);
    insertTransaction(elena,15000,"ONLINE","Russia","WEB","6.6.6.1",18,"LOW",false);

    // ================= FATIMA =================
    insertTransaction(fatima,7000,"ONLINE","Pakistan","WEB","7.7.7.1",15,"LOW",false);
    insertTransaction(fatima,9000,"ONLINE","Pakistan","MOBILE","7.7.7.2",20,"LOW",false);
    insertTransaction(fatima,15000,"ONLINE","Pakistan","WEB","7.7.7.1",25,"MEDIUM",false);
    insertTransaction(fatima,20000,"ONLINE","India","NEW_DEVICE","7.7.7.3",60,"HIGH",false);
    insertTransaction(fatima,85000,"ONLINE","Russia","NEW_DEVICE","7.7.7.4",95,"FRAUD",true);
    insertTransaction(fatima,8000,"ONLINE","Pakistan","WEB","7.7.7.1",15,"LOW",false);
    insertTransaction(fatima,12000,"ONLINE","Pakistan","WEB","7.7.7.1",20,"LOW",false);
    insertTransaction(fatima,18000,"ONLINE","Pakistan","MOBILE","7.7.7.2",25,"MEDIUM",false);
    insertTransaction(fatima,22000,"ONLINE","Pakistan","WEB","7.7.7.1",30,"MEDIUM",false);
    insertTransaction(fatima,11000,"ONLINE","Pakistan","WEB","7.7.7.1",18,"LOW",false);

    // ================= DAVID =================
    insertTransaction(david,30000,"ONLINE","UK","WEB","8.8.8.1",25,"LOW",false);
    insertTransaction(david,45000,"ONLINE","UK","WEB","8.8.8.1",40,"MEDIUM",false);
    insertTransaction(david,90000,"ONLINE","USA","NEW_DEVICE","8.8.8.2",98,"FRAUD",true);
    insertTransaction(david,25000,"ONLINE","UK","MOBILE","8.8.8.3",30,"MEDIUM",false);
    insertTransaction(david,70000,"ONLINE","Germany","NEW_DEVICE","8.8.8.4",85,"HIGH",false);
    insertTransaction(david,20000,"ONLINE","UK","WEB","8.8.8.1",20,"LOW",false);
    insertTransaction(david,22000,"ONLINE","UK","MOBILE","8.8.8.3",25,"LOW",false);
    insertTransaction(david,50000,"ONLINE","UK","NEW_DEVICE","8.8.8.4",70,"HIGH",false);
    insertTransaction(david,18000,"ONLINE","UK","WEB","8.8.8.1",15,"LOW",false);
    insertTransaction(david,15000,"ONLINE","UK","WEB","8.8.8.1",12,"LOW",false);

    // ================= KENJI =================
    insertTransaction(kenji,10000,"ONLINE","Japan","WEB","9.9.9.1",10,"LOW",false);
    insertTransaction(kenji,12000,"ONLINE","Japan","WEB","9.9.9.1",15,"LOW",false);
    insertTransaction(kenji,15000,"ONLINE","Japan","MOBILE","9.9.9.2",20,"LOW",false);
    insertTransaction(kenji,18000,"ONLINE","Japan","WEB","9.9.9.1",25,"LOW",false);
    insertTransaction(kenji,20000,"ONLINE","Japan","WEB","9.9.9.1",30,"LOW",false);
    insertTransaction(kenji,13000,"ONLINE","Japan","WEB","9.9.9.1",15,"LOW",false);
    insertTransaction(kenji,16000,"ONLINE","Japan","MOBILE","9.9.9.2",20,"LOW",false);
    insertTransaction(kenji,14000,"ONLINE","Japan","WEB","9.9.9.1",18,"LOW",false);
    insertTransaction(kenji,17000,"ONLINE","Japan","WEB","9.9.9.1",22,"LOW",false);
    insertTransaction(kenji,19000,"ONLINE","Japan","WEB","9.9.9.1",25,"LOW",false);

    // ================= OMAR =================
    insertTransaction(omar,11000,"ONLINE","Egypt","WEB","10.10.10.1",15,"LOW",false);
    insertTransaction(omar,14000,"ONLINE","Egypt","MOBILE","10.10.10.2",20,"LOW",false);
    insertTransaction(omar,30000,"ONLINE","Egypt","NEW_DEVICE","10.10.10.3",65,"HIGH",false);
    insertTransaction(omar,85000,"ONLINE","USA","NEW_DEVICE","10.10.10.4",96,"FRAUD",true);
    insertTransaction(omar,15000,"ONLINE","Egypt","WEB","10.10.10.1",20,"LOW",false);
    insertTransaction(omar,17000,"ONLINE","Egypt","WEB","10.10.10.1",25,"LOW",false);
    insertTransaction(omar,20000,"ONLINE","Egypt","MOBILE","10.10.10.2",30,"MEDIUM",false);
    insertTransaction(omar,18000,"ONLINE","Egypt","WEB","10.10.10.1",25,"LOW",false);
    insertTransaction(omar,22000,"ONLINE","Egypt","WEB","10.10.10.1",35,"MEDIUM",false);
    insertTransaction(omar,16000,"ONLINE","Egypt","WEB","10.10.10.1",20,"LOW",false);

    // ================= MARIA =================
    insertTransaction(maria,9000,"ONLINE","Spain","WEB","11.11.11.1",12,"LOW",false);
    insertTransaction(maria,11000,"ONLINE","Spain","WEB","11.11.11.1",18,"LOW",false);
    insertTransaction(maria,13000,"ONLINE","Spain","MOBILE","11.11.11.2",20,"LOW",false);
    insertTransaction(maria,25000,"ONLINE","France","NEW_DEVICE","11.11.11.3",60,"HIGH",false);
    insertTransaction(maria,95000,"ONLINE","USA","NEW_DEVICE","11.11.11.4",99,"FRAUD",true);
    insertTransaction(maria,14000,"ONLINE","Spain","WEB","11.11.11.1",22,"LOW",false);
    insertTransaction(maria,16000,"ONLINE","Spain","WEB","11.11.11.1",25,"LOW",false);
    insertTransaction(maria,18000,"ONLINE","Spain","MOBILE","11.11.11.2",28,"MEDIUM",false);
    insertTransaction(maria,20000,"ONLINE","Spain","WEB","11.11.11.1",30,"MEDIUM",false);
    insertTransaction(maria,17000,"ONLINE","Spain","WEB","11.11.11.1",24,"LOW",false);


    // (Other users already structured above similarly)
    // For brevity: pattern continues exactly like previous answer
    // Each already defined user has 10 transactions

    std::cout << "Full manual seeding complete.\n";
}
int Database::getUserTransactionCount(int userId) {
    std::string sql = "SELECT COUNT(*) FROM transactions WHERE user_id = ?";
    sqlite3_stmt* stmt;

    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, userId);

    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return count;
}
double Database::getUserAverageAmount(int userId) {
    std::string sql = "SELECT AVG(amount) FROM transactions WHERE user_id = ?";
    sqlite3_stmt* stmt;

    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, userId);

    double avg = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        avg = sqlite3_column_double(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return avg;
}
void Database::insertTransaction(
    int userId,
    double amount,
    std::string type,
    std::string country,
    std::string deviceId,
    std::string ip,
    int riskScore,
    std::string riskLevel,
    bool isFraud
) {
    std::string sql =
        "INSERT INTO transactions (user_id, amount, type, country, device_id, ip_address, risk_score, risk_level, is_fraud) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)";

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

    sqlite3_bind_int(stmt, 1, userId);
    sqlite3_bind_double(stmt, 2, amount);
    sqlite3_bind_text(stmt, 3, type.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, country.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, deviceId.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, ip.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 7, riskScore);
    sqlite3_bind_text(stmt, 8, riskLevel.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 9, isFraud ? 1 : 0);

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}
bool Database::registerUser(std::string name,
                            std::string email,
                            std::string password,
                            std::string country,
                            std::string role) {

    std::string hashed = sha256(password);

    std::string sql =
        "INSERT INTO users (name,email,password,country,role) VALUES ('" +
        name + "','" + email + "','" + hashed + "','" + country + "','" + role + "');";

    return sqlite3_exec(db, sql.c_str(), 0, 0, 0) == SQLITE_OK;
}

User* Database::loginUser(std::string email, std::string password) {

    std::string hashed = sha256(password);

    std::string sql =
        "SELECT id,name,email,country,role,trust_score,is_blocked FROM users WHERE email='"
        + email + "' AND password='" + hashed + "';";

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0);

    if(sqlite3_step(stmt) == SQLITE_ROW){

        User* user = new User();
        user->id = sqlite3_column_int(stmt,0);
        user->name = (char*)sqlite3_column_text(stmt,1);
        user->email = (char*)sqlite3_column_text(stmt,2);
        user->country = (char*)sqlite3_column_text(stmt,3);
        user->role = (char*)sqlite3_column_text(stmt,4);
        user->trustScore = sqlite3_column_int(stmt,5);
        int blocked = sqlite3_column_int(stmt,6);
        if(blocked == 1){
            sqlite3_finalize(stmt);
            return nullptr;
        }


        sqlite3_finalize(stmt);
        std::cout << "LOGIN USER ROLE: " << user->role << std::endl;
        return user;
    }

    sqlite3_finalize(stmt);
    return nullptr;
}
User* Database::getUserById(int id) {

    std::string sql =
        "SELECT id,name,email,country,role,trust_score FROM users WHERE id=" +
        std::to_string(id);

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0);

    if (sqlite3_step(stmt) == SQLITE_ROW) {

        User* user = new User();
        user->id = sqlite3_column_int(stmt,0);
        user->name = (char*)sqlite3_column_text(stmt,1);
        user->email = (char*)sqlite3_column_text(stmt,2);
        user->country = (char*)sqlite3_column_text(stmt,3);
        user->role = (char*)sqlite3_column_text(stmt,4);
        user->trustScore = sqlite3_column_int(stmt,5);

        sqlite3_finalize(stmt);
        return user;
    }

    sqlite3_finalize(stmt);
    return nullptr;
}
std::vector<User> Database::getAllUsers() {

    std::vector<User> list;

    std::string sql = "SELECT id,name,email,country,role,trust_score,is_blocked FROM users";

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0);

    while (sqlite3_step(stmt) == SQLITE_ROW) {

        User u;
        u.id = sqlite3_column_int(stmt,0);
        u.name = (char*)sqlite3_column_text(stmt,1);
        u.email = (char*)sqlite3_column_text(stmt,2);
        u.country = (char*)sqlite3_column_text(stmt,3);
        u.role = (char*)sqlite3_column_text(stmt,4);
        u.trustScore = sqlite3_column_int(stmt,5);
        u.isBlocked = sqlite3_column_int(stmt,6);

        list.push_back(u);
    }

    sqlite3_finalize(stmt);
    return list;
}
bool Database::isNewDevice(int userId, std::string deviceId) {

    std::string sql =
        "SELECT id FROM devices WHERE userId=" +
        std::to_string(userId) +
        " AND deviceId='" + deviceId + "'";

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0);

    bool exists = (sqlite3_step(stmt) == SQLITE_ROW);

    sqlite3_finalize(stmt);

    if (!exists) {
        std::string insert =
            "INSERT INTO devices(userId,deviceId) VALUES(" +
            std::to_string(userId) + ",'" + deviceId + "')";
        sqlite3_exec(db, insert.c_str(), 0, 0, 0);
    }

    return !exists;
}


void Database::reduceTrustScore(int userId, int amount) {

    std::string sql =
        "UPDATE users SET trust_score = MAX(trust_score - " +
        std::to_string(amount) +
        ",0) WHERE id=" + std::to_string(userId);

    sqlite3_exec(db, sql.c_str(), 0, 0, 0);
}

void Database::increaseTrustScore(int userId, int amount) {

    std::string sql =
        "UPDATE users SET trust_score = MIN(trust_score + " +
        std::to_string(amount) +
        ",100) WHERE id=" + std::to_string(userId);

    sqlite3_exec(db, sql.c_str(), 0, 0, 0);
}
std::vector<Transaction> Database::getLastTransactions(int userId) {

    std::vector<Transaction> list;

    std::string sql =
        "SELECT amount,risk_score,risk_level,is_fraud,created_at "
        "FROM transactions WHERE user_id=" +
        std::to_string(userId) +
        " ORDER BY id DESC LIMIT 20";
    
    std::cout << "Fetching transactions for userId: " << userId << std::endl;
    std::cout << "SQL: " << sql << std::endl;

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0);

    while (sqlite3_step(stmt) == SQLITE_ROW) {

        Transaction t;
        t.amount = sqlite3_column_double(stmt,0);
        t.riskScore = sqlite3_column_int(stmt,1);
        t.riskLevel = (char*)sqlite3_column_text(stmt,2);
        t.isFraud = sqlite3_column_int(stmt,3);
        t.date = (char*)sqlite3_column_text(stmt,4);

        list.push_back(t);
    }

    sqlite3_finalize(stmt);
    return list;
}