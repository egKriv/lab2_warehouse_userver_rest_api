#pragma once

#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/components/component.hpp>

namespace inventory {

class ReceiptHandler final : public userver::server::handlers::HttpHandlerBase {
public:
    ReceiptHandler(const userver::components::ComponentConfig& config,
                   const userver::components::ComponentContext& context);
    std::string HandleRequestThrow(
        const userver::server::http::HttpRequest& request,
        userver::server::request::RequestContext& context) const override;
};

class WriteOffHandler final : public userver::server::handlers::HttpHandlerBase {
public:
    WriteOffHandler(const userver::components::ComponentConfig& config,
                    const userver::components::ComponentContext& context);
    std::string HandleRequestThrow(
        const userver::server::http::HttpRequest& request,
        userver::server::request::RequestContext& context) const override;
};

} // namespace inventory