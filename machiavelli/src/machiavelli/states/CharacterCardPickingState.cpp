#include "machiavelli/states/CharacterCardPickingState.hpp"

CharacterCardPickingState::CharacterCardPickingState(Context &ctx, bool pick)
: BaseState(ctx, {}), _pick(pick) {}

bool CharacterCardPickingState::handle(server::input::Command cmd) {
    if(auto ptr = cmd.get_client_info().lock()) {
        try {
            if(cmd.get_cmd().length() >= 2) {
                std::string num = cmd.get_cmd();
                num.erase(std::remove(num.begin(), num.end(), '!'), num.end());
                int i = std::stoi(num);
                return pick_card(cmd.get_client_info(), i);
            }
        }catch(...) {
            BaseState::handle(cmd);
        }
    }
    return false;
}

bool CharacterCardPickingState::pick_card(std::weak_ptr<server::ClientInfo> client, int i) {
    if(i < 0 || i >= _ctx->game().character_cards.size()) return false;

    if(auto clientptr = client.lock()) {
        auto& data = clientptr->get_player().get_data<GameData>();
        if(_pick) {
            auto card = _ctx->game().character_cards.take(i);
            _ctx->game().game_order[card.order()] = clientptr;
            data.character_cards.add(card);
            auto other = _ctx->game().other_player(clientptr->get_player());
            if(_ctx->game().character_cards.size() == 0) {
                clientptr->get_player().get_states().pop();
                _ctx->game().next_turn(*_ctx); return true; }
            if(auto otherptr = other.lock()) {
                otherptr->get_socket() << "Pick one to drop:\r\n";
                for(int i = 0; i < _ctx->game().character_cards.size(); i++)
                    otherptr->get_socket() << '[' << std::to_string(i) << "] " << _ctx->game().character_cards.peek(i).name() << "\r\n";
                clientptr->get_player().get_states().pop();
                otherptr->get_player().get_states().put(std::make_unique<CharacterCardPickingState>(*_ctx, false));
            }
        } else {
            _ctx->game().character_cards.take(i);
            if(_ctx->game().character_cards.size() == 0) {
                clientptr->get_player().get_states().pop();
                _ctx->game().next_turn(*_ctx); return true; }
            clientptr->get_socket() << "Pick one to take:\r\n";
            for(int i = 0; i < _ctx->game().character_cards.size(); i++)
                clientptr->get_socket() << '[' << std::to_string(i) << "] " << _ctx->game().character_cards.peek(i).name() << "\r\n";
            clientptr->get_player().get_states().pop();
            clientptr->get_player().get_states().put(std::make_unique<CharacterCardPickingState>(*_ctx, true));
        }
    }
    return true;
}
