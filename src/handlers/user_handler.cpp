#include "user_handler.hpp"
#include "utils/auth_utils.hpp"
#include "services/user_service.hpp"
#include <userver/formats/json.hpp>

namespace inventory {

static UserService user_service;

UserSearchHandler::UserSearchHandler(const userver::components::ComponentConfig& config,
                                     const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context) {}

std::string UserSearchHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext& context) const {
    
    if (request.GetMethod() != userver::server::http::HttpMethod::kGet) {
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
    
    std::string name_mask = request.GetArg("name_mask");
    if (name_mask.empty()) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
        userver::formats::json::ValueBuilder builder;
        builder["error"] = "Query parameter 'name_mask' is required";
        return userver::formats::json::ToString(builder.ExtractValue());
    }
    
    auto users = user_service.SearchByNameMask(name_mask);
    userver::formats::json::ValueBuilder builder;
    builder = userver::formats::json::Type::kArray;
    for (const auto& user : users) {
        userver::formats::json::ValueBuilder item;
        item["id"] = user.id;
        item["login"] = user.login;
        item["first_name"] = user.first_name;
        item["last_name"] = user.last_name;
        item["email"] = user.email;
        item["role"] = user.role;
        builder.PushBack(item.ExtractValue());
    }
    
    request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
    return userver::formats::json::ToString(builder.ExtractValue());
}

// Поиск по логину (требует параметр пути: login)
UserByLoginHandler::UserByLoginHandler(const userver::components::ComponentConfig& config,
                                       const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context) {}

std::string UserByLoginHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext& context) const {
    
    if (request.GetMethod() != userver::server::http::HttpMethod::kGet) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kMethodNotAllowed);
        userver::formats::json::ValueBuilder builder;
        builder["error"] = "Method not allowed";
        return userver::formats::json::ToString(builder.ExtractValue());
    }
    
    std::string token = ExtractTokenFromHeader(request);
    std::string current_login;
    if (!ValidateToken(token, current_login)) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kUnauthorized);
        userver::formats::json::ValueBuilder builder;
        builder["error"] = "Authentication required";
        return userver::formats::json::ToString(builder.ExtractValue());
    }
    
    std::string target_login = request.GetPathArg("login");
    auto user = user_service.GetUserByLogin(target_login);
    if (!user) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);
        userver::formats::json::ValueBuilder builder;
        builder["error"] = "User not found";
        return userver::formats::json::ToString(builder.ExtractValue());
    }
    
    userver::formats::json::ValueBuilder builder;
    builder["id"] = user->id;
    builder["login"] = user->login;
    builder["first_name"] = user->first_name;
    builder["last_name"] = user->last_name;
    builder["email"] = user->email;
    builder["role"] = user->role;
    
    request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
    return userver::formats::json::ToString(builder.ExtractValue());
}

} // namespace inventory