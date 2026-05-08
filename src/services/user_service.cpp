#include "user_service.hpp"
#include <regex>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <random>
#include <algorithm>

namespace inventory {

std::string UserService::GenerateId() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(100000, 999999);
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
    return "user_" + std::to_string(timestamp) + "_" + std::to_string(dis(gen));
}

std::string UserService::GetCurrentTimeISO() {
    auto now = std::chrono::system_clock::now();
    std::time_t time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::gmtime(&time_t), "%Y-%m-%dT%H:%M:%SZ");
    return ss.str();
}

bool UserService::IsValidEmail(const std::string& email) {
    const std::regex pattern(R"((\w+)(\.\w+)*@(\w+)(\.\w{2,})+)");
    return std::regex_match(email, pattern);
}

UserData UserService::Register(const std::string& login, const std::string& password,
                               const std::string& first_name, const std::string& last_name,
                               const std::string& email, const std::string& role) {
    if (login.length() < 3 || login.length() > 50)
        throw std::runtime_error("Login must be between 3 and 50 characters");
    if (password.length() < 6)
        throw std::runtime_error("Password must be at least 6 characters");
    if (!IsValidEmail(email))
        throw std::runtime_error("Invalid email format");
    if (users.find(login) != users.end())
        throw std::runtime_error("User already exists");

    UserData user;
    user.id = GenerateId();
    user.login = login;
    user.password_hash = password;
    user.first_name = first_name;
    user.last_name = last_name;
    user.email = email;
    user.role = role;
    user.created_at = GetCurrentTimeISO();

    users[login] = user;
    return user;
}

std::optional<UserData> UserService::Login(const std::string& login, const std::string& password) {
    auto it = users.find(login);
    if (it != users.end() && it->second.password_hash == password) {
        return it->second;
    }
    return std::nullopt;
}

std::string UserService::GenerateToken(const std::string& login) {
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
    std::string token = "jwt_" + login + "_" + std::to_string(timestamp);
    tokens[login] = token;
    return token;
}

bool UserService::ValidateToken(const std::string& token, std::string& login) {
    if (token.find("jwt_") != 0) return false;
    std::string content = token.substr(4);
    auto pos = content.rfind('_');
    if (pos == std::string::npos) return false;
    login = content.substr(0, pos);
    auto it = tokens.find(login);
    return it != tokens.end() && it->second == token;
}

std::optional<UserData> UserService::GetUserByLogin(const std::string& login) {
    auto it = users.find(login);
    if (it != users.end()) return it->second;
    return std::nullopt;
}

std::vector<UserData> UserService::SearchByNameMask(const std::string& name_mask) {
    std::vector<UserData> result;
    std::string lower_mask = name_mask;
    std::transform(lower_mask.begin(), lower_mask.end(), lower_mask.begin(), ::tolower);
    for (const auto& [login, user] : users) {
        std::string full_name = user.first_name + " " + user.last_name;
        std::string full_lower = full_name;
        std::transform(full_lower.begin(), full_lower.end(), full_lower.begin(), ::tolower);
        // Простая проверка на подстроку (можно улучшить до regex)
        if (full_lower.find(lower_mask) != std::string::npos) {
            result.push_back(user);
        }
    }
    return result;
}

} // namespace inventory