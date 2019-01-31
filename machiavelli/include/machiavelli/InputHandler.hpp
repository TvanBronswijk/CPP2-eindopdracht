#pragma once
#include <server/ServerCallbackHandler.hpp>
#include "Context.hpp"

class InputHandler : public server::ServerCallbackHandler {
public:
    explicit InputHandler(Context& ctx) : _ctx{ &ctx } {}
    bool on_command(server::input::Command) const override;
    std::shared_ptr<server::ClientInfo> on_client_register(server::connection::Socket) const override;
    Event on_client_input(std::weak_ptr<server::ClientInfo>, std::string) const override;

private:
    Context* _ctx;
};
