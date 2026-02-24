#pragma once
#include <string>

struct RiskResult {
    int score;
    std::string level;
    bool fraud;
};

class FraudEngine {
public:
    RiskResult evaluate(
        double amount,
        std::string userCountry,
        std::string txnCountry,
        bool newDevice,
        int txnCount,
        double avgAmount
    );
};