#include "fraud_engine.h"
#include <ctime>

RiskResult FraudEngine::evaluate(
    double amount,
    std::string userCountry,
    std::string txnCountry,
    bool newDevice,
    int txnCount,
    double avgAmount
) {
    int score = 0;

    if (amount > 100000) score += 60;
    else if (amount > 50000) score += 40;
    else if (amount > 20000) score += 25;
    else if (amount > 10000) score += 15;

    if (userCountry != txnCountry)
        score += 35;

    if (newDevice)
        score += 25;

    if (txnCount >= 5)
        score += 30;
    else if (txnCount >= 3)
        score += 20;

    if (avgAmount > 0) {
        double ratio = amount / avgAmount;
        if (ratio > 5) score += 35;
        else if (ratio > 3) score += 25;
        else if (ratio > 2) score += 15;
    }

    time_t now = time(0);
    tm* ltm = localtime(&now);
    if (ltm->tm_hour >= 0 && ltm->tm_hour <= 4)
        score += 15;

    if (score > 100) score = 100;

    RiskResult r;
    r.score = score;

    if (score >= 80) {
        r.level = "FRAUD";
        r.fraud = true;
    } else if (score >= 60) {
        r.level = "HIGH";
        r.fraud = false;
    } else if (score >= 35) {
        r.level = "MEDIUM";
        r.fraud = false;
    } else {
        r.level = "LOW";
        r.fraud = false;
    }

    return r;
}