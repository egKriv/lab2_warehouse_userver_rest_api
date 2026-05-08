#include "product_service.hpp"
#include <chrono>
#include <sstream>
#include <iomanip>
#include <random>
#include <algorithm>

namespace inventory {

std::string ProductService::GenerateId() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(10000, 99999);
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
    return "prod_" + std::to_string(timestamp) + "_" + std::to_string(dis(gen));
}

std::string ProductService::GetCurrentTimeISO() {
    auto now = std::chrono::system_clock::now();
    std::time_t time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::gmtime(&time_t), "%Y-%m-%dT%H:%M:%SZ");
    return ss.str();
}

Product ProductService::AddProduct(const std::string& name, const std::string& description,
                                   const std::string& sku, int initial_quantity,
                                   const std::string& unit) {
    if (name.empty()) throw std::runtime_error("Product name cannot be empty");
    if (initial_quantity < 0) throw std::runtime_error("Initial quantity cannot be negative");

    Product prod;
    prod.id = GenerateId();
    prod.name = name;
    prod.description = description;
    prod.sku = sku;
    prod.quantity_on_hand = initial_quantity;
    prod.unit = unit;
    prod.created_at = GetCurrentTimeISO();
    prod.updated_at = prod.created_at;

    products[prod.id] = prod;
    return prod;
}

std::optional<Product> ProductService::GetProductById(const std::string& id) {
    auto it = products.find(id);
    if (it != products.end()) return it->second;
    return std::nullopt;
}

std::vector<Product> ProductService::SearchByName(const std::string& name_pattern) {
    std::vector<Product> result;
    std::string lower_pattern = name_pattern;
    std::transform(lower_pattern.begin(), lower_pattern.end(), lower_pattern.begin(), ::tolower);
    for (const auto& [id, prod] : products) {
        std::string lower_name = prod.name;
        std::transform(lower_name.begin(), lower_name.end(), lower_name.begin(), ::tolower);
        if (lower_name.find(lower_pattern) != std::string::npos) {
            result.push_back(prod);
        }
    }
    return result;
}

std::vector<Product> ProductService::GetAllStock() {
    std::vector<Product> result;
    for (const auto& [id, prod] : products) {
        result.push_back(prod);
    }
    return result;
}

bool ProductService::UpdateQuantity(const std::string& id, int delta) {
    auto it = products.find(id);
    if (it == products.end()) return false;
    int new_qty = it->second.quantity_on_hand + delta;
    if (new_qty < 0) return false; // нельзя уйти в минус
    it->second.quantity_on_hand = new_qty;
    it->second.updated_at = GetCurrentTimeISO();
    return true;
}

bool ProductService::ProductExists(const std::string& id) {
    return products.find(id) != products.end();
}

} // namespace inventory