#include "receipt_service.hpp"
#include "product_service.hpp"
#include <chrono>
#include <sstream>
#include <iomanip>
#include <random>

namespace inventory {

std::string ReceiptService::GenerateId() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(10000, 99999);
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
    return "rcpt_" + std::to_string(timestamp) + "_" + std::to_string(dis(gen));
}

std::string ReceiptService::GetCurrentTimeISO() {
    auto now = std::chrono::system_clock::now();
    std::time_t time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::gmtime(&time_t), "%Y-%m-%dT%H:%M:%SZ");
    return ss.str();
}

Receipt ReceiptService::CreateReceipt(const std::string& product_id, int quantity,
                                      const std::string& supplier, const std::string& created_by) {
    if (quantity <= 0) throw std::runtime_error("Quantity must be positive");
    ProductService product_service;
    if (!product_service.ProductExists(product_id)) throw std::runtime_error("Product not found");

    Receipt receipt;
    receipt.id = GenerateId();
    receipt.product_id = product_id;
    receipt.quantity = quantity;
    receipt.supplier = supplier;
    receipt.created_by = created_by;
    receipt.receipt_date = GetCurrentTimeISO();

    // Обновляем остаток товара
    product_service.UpdateQuantity(product_id, quantity);

    receipts[receipt.id] = receipt;
    return receipt;
}

std::vector<Receipt> ReceiptService::GetReceiptHistory(const std::string& product_id) {
    std::vector<Receipt> result;
    for (const auto& [id, r] : receipts) {
        if (product_id.empty() || r.product_id == product_id) {
            result.push_back(r);
        }
    }
    return result;
}

std::optional<Receipt> ReceiptService::GetReceiptById(const std::string& id) {
    auto it = receipts.find(id);
    if (it != receipts.end()) return it->second;
    return std::nullopt;
}

} // namespace inventory