#pragma once

#include "storage.hpp"
#include <string>
#include <vector>
#include <optional>

namespace inventory {

class ProductService {
public:
    ProductService() = default;
    
    Product AddProduct(const std::string& name, const std::string& description,
                       const std::string& sku, int initial_quantity,
                       const std::string& unit = "pcs");
    std::optional<Product> GetProductById(const std::string& id);
    std::vector<Product> SearchByName(const std::string& name_pattern);
    std::vector<Product> GetAllStock(); // все товары с остатками
    bool UpdateQuantity(const std::string& id, int delta); // delta может быть отрицательным для списания
    bool ProductExists(const std::string& id);
    
private:
    std::string GenerateId();
    std::string GetCurrentTimeISO();
};

} // namespace inventory