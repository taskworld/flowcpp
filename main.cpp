#include <iostream>
#include <vector>

#include <flowcpp/flow.h>

enum class counter_action_type {
  nothing, increment, decrement
};

struct increment_action {
  const void* payload() const { return &_payload; }

  const void* type() const { return &_type; }

  const void* meta() const { return _meta; }

  bool error() const { return _error; }

  int _payload = {1};
  counter_action_type _type = {counter_action_type::increment};
  void *_meta = nullptr;
  bool _error = false;
};

struct decrement_action {
  const void* payload() const { return &_payload; }

  const void* type() const { return &_type; }

  const void *meta() const { return _meta; }

  bool error() const { return _error; }

  int _payload = {1};
  counter_action_type _type = {counter_action_type::decrement};
  void *_meta = nullptr;
  bool _error = false;
};

struct counter_state {
  std::string to_string() {
    return "counter: " + std::to_string(_counter);
  }

  int _counter{0};
};

auto reducer = [](counter_state state, flow::action action) {
  int multiplier = 1;
  auto type = *static_cast<const counter_action_type*>(action.type());
  switch (type) {
    case counter_action_type::decrement:
      multiplier = -1;
      break;
    case counter_action_type::increment:
      multiplier = 1;
      break;
    case counter_action_type::nothing:
      break;
  }
  auto payload = *static_cast<const int*>(action.payload());
  state._counter += multiplier * payload;
  return state;
};

int main() {

  // store
  auto s = flow::create_store_with_action<counter_state>(reducer, counter_state(), increment_action{10});

  // disposable
  auto d = s.subscribe([](counter_state state) { std::cout << state.to_string() << std::endl; });

  s.dispatch(increment_action{2});

  d.disposable()();

  s.dispatch(decrement_action{10});
  s.dispatch(increment_action{3});
  s.dispatch(decrement_action{5});

  std::cout << s.state().to_string() << std::endl;

  return 0;
}
