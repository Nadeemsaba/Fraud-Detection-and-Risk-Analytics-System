#include "server.h"
#include "fraud_engine.h"
#include "auth.h"
#include "jwt.h"
#include "crow.h"

void startServer(Database& db) {

    crow::SimpleApp app;
    FraudEngine engine;

    // ================= LOGIN =================
    CROW_ROUTE(app, "/api/login").methods("POST"_method)
    ([&db](const crow::request& req) {

        auto body = crow::json::load(req.body);
        if (!body)
            return crow::response(400, "Invalid JSON");

        std::string email = body["email"].s();
        std::string password = body["password"].s();

        User* user = db.loginUser(email, password);

        if (!user)
            return crow::response(401, "Invalid credentials");

        std::string token = createJWT(user->id, user->role);

        crow::json::wvalue res;
        res["token"] = token;
        res["role"] = user->role;
        res["name"] = user->name;

        return crow::response(res);
    });

    CROW_ROUTE(app, "/api/register").methods("POST"_method)
([&db](const crow::request& req){

    auto body = crow::json::load(req.body);
    if (!body) return crow::response(400);

    std::string name = body["name"].s();
    std::string email = body["email"].s();
    std::string password = body["password"].s();
    std::string country = body["country"].s();

    bool success = db.registerUser(name,email,password,country,"USER");

    if (!success)
        return crow::response(400,"User exists");

    return crow::response(200,"Registered");
});


    // ================= TRANSACTION (PROTECTED) =================
CROW_ROUTE(app, "/api/transaction").methods("POST"_method)
([&db, &engine](const crow::request& req) {

    auto authHeader = req.get_header_value("Authorization");
    if (authHeader.empty()) return crow::response(401);

    std::string token = authHeader.substr(7);

    int userId;
    std::string role;

    if (!verifyJWT(token, userId, role))
        return crow::response(403);

    auto body = crow::json::load(req.body);
    if (!body) return crow::response(400);

    double amount = body["amount"].d();
    std::string type = body["type"].s();
    std::string txnCountry = body["country"].s();
    std::string deviceId = body["deviceId"].s();
    std::string ip = body["ip"].s();

    User* user = db.getUserById(userId);
    if (!user) return crow::response(404);

    int txnCount = db.getUserTransactionCount(userId);
    double avgAmount = db.getUserAverageAmount(userId);
    bool newDevice = db.isNewDevice(userId, deviceId);

    RiskResult result = engine.evaluate(
        amount,
        user->country,
        txnCountry,
        newDevice,
        txnCount,
        avgAmount
    );

    db.insertTransaction(
        userId,
        amount,
        type,
        txnCountry,
        deviceId,
        ip,
        result.score,
        result.level,
        result.fraud
    );

    if (result.score >= 80)
        db.reduceTrustScore(userId, 20);
    else if (result.score >= 60)
        db.reduceTrustScore(userId, 10);
    else if (result.score >= 40)
        db.reduceTrustScore(userId, 5);
    else
        db.increaseTrustScore(userId, 3);

    crow::json::wvalue res;
    res["riskScore"] = result.score;
    res["riskLevel"] = result.level;
    res["isFraud"] = result.fraud;

    return crow::response(res);
});

    // ================= ADMIN =================
    CROW_ROUTE(app, "/api/admin").methods("GET"_method)
    ([&db](const crow::request& req){

        auto authHeader = req.get_header_value("Authorization");

        if (authHeader.empty())
            return crow::response(401);

        std::string token = authHeader.substr(7);

        int userId;
        std::string role;

        if (!verifyJWT(token, userId, role))
            return crow::response(403);
        std::cout << "JWT ROLE: " << role << std::endl;

        if (role != "ADMIN")
            return crow::response(403, "Access denied");

        crow::json::wvalue res;
        res["message"] = "Welcome Admin";

        return crow::response(res);
    });

    // ================= PROFILE =================
CROW_ROUTE(app, "/api/profile").methods("GET"_method)
([&db](const crow::request& req){

    auto authHeader = req.get_header_value("Authorization");
    if (authHeader.empty()) return crow::response(401);

    std::string token = authHeader.substr(7);

    int userId;
    std::string role;

    if (!verifyJWT(token, userId, role))
        return crow::response(403);

    User* user = db.getUserById(userId);
    if (!user) return crow::response(404);

    auto transactions = db.getLastTransactions(userId);

    crow::json::wvalue res;
    res["name"] = user->name;
    res["email"] = user->email;
    res["trustScore"] = user->trustScore;
    res["role"] = user->role;
    res["country"] = user->country;

    int index = 0;
    for (auto& t : transactions) {
        res["transactions"][index]["amount"] = t.amount;
        res["transactions"][index]["riskScore"] = t.riskScore;
        res["transactions"][index]["riskLevel"] = t.riskLevel;
        res["transactions"][index]["isFraud"] = t.isFraud;
        res["transactions"][index]["date"] = t.date;
        index++;
    }

    return crow::response(res);
});
CROW_ROUTE(app, "/api/admin/users").methods("GET"_method)
([&db](const crow::request& req){

    auto authHeader = req.get_header_value("Authorization");
    if (authHeader.empty()) return crow::response(401);

    std::string token = authHeader.substr(7);

    int userId;
    std::string role;

    if (!verifyJWT(token, userId, role))
        return crow::response(403);

    if (role != "ADMIN")
        return crow::response(403);

    auto users = db.getAllUsers();

    crow::json::wvalue res;
    int index = 0;

    for (auto& u : users) {
        res[index]["id"] = u.id;
        res[index]["name"] = u.name;
        res[index]["email"] = u.email;
        res[index]["trustScore"] = u.trustScore;
        res[index]["role"] = u.role;
        res[index]["isBlocked"] = u.isBlocked;
        index++;
    }

    return crow::response(res);
});

// ================= USER TRANSACTIONS =================
CROW_ROUTE(app, "/api/user/transactions").methods("GET"_method)
([&db](const crow::request& req){

    auto authHeader = req.get_header_value("Authorization");
    if (authHeader.empty()) return crow::response(401);

    std::string token = authHeader.substr(7);

    int userId;
    std::string role;

    if (!verifyJWT(token, userId, role))
        return crow::response(403);

    auto list = db.getLastTransactions(userId);

    std::string json = "[";

    for (size_t i = 0; i < list.size(); i++) {

        json += "{";
        json += "\"amount\":" + std::to_string(list[i].amount) + ",";
        json += "\"riskScore\":" + std::to_string(list[i].riskScore) + ",";
        json += "\"riskLevel\":\"" + list[i].riskLevel + "\",";
        json += "\"isFraud\":" + std::to_string(list[i].isFraud) + ",";
        json += "\"date\":\"" + list[i].date + "\"";
        json += "}";

        if (i != list.size() - 1)
            json += ",";
    }

    json += "]";

    crow::response res;
    res.code = 200;
    res.set_header("Content-Type", "application/json");
    res.body = json;

    return res;
});

CROW_ROUTE(app, "/api/admin/analytics").methods("GET"_method)
([&db](const crow::request& req){

    auto users = db.getAllUsers();

    crow::json::wvalue res;

    int i = 0;
    for(auto& u : users){
        res[i]["name"] = u.name;
        res[i]["trustScore"] = u.trustScore;
        i++;
    }

    return crow::response(res);
});
CROW_ROUTE(app, "/api/admin/toggle-block/<int>").methods("POST"_method)
([&db](const crow::request& req, int userId){

    auto authHeader = req.get_header_value("Authorization");
    if(authHeader.empty()) return crow::response(401);

    std::string token = authHeader.substr(7);

    int adminId;
    std::string role;

    if(!verifyJWT(token, adminId, role) || role != "ADMIN")
        return crow::response(403);

    std::string sql =
        "UPDATE users SET is_blocked = CASE WHEN is_blocked=1 THEN 0 ELSE 1 END WHERE id="
        + std::to_string(userId);

    sqlite3_exec(db.db, sql.c_str(), 0,0,0);

    return crow::response(200);
});


    std::cout << "Server running on http://localhost:18080\n";
    app.port(18080).multithreaded().run();
}

