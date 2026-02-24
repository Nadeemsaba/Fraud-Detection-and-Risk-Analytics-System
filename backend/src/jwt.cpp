#include "jwt.h"
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <ctime>
#include <sstream>
#include <vector>
#include <iomanip>
#include <openssl/buffer.h>
#include <algorithm>

static const std::string SECRET = "SUPER_SECRET_KEY_2025";

// Base64 URL encode
std::string base64UrlEncode(const std::string &input) {
    std::string base64;
    BIO *bio, *b64;
    BUF_MEM *bufferPtr;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    BIO_write(bio, input.c_str(), input.length());
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);

    base64.assign(bufferPtr->data, bufferPtr->length);
    BIO_free_all(bio);

    for (auto &c : base64) {
        if (c == '+') c = '-';
        if (c == '/') c = '_';
    }
    base64.erase(std::remove(base64.begin(), base64.end(), '='), base64.end());

    return base64;
}

std::string hmacSha256(const std::string &data) {
    unsigned char* digest;
    digest = HMAC(EVP_sha256(),
                  SECRET.c_str(), SECRET.length(),
                  (unsigned char*)data.c_str(), data.length(),
                  NULL, NULL);

    return std::string((char*)digest, 32);
}

std::string createJWT(int userId, std::string role) {

    std::string header = R"({"alg":"HS256","typ":"JWT"})";

    long exp = std::time(nullptr) + 3600; // 1 hour expiry

    std::stringstream payload;
    payload << "{\"userId\":" << userId
            << ",\"role\":\"" << role
            << "\",\"exp\":" << exp << "}";

    std::string encodedHeader = base64UrlEncode(header);
    std::string encodedPayload = base64UrlEncode(payload.str());

    std::string data = encodedHeader + "." + encodedPayload;

    std::string signature = base64UrlEncode(hmacSha256(data));

    return data + "." + signature;
}

bool verifyJWT(const std::string& token, int& userId, std::string& role) {

    size_t firstDot = token.find('.');
    size_t secondDot = token.find('.', firstDot + 1);

    if (firstDot == std::string::npos || secondDot == std::string::npos)
        return false;

    std::string encodedHeader = token.substr(0, firstDot);
    std::string encodedPayload = token.substr(firstDot + 1, secondDot - firstDot - 1);
    std::string signature = token.substr(secondDot + 1);

    std::string data = encodedHeader + "." + encodedPayload;
    std::string expectedSignature = base64UrlEncode(hmacSha256(data));

    if (expectedSignature != signature)
        return false;

    // --- BASE64 URL DECODE ---
    std::string payload = encodedPayload;
    std::replace(payload.begin(), payload.end(), '-', '+');
    std::replace(payload.begin(), payload.end(), '_', '/');
    while (payload.size() % 4) payload += '=';

    BIO* bio, *b64;
    int decodeLen = payload.size();
    std::vector<char> buffer(decodeLen);

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new_mem_buf(payload.c_str(), payload.length());
    bio = BIO_push(b64, bio);
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);

    int length = BIO_read(bio, buffer.data(), payload.length());
    BIO_free_all(bio);

    std::string decodedPayload(buffer.data(), length);

    // --- EXTRACT VALUES ---
    size_t idPos = decodedPayload.find("\"userId\":");
    size_t rolePos = decodedPayload.find("\"role\":\"");

    if (idPos == std::string::npos || rolePos == std::string::npos)
        return false;

    userId = std::stoi(decodedPayload.substr(idPos + 9));

    size_t roleStart = rolePos + 8;
    size_t roleEnd = decodedPayload.find("\"", roleStart);
    role = decodedPayload.substr(roleStart, roleEnd - roleStart);

    return true;
}