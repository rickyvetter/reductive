@@bs.config({jsx: 3})
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

type appState = {
  counter: int,
  notACounter: string,
}

let appReducer = (state, action) =>
  switch action {
  | StringAction(action) => {...state, notACounter: stringReduce(state.notACounter, action)}
  | CounterAction(action) => {...state, counter: counter(state.counter, action)}
  | _ => state
  }

let thunkedLogger = (store, next) =>
  \"@@"(Middleware.thunk(store), \"@@"(Middleware.logger(store), next))

let appStore = Reductive.Store.create(
  ~reducer=appReducer,
  ~preloadedState={counter: 0, notACounter: ""},
  ~enhancer=thunkedLogger,
  (),
)

include ReductiveContext.Make({
  type state = appState
  type action = ReduxThunk.thunk<appState>
})
