#include "machiavelli/states/ActionState.hpp"
#include "machiavelli/states/OptionState.hpp"

using namespace server;
using namespace server::command;
using namespace server::command::options;
using namespace server::command::validate;
using namespace server::connection;
using namespace server::player;

ActionState::ActionState(Context &ctx) : _take_gold_or_card(false), _character_action(false), _built_building(false), _built_buildings(0),
BaseState(ctx, {
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
                    if(!_built_building) {
                        auto& data = player.get_data<GameData>();
                        std::vector<std::string> options;
                        std::transform(data.building_cards.begin(), data.building_cards.end(), std::back_inserter(options), [](BuildingCard& bc){
                            return bc.name();
                        });
                        OptionHandler handler{options, [&](int i) -> bool {
                            BuildingCard b = data.building_cards.take(i);
                            if(data.gold_coins >= b.coins()) {
                                socket << "You built a " << b.name() << "!\r\n";
                                data.gold_coins -= b.coins();
                                data.built_buildings.add(b);
                                if(context.game().get_card(context.game().get_current_turn()).name() == "Bouwmeester") {
                                    _built_buildings++;
                                    if(_built_buildings >= 3) {
                                        _built_building = true;
                                    }
                                } else {
                                    _built_building = true;
                                }
                            } else {
                                data.building_cards.add(b);
                                socket << "You can't afford that!\r\n";
                            }
                            return true;
                        }};
                        socket << handler;
                        player.get_states().put(std::make_unique<OptionState>(context, handler));
                    } else {
                        socket << "You cannot build anymore!\r\n";
                    }
                }
        },
        {
                "finish",
                "End your turn.",
                [](StringArgs args) { return validate_that<StringArgs>(args, is_empty<std::string>); },
                [&](StringArgs args, Player& player, Socket& socket, Context& context) {
                    socket << "You finished your turn!\r\n";
                    player.get_states().pop();
                    context.game().next_turn(context);
                }
        },
}) {}