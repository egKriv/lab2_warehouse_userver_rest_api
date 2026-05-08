#include "product_handler.hpp"
#include "utils/auth_utils.hpp"
#include "services/product_service.hpp"
#include <userver/formats/json.hpp>

namespace inventory {

static ProductService product_service;

ProductHandler::ProductHandler(const userver::components::ComponentConfig& config,
                               const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context) {}

std::string ProductHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext& context) const {
    
    auto method = request.GetMethod();
    
    if (method == userver::server::http::HttpMethod::kGet) {
        // Получение остатков всех товаров
        auto stock = product_service.GetAllStock();
        userver::formats::json::ValueBuilder builder;
        builder = userver::formats::json::Type::kArray;
        for (const auto& prod : stock) {
            userver::formats::json::ValueBuilder item;
            item["id"] = prod.id;
            item["name"] = prod.name;
            item["description"] = prod.description;
            item["sku"] = prod.sku;
            item["quantity_on_hand"] = prod.quantity_on_hand;
            item["unit"] = prod.unit;
            builder.PushBack(item.ExtractValue());
        }
        request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
        return userver::formats::json::ToString(builder.ExtractValue());
    }
    else if (method == userver::server::http::HttpMethod::kPost) {
        // Добавление товара (требуется аутентификация, роль manager/admin)
        std::string token = ExtractTokenFromHeader(request);
        std::string login;
        if (!ValidateToken(token, login)) {
            request.SetResponseStatus(userver::server::http::HttpStatus::kUnauthorized);
            userver::formats::json::ValueBuilder builder;
            builder["error"] = "Authentication required";
            return userver::formats::json::ToString(builder.ExtractValue());
        }
        // Проверка роли может быть добавлена при необходимости
        try {
            auto body = userver::formats::json::FromString(request.RequestBody());
            if (!body.HasMember("name") || !body.HasMember("initial_quantity")) {
                request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
                userver::formats::json::ValueBuilder builder;
                builder["error"] = "Missing required fields: name, initial_quantity";
                return userver::formats::json::ToString(builder.ExtractValue());
            }
            
            std::string name = body["name"].As<std::string>();
            std::string description = body.HasMember("description") ? body["description"].As<std::string>() : "";
            std::string sku = body.HasMember("sku") ? body["sku"].As<std::string>() : "";
            int qty = body["initial_quantity"].As<int>();
            std::string unit = body.HasMember("unit") ? body["unit"].As<std::string>() : "pcs";
            
            auto prod = product_service.AddProduct(name, description, sku, qty, unit);
            
            userver::formats::json::ValueBuilder builder;
            builder["id"] = prod.id;
            builder["name"] = prod.name;
            builder["description"] = prod.description;
            builder["sku"] = prod.sku;
            builder["quantity_on_hand"] = prod.quantity_on_hand;
            builder["unit"] = prod.unit;
            builder["message"] = "Product added successfully";
            
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

// Поиск товара по названию
ProductSearchHandler::ProductSearchHandler(const userver::components::ComponentConfig& config,
                                           const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context) {}

std::string ProductSearchHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext& context) const {
    
    if (request.GetMethod() != userver::server::http::HttpMethod::kGet) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kMethodNotAllowed);
        userver::formats::json::ValueBuilder builder;
        builder["error"] = "Method not allowed";
        return userver::formats::json::ToString(builder.ExtractValue());
    }
    
    std::string name = request.GetArg("name");
    if (name.empty()) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
        userver::formats::json::ValueBuilder builder;
        builder["error"] = "Query parameter 'name' is required";
        return userver::formats::json::ToString(builder.ExtractValue());
    }
    
    auto products = product_service.SearchByName(name);
    userver::formats::json::ValueBuilder builder;
    builder = userver::formats::json::Type::kArray;
    for (const auto& prod : products) {
        userver::formats::json::ValueBuilder item;
        item["id"] = prod.id;
        item["name"] = prod.name;
        item["description"] = prod.description;
        item["sku"] = prod.sku;
        item["quantity_on_hand"] = prod.quantity_on_hand;
        item["unit"] = prod.unit;
        builder.PushBack(item.ExtractValue());
    }
    request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
    return userver::formats::json::ToString(builder.ExtractValue());
}

// Получение всех товаров с остатками (аналог GET /products)
ProductStockHandler::ProductStockHandler(const userver::components::ComponentConfig& config,
                                         const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context) {}

std::string ProductStockHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext& context) const {
    
    if (request.GetMethod() != userver::server::http::HttpMethod::kGet) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kMethodNotAllowed);
        userver::formats::json::ValueBuilder builder;
        builder["error"] = "Method not allowed";
        return userver::formats::json::ToString(builder.ExtractValue());
    }
    
    auto stock = product_service.GetAllStock();
    userver::formats::json::ValueBuilder builder;
    builder = userver::formats::json::Type::kArray;
    for (const auto& prod : stock) {
        userver::formats::json::ValueBuilder item;
        item["id"] = prod.id;
        item["name"] = prod.name;
        item["quantity_on_hand"] = prod.quantity_on_hand;
        item["unit"] = prod.unit;
        builder.PushBack(item.ExtractValue());
    }
    request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
    return userver::formats::json::ToString(builder.ExtractValue());
}

} // namespace inventory