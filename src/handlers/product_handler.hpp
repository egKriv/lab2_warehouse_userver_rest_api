#pragma once

#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/components/component.hpp>

namespace inventory {

class ProductHandler final : public userver::server::handlers::HttpHandlerBase {
public:
    ProductHandler(const userver::components::ComponentConfig& config,
                   const userver::components::ComponentContext& context);
    std::string HandleRequestThrow(
        const userver::server::http::HttpRequest& request,
        userver::server::request::RequestContext& context) const override;
};

class ProductSearchHandler final : public userver::server::handlers::HttpHandlerBase {
public:
    ProductSearchHandler(const userver::components::ComponentConfig& config,
                         const userver::components::ComponentContext& context);
    std::string HandleRequestThrow(
        const userver::server::http::HttpRequest& request,
        userver::server::request::RequestContext& context) const override;
};

class ProductStockHandler final : public userver::server::handlers::HttpHandlerBase {
public:
    ProductStockHandler(const userver::components::ComponentConfig& config,
                        const userver::components::ComponentContext& context);
    std::string HandleRequestThrow(
        const userver::server::http::HttpRequest& request,
        userver::server::request::RequestContext& context) const override;
};

} // namespace inventory