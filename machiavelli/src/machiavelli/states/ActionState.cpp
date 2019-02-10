#include "machiavelli/states/ActionState.hpp"

using namespace server;
using namespace server::command;
using namespace server::command::validate;
using namespace server::connection;
using namespace server::player;

ActionState::ActionState(Context &ctx) : BaseState(ctx, {
        {
                "info",
                "Get your current character info.",
                [](StringArgs args) { return validate_that<StringArgs>(args, is_empty<std::string>); },
                [&](StringArgs args, Player& player, Socket& socket, Context& context) {
                    //TODO
                }
        },
        {
                "gold",
                "Take two gold.",
                [](StringArgs args) { return validate_that<StringArgs>(args, is_empty<std::string>); },
                [&](StringArgs args, Player& player, Socket& socket, Context& context) {
                    if(!_take_gold_or_card) {
                        player.get_data<GameData>().gold_coins += 2;
                        _take_gold_or_card = true;
                    } else {
                        socket << "You already took a card or gold this turn.";
                    }
                }
        },
        {
                "card",
                "Take a card from the deck.",
                [](StringArgs args) { return validate_that<StringArgs>(args, is_empty<std::string>); },
                [&](StringArgs args, Player& player, Socket& socket, Context& context) {
                    if(!_take_gold_or_card) {
                        BuildingCard card = context.game().deck.take_random();
                        player.get_data<GameData>().building_cards.add(card);
                        _take_gold_or_card = true;
                    } else {
                        socket << "You already took a card or gold this turn.";
                    }
                }
        },
        {
                "character",
                "Use your character's action.",
                [](StringArgs args) { return validate_that<StringArgs>(args, is_empty<std::string>); },
                [&](StringArgs args, Player& player, Socket& socket, Context& context) {
                    //TODO BIG
                }
        },
        {
                "build",
                "Build a card from your hand.",
                [](StringArgs args) { return validate_that<StringArgs>(args, is_empty<std::string>); },
                [&](StringArgs args, Player& player, Socket& socket, Context& context) {
                    //TODO
                }
        },
        {
                "finish",
                "End your turn.",
                [](StringArgs args) { return validate_that<StringArgs>(args, is_empty<std::string>); },
                [&](StringArgs args, Player& player, Socket& socket, Context& context) {
                    context.game().next_turn();
                }
        },
}) {}