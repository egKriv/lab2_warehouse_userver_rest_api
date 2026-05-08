#include <userver/server/handlers/ping.hpp>
#include <userver/utils/daemon_run.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/components/component.hpp>

#include "handlers/auth_handler.hpp"
#include "handlers/user_handler.hpp"
#include "handlers/product_handler.hpp"
#include "handlers/receipt_handler.hpp"

int main(int argc, char* argv[]) {
    auto component_list = userver::components::MinimalServerComponentList();
    
    component_list.Append<userver::server::handlers::Ping>();
    component_list.Append<inventory::AuthHandler>("auth-handler");
    component_list.Append<inventory::RegisterHandler>("register-handler");
    component_list.Append<inventory::UserSearchHandler>("user-search-handler");
    component_list.Append<inventory::UserByLoginHandler>("user-by-login-handler");
    component_list.Append<inventory::ProductHandler>("product-handler");
    component_list.Append<inventory::ProductSearchHandler>("product-search-handler");
    component_list.Append<inventory::ProductStockHandler>("product-stock-handler");
    component_list.Append<inventory::ReceiptHandler>("receipt-handler");
    component_list.Append<inventory::WriteOffHandler>("write-off-handler");
    
    return userver::utils::DaemonMain(argc, argv, component_list);
}