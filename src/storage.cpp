#include "storage.hpp"

namespace inventory {

std::unordered_map<std::string, UserData> users;
std::unordered_map<std::string, std::string> tokens;
std::unordered_map<std::string, Product> products;
std::unordered_map<std::string, Receipt> receipts;
int next_product_id = 1;
int next_receipt_id = 1;

} // namespace inventory