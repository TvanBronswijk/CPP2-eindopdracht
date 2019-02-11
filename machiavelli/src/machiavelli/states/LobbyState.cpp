#include "machiavelli/states/LobbyState.hpp"
#include "machiavelli/states/IdleState.hpp"

using namespace server;
using namespace server::command;
using namespace server::command::validate;
using namespace server::connection;
using namespace server::player;

LobbyState::LobbyState(Context &ctx) : BaseState(ctx, {
        {
                "start",
                "Start Machiavelli.",
                [](StringArgs args) { return validate_that<StringArgs>(args, is_empty<std::string>); },
                [&](StringArgs args, Player& player, Socket& socket, Context& context) {
                    try {
                        if(context.server().registry().size() == 2) {
                            context.new_game();
                            context.game().player_one().lock()->get_player().get_states().put(std::make_unique<IdleState>(*_ctx));
                            context.game().player_two().lock()->get_player().get_states().put(std::make_unique<IdleState>(*_ctx));
                            context.game().start(false);
                        } else {
                            socket << "Not enough players to start the game.\r\n";
                        }
                    }catch(...) {
                        socket << "cannot start game. Did something go wrong?\r\n";
                    }
                }
        },
        {
                "start-random",
                "Start Machiavelli with a random setup round.",
                [](StringArgs args) { return validate_that<StringArgs>(args, is_empty<std::string>); },
                [&](StringArgs args, Player& player, Socket& socket, Context& context) {
                    try {
                        if(context.server().registry().size() == 2) {
                            context.new_game();
                            context.game().player_one().lock()->get_player().get_states().put(std::make_unique<IdleState>(*_ctx));
                            context.game().player_two().lock()->get_player().get_states().put(std::make_unique<IdleState>(*_ctx));
                            context.game().start(true);
                        } else {
                            socket << "Not enough players to start the game.\r\n";
                        }
                    }catch(...) {
                        socket << "cannot start game. Did something go wrong?\r\n";
                    }
                }
        }
}) {}