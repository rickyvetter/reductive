open CounterStore;

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

let logger store next action => {
  Js.log "will dispatch";
  Js.log action;
  let returnValue = next action;
  Js.log "state after dispatch";
  Js.log (Reductive.Store.getState store);
  returnValue
};

let states = ref Immutable.Deque.empty;

let timeTravel store next action => {
  Js.log "wowowowow";
  let returnValue = next action;
  states := Immutable.Deque.addLast (Reductive.Store.getState store) !states;
  Js.log states;
  returnValue
};

let thunkedLogger store next action =>
  ignore (
    ReduxThunk.thunk
      store
      (
        fun action =>
          ignore (logger store (fun action => ignore (timeTravel store next action)) action)
      )
      action
  );

/* let a = Reductive.compose appReducter; */
let store =
  Reductive.Store.create
    reducer::appReducter preloadedState::{counter: 0, notACounter: ""} enhancer::thunkedLogger ();
