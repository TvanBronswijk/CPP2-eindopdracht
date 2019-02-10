#include "machiavelli/states/IdleState.hpp"

using namespace server;
using namespace server::command;
using namespace server::command::validate;
using namespace server::connection;
using namespace server::player;

IdleState::IdleState(Context &ctx) : BaseState(ctx, {}) {}