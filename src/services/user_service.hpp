#pragma once

#include "storage.hpp"
#include <string>
#include <vector>
#include <optional>

namespace inventory {

class UserService {
public:
    UserService() = default;
    
    // Регистрация и аутентификация
    UserData Register(const std::string& login, const std::string& password,
                      const std::string& first_name, const std::string& last_name,
                      const std::string& email, const std::string& role = "storekeeper");
    std::optional<UserData> Login(const std::string& login, const std::string& password);
    std::string GenerateToken(const std::string& login);
    bool ValidateToken(const std::string& token, std::string& login);
    
    // Поиск пользователей
    std::optional<UserData> GetUserByLogin(const std::string& login);
    std::vector<UserData> SearchByNameMask(const std::string& name_mask);
    
private:
    std::string GenerateId();
    std::string GetCurrentTimeISO();
    bool IsValidEmail(const std::string& email);
};

} // namespace inventory