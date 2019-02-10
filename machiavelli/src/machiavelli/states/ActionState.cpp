#include "machiavelli/states/ActionState.hpp"

using namespace server;
using namespace server::command;
using namespace server::command::validate;
using namespace server::connection;
using namespace server::player;

ActionState::ActionState(Context &ctx) : BaseState(ctx, {}) {}