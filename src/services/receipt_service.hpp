#pragma once

#include "storage.hpp"
#include <string>
#include <vector>
#include <optional>

namespace inventory {

class ReceiptService {
public:
    ReceiptService() = default;
    
    Receipt CreateReceipt(const std::string& product_id, int quantity,
                          const std::string& supplier, const std::string& created_by);
    std::vector<Receipt> GetReceiptHistory(const std::string& product_id = "");
    std::optional<Receipt> GetReceiptById(const std::string& id);
    
private:
    std::string GenerateId();
    std::string GetCurrentTimeISO();
};

} // namespace inventory