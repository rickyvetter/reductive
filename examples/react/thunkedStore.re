open SimpleStore;

type stringAction =
  | A
  | B;

let stringReduce state action =>
  switch action {
  | A => state ^ "a"
  | B => state ^ "b"
  };

type ReduxThunk.thunk _ +=
  | StringAction stringAction
  | CounterAction action;

type appState = {counter: int, notACounter: string};

let appReducter state action =>
  switch action {
  | StringAction action => {...state, notACounter: stringReduce state.notACounter action}
  | CounterAction action => {...state, counter: counter state.counter action}
  | _ => state
  };

let thunkedLogger store next =>
  Middleware.thunk store @@
  Middleware.logger store @@
  next;

let store =
  Reductive.Store.create
    reducer::appReducter preloadedState::{counter: 0, notACounter: ""} enhancer::thunkedLogger ();
