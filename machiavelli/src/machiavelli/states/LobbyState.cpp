#include "machiavelli/states/LobbyState.hpp"

using namespace server;
using namespace server::command;
using namespace server::command::validate;
using namespace server::connection;
using namespace server::player;

LobbyState::LobbyState(Context &ctx) : BaseState(ctx, {
        {
                "start-game",
                "Start Machiavelli.",
                [](StringArgs args) { return validate_that<StringArgs>(args, is_empty<std::string>); },
                [&](StringArgs args, Player& player, Socket& socket, Context& context) {
                    try {
                        if(context.server().registry().size() == 2) {
                            context.new_game().start(false);
                        } else {
                            socket << "Not enough players to start the game.\r\n";
                        }
                    }catch(...) {
                        socket << "cannot start game. Did something go wrong?\r\n";
                    }
                }
        }
}) {}