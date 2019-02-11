#include "machiavelli/states/IdleState.hpp"

using namespace server;
using namespace server::command;
using namespace server::command::validate;
using namespace server::connection;
using namespace server::player;

IdleState::IdleState(Context &ctx) : BaseState(ctx, {
        {
                "info",
                "Get your current character info.",
                [](StringArgs args) { return validate_that<StringArgs>(args, is_empty<std::string>); },
                [&](StringArgs args, Player& player, Socket& socket, Context& context) {
                    auto& data = player.get_data<GameData>();
                    socket << "Gold: " << data.gold_coins << "\r\n";
                    socket << "Character Cards:\r\n";
                    std::for_each(data.character_cards.begin(), data.character_cards.end(), [&](CharacterCard& cc){
                        socket << cc.description() << "\r\n";
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
        }
}) {}