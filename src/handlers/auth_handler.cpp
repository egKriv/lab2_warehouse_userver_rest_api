#include "auth_handler.hpp"
#include "services/user_service.hpp"
#include <userver/formats/json.hpp>
#include <userver/formats/json/serialize.hpp>

namespace inventory {

static UserService user_service;

AuthHandler::AuthHandler(const userver::components::ComponentConfig& config,
                         const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context) {}

std::string AuthHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext& context) const {
    
    if (request.GetMethod() != userver::server::http::HttpMethod::kPost) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kMethodNotAllowed);
        userver::formats::json::ValueBuilder builder;
        builder["error"] = "Method not allowed";
        return userver::formats::json::ToString(builder.ExtractValue());
    }
    
    try {
        auto body = userver::formats::json::FromString(request.RequestBody());
        if (!body.HasMember("login") || !body.HasMember("password")) {
            request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
            userver::formats::json::ValueBuilder builder;
            builder["error"] = "Missing required fields: login, password";
            return userver::formats::json::ToString(builder.ExtractValue());
        }
        
        std::string login = body["login"].As<std::string>();
        std::string password = body["password"].As<std::string>();
        
        auto user = user_service.Login(login, password);
        if (user) {
            std::string token = user_service.GenerateToken(login);
            userver::formats::json::ValueBuilder builder;
            builder["access_token"] = token;
            builder["token_type"] = "Bearer";
            builder["expires_in"] = 3600;
            builder["user"]["id"] = user->id;
            builder["user"]["login"] = user->login;
            builder["user"]["role"] = user->role;
            
            request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
            return userver::formats::json::ToString(builder.ExtractValue());
        }
        
        request.SetResponseStatus(userver::server::http::HttpStatus::kUnauthorized);
        userver::formats::json::ValueBuilder builder;
        builder["error"] = "Invalid credentials";
        return userver::formats::json::ToString(builder.ExtractValue());
        
    } catch (const std::exception& e) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
        userver::formats::json::ValueBuilder builder;
        builder["error"] = "Invalid JSON format";
        return userver::formats::json::ToString(builder.ExtractValue());
    }
}

// Регистрация
RegisterHandler::RegisterHandler(const userver::components::ComponentConfig& config,
                                 const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context) {}

std::string RegisterHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext& context) const {
    
    if (request.GetMethod() != userver::server::http::HttpMethod::kPost) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kMethodNotAllowed);
        userver::formats::json::ValueBuilder builder;
        builder["error"] = "Method not allowed";
        return userver::formats::json::ToString(builder.ExtractValue());
    }
    
    try {
        auto body = userver::formats::json::FromString(request.RequestBody());
        if (!body.HasMember("login") || !body.HasMember("password") ||
            !body.HasMember("first_name") || !body.HasMember("last_name") ||
            !body.HasMember("email")) {
            request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
            userver::formats::json::ValueBuilder builder;
            builder["error"] = "Missing required fields: login, password, first_name, last_name, email";
            return userver::formats::json::ToString(builder.ExtractValue());
        }
        
        std::string login = body["login"].As<std::string>();
        std::string password = body["password"].As<std::string>();
        std::string first_name = body["first_name"].As<std::string>();
        std::string last_name = body["last_name"].As<std::string>();
        std::string email = body["email"].As<std::string>();
        std::string role = body.HasMember("role") ? body["role"].As<std::string>() : "storekeeper";
        
        auto user = user_service.Register(login, password, first_name, last_name, email, role);
        
        userver::formats::json::ValueBuilder builder;
        builder["id"] = user.id;
        builder["login"] = user.login;
        builder["first_name"] = user.first_name;
        builder["last_name"] = user.last_name;
        builder["email"] = user.email;
        builder["role"] = user.role;
        builder["message"] = "User registered successfully";
        
        request.SetResponseStatus(userver::server::http::HttpStatus::kCreated);
        return userver::formats::json::ToString(builder.ExtractValue());
        
    } catch (const std::exception& e) {
        std::string error_msg = e.what();
        if (error_msg.find("User already exists") != std::string::npos) {
            request.SetResponseStatus(userver::server::http::HttpStatus::kConflict);
        } else {
            request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
        }
        userver::formats::json::ValueBuilder builder;
        builder["error"] = error_msg;
        return userver::formats::json::ToString(builder.ExtractValue());
    }
}

} // namespace inventory