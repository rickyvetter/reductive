open SimpleStore

type stringAction =
  | A
  | B

let stringReduce = (state, action) =>
  switch action {
  | A => state ++ "a"
  | B => state ++ "b"
  }

type ReduxThunk.thunk<_> +=
  | StringAction(stringAction)
  | CounterAction(action)

type ReduxThunk.thunk<'a> +=
  | ReplaceState('a)

let appReducter = (state: AppState.appState, action) =>
  switch action {
  | StringAction(action) => {...state, notACounter: stringReduce(state.notACounter, action)}
  | CounterAction(action) => {...state, counter: counter(state.counter, action)}
  | ReplaceState(replacedState) => replacedState
  | _ => state
  }

type ReduxThunk.thunk<_> +=
  | TravelBackward
  | TravelForward

let past = ref(list{})

let future = ref(list{})

let goBack = currentState =>
  switch Belt.List.head(past.contents) {
  | Some(lastState) =>
    future := Belt.List.add(future.contents, currentState)
    past :=
      switch past.contents {
      | list{} => list{}
      | list{_, ...t} => t
      }
    lastState
  | None => currentState
  }

let goForward = currentState =>
  switch Belt.List.head(future.contents) {
  | Some(nextState) =>
    past := Belt.List.add(past.contents, currentState)
    future :=
      switch future.contents {
      | list{} => list{}
      | list{_, ...t} => t
      }

    nextState
  | None => currentState
  }

let recordHistory = currentState => {
  past := Belt.List.add(past.contents, currentState)
  future := list{}
}

let timeTravel = (store, next, action) => {
  let currentState = Reductive.Store.getState(store)
  switch action {
  | TravelBackward => next(ReplaceState(goBack(currentState)))
  | TravelForward => next(ReplaceState(goForward(currentState)))
  | _ =>
    next(action)
    let newState = Reductive.Store.getState(store)
    if currentState !== newState {
      recordHistory(currentState)
    }
  }
}

let thunkedLoggedTimeTravelLogger = (store, next) =>
  \"@@"(Middleware.thunk(store), \"@@"(Middleware.logger(store), \"@@"(timeTravel(store), next)))

let timeTravelStore = Reductive.Store.create(
  ~reducer=appReducter,
  ~preloadedState={counter: 0, notACounter: ""},
  ~enhancer=thunkedLoggedTimeTravelLogger,
  (),
)

include ReductiveContext.Make({
  type action = ReduxThunk.thunk<AppState.appState>
  type state = AppState.appState
})
