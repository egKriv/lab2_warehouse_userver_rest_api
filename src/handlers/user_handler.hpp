#pragma once

#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/components/component.hpp>

namespace inventory {

class UserSearchHandler final : public userver::server::handlers::HttpHandlerBase {
public:
    UserSearchHandler(const userver::components::ComponentConfig& config,
                      const userver::components::ComponentContext& context);
    std::string HandleRequestThrow(
        const userver::server::http::HttpRequest& request,
        userver::server::request::RequestContext& context) const override;
};

class UserByLoginHandler final : public userver::server::handlers::HttpHandlerBase {
public:
    UserByLoginHandler(const userver::components::ComponentConfig& config,
                       const userver::components::ComponentContext& context);
    std::string HandleRequestThrow(
        const userver::server::http::HttpRequest& request,
        userver::server::request::RequestContext& context) const override;
};

} // namespace inventory