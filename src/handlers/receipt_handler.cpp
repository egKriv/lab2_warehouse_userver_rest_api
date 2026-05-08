#include "receipt_handler.hpp"
#include "utils/auth_utils.hpp"
#include "services/receipt_service.hpp"
#include "services/product_service.hpp"
#include <userver/formats/json.hpp>

namespace inventory {

static ReceiptService receipt_service;
static ProductService product_service;

ReceiptHandler::ReceiptHandler(const userver::components::ComponentConfig& config,
                               const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context) {}

std::string ReceiptHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext& context) const {
    
    auto method = request.GetMethod();
    
    // Все операции с поступлениями требуют аутентификации
    std::string token = ExtractTokenFromHeader(request);
    std::string login;
    if (!ValidateToken(token, login)) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kUnauthorized);
        userver::formats::json::ValueBuilder builder;
        builder["error"] = "Authentication required";
        return userver::formats::json::ToString(builder.ExtractValue());
    }
    
    if (method == userver::server::http::HttpMethod::kGet) {
        // История поступлений (можно фильтровать по product_id)
        std::string product_id = request.GetArg("product_id");
        auto history = receipt_service.GetReceiptHistory(product_id);
        
        userver::formats::json::ValueBuilder builder;
        builder = userver::formats::json::Type::kArray;
        for (const auto& r : history) {
            userver::formats::json::ValueBuilder item;
            item["id"] = r.id;
            item["product_id"] = r.product_id;
            item["quantity"] = r.quantity;
            item["supplier"] = r.supplier;
            item["receipt_date"] = r.receipt_date;
            item["created_by"] = r.created_by;
            builder.PushBack(item.ExtractValue());
        }
        request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
        return userver::formats::json::ToString(builder.ExtractValue());
    }
    else if (method == userver::server::http::HttpMethod::kPost) {
        // Создание поступления
        try {
            auto body = userver::formats::json::FromString(request.RequestBody());
            if (!body.HasMember("product_id") || !body.HasMember("quantity") || !body.HasMember("supplier")) {
                request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
                userver::formats::json::ValueBuilder builder;
                builder["error"] = "Missing required fields: product_id, quantity, supplier";
                return userver::formats::json::ToString(builder.ExtractValue());
            }
            
            std::string product_id = body["product_id"].As<std::string>();
            int quantity = body["quantity"].As<int>();
            std::string supplier = body["supplier"].As<std::string>();
            
            auto receipt = receipt_service.CreateReceipt(product_id, quantity, supplier, login);
            
            userver::formats::json::ValueBuilder builder;
            builder["id"] = receipt.id;
            builder["product_id"] = receipt.product_id;
            builder["quantity"] = receipt.quantity;
            builder["supplier"] = receipt.supplier;
            builder["receipt_date"] = receipt.receipt_date;
            builder["created_by"] = receipt.created_by;
            builder["message"] = "Receipt created successfully";
            
            request.SetResponseStatus(userver::server::http::HttpStatus::kCreated);
            return userver::formats::json::ToString(builder.ExtractValue());
            
        } catch (const std::exception& e) {
            request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
            userver::formats::json::ValueBuilder builder;
            builder["error"] = e.what();
            return userver::formats::json::ToString(builder.ExtractValue());
        }
    }
    
    request.SetResponseStatus(userver::server::http::HttpStatus::kMethodNotAllowed);
    userver::formats::json::ValueBuilder builder;
    builder["error"] = "Method not allowed";
    return userver::formats::json::ToString(builder.ExtractValue());
}

// Списание товара (write-off)
WriteOffHandler::WriteOffHandler(const userver::components::ComponentConfig& config,
                                 const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context) {}

std::string WriteOffHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext& context) const {
    
    if (request.GetMethod() != userver::server::http::HttpMethod::kPost) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kMethodNotAllowed);
        userver::formats::json::ValueBuilder builder;
        builder["error"] = "Method not allowed";
        return userver::formats::json::ToString(builder.ExtractValue());
    }
    
    std::string token = ExtractTokenFromHeader(request);
    std::string login;
    if (!ValidateToken(token, login)) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kUnauthorized);
        userver::formats::json::ValueBuilder builder;
        builder["error"] = "Authentication required";
        return userver::formats::json::ToString(builder.ExtractValue());
    }
    
    try {
        auto body = userver::formats::json::FromString(request.RequestBody());
        if (!body.HasMember("product_id") || !body.HasMember("quantity")) {
            request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
            userver::formats::json::ValueBuilder builder;
            builder["error"] = "Missing required fields: product_id, quantity";
            return userver::formats::json::ToString(builder.ExtractValue());
        }
        
        std::string product_id = body["product_id"].As<std::string>();
        int quantity = body["quantity"].As<int>();
        if (quantity <= 0) {
            request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
            userver::formats::json::ValueBuilder builder;
            builder["error"] = "Quantity must be positive";
            return userver::formats::json::ToString(builder.ExtractValue());
        }
        
        // списание — это отрицательное изменение остатка
        bool success = product_service.UpdateQuantity(product_id, -quantity);
        if (!success) {
            request.SetResponseStatus(userver::server::http::HttpStatus::kConflict);
            userver::formats::json::ValueBuilder builder;
            builder["error"] = "Write-off failed. Product not found or insufficient stock.";
            return userver::formats::json::ToString(builder.ExtractValue());
        }
        
        auto updated = product_service.GetProductById(product_id);
        userver::formats::json::ValueBuilder builder;
        builder["message"] = "Write-off completed";
        builder["product_id"] = product_id;
        builder["remaining_quantity"] = updated->quantity_on_hand;
        request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
        return userver::formats::json::ToString(builder.ExtractValue());
        
    } catch (const std::exception& e) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
        userver::formats::json::ValueBuilder builder;
        builder["error"] = e.what();
        return userver::formats::json::ToString(builder.ExtractValue());
    }
}

} // namespace inventory