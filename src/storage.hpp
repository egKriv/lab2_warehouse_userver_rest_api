#pragma once

#include <unordered_map>
#include <string>

namespace inventory {

struct UserData {
    std::string id;
    std::string login;
    std::string password_hash;
    std::string first_name;
    std::string last_name;
    std::string email;
    std::string role; // storekeeper, manager, admin
    std::string created_at;
};

struct Product {
    std::string id;
    std::string name;
    std::string description;
    std::string sku;
    int quantity_on_hand = 0;
    std::string unit;
    std::string created_at;
    std::string updated_at;
};

struct Receipt {
    std::string id;
    std::string product_id;
    int quantity = 0;
    std::string supplier;
    std::string receipt_date;
    std::string created_by;
};

// Глобальные хранилища (in-memory)
extern std::unordered_map<std::string, UserData> users;              // key: login
extern std::unordered_map<std::string, std::string> tokens;         // key: login, value: token
extern std::unordered_map<std::string, Product> products;           // key: product_id
extern std::unordered_map<std::string, Receipt> receipts;           // key: receipt_id
extern int next_product_id;
extern int next_receipt_id;

} // namespace inventory