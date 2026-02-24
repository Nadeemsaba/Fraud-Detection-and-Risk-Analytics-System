#pragma once
#include <string>

std::string createJWT(int userId, std::string role);
bool verifyJWT(const std::string& token, int& userId, std::string& role);