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
                    auto& data = player.get_data<GameData>();
                    socket << "Gold: " << data.gold_coins << "\r\n";
                    socket << "Character Cards:\r\n";
                    std::for_each(data.character_cards.begin(), data.character_cards.end(), [&](CharacterCard& cc){
                        socket << cc.description()  << "\r\n";
                    });
                    socket << "Hand:\r\n";
                    std::for_each(data.building_cards.begin(), data.building_cards.end(), [&](BuildingCard& bc){
                        socket << bc.description()  << "\r\n";
                    });
                    socket << "Built:\r\n";
                    std::for_each(data.built_buildings.begin(), data.built_buildings.end(), [&](BuildingCard& bc){
                        socket << bc.description()  << "\r\n";
                    });
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
                        socket << "You recieved two gold!\r\n";
                    } else {
                        socket << "You already took a card or gold this turn.\r\n";
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
                        socket << "You drew a " << card.name() << "!\r\n";
                    } else {
                        socket << "You already took a card or gold this turn.\r\n";
                    }
                }
        },
        {
                "character",
                "Use your character's action.",
                [](StringArgs args) { return validate_that<StringArgs>(args, is_empty<std::string>); },
                [&](StringArgs args, Player& player, Socket& socket, Context& context) {
                    if(!_character_action) {
                        auto action = context.game().get_action(context.game().get_current_turn());
                        action(player, socket, context);
                        _character_action = true;
                        socket << "your action has triggered...\r\n";
                    }else {
                        socket << "You already did your action this turn.\r\n";
                    }
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
                    socket << "You finished your turn!\r\n";
                    context.game().next_turn(context);
                }
        },
}) {}