#pragma once
#include <sqlite3.h>
#include <string>
#include <vector>

struct User {
    int id;
    std::string name;
    std::string email;
    std::string country;
    std::string role;
    int trustScore;
    int isBlocked;
};
struct Transaction {
    double amount;
    int riskScore;
    std::string riskLevel;
    bool isFraud;
    std::string date;
};

class Database {
public:
    sqlite3* db;

    Database();
    void init();
    void createTables();
    void seedData();

    // Auth
    bool registerUser(std::string name,
                      std::string email,
                      std::string password,
                      std::string country,
                      std::string role);

    User* loginUser(std::string email, std::string password);

    // Transactions
    void insertTransaction(int userId,
                           double amount,
                           std::string type,
                           std::string country,
                           std::string deviceId,
                           std::string ip,
                           int riskScore,
                           std::string riskLevel,
                           bool isFraud);

    int getUserTransactionCount(int userId);
    double getUserAverageAmount(int userId);
    std::vector<User> getAllUsers();
    User* getUserById(int id);
    bool isNewDevice(int userId, std::string deviceId);
    bool registerUser(std::string name,
                  std::string email,
                  std::string password,
                  std::string country);
    void reduceTrustScore(int userId, int amount);
    void increaseTrustScore(int userId, int amount);
    std::vector<Transaction> getLastTransactions(int userId);
};