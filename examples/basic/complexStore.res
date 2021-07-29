open SimpleStore

type stringAction =
  | A
  | B

let stringReduce = (state, action) =>
  switch action {
  | A => state ++ "a"
  | B => state ++ "b"
  }

type appActions =
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
  }

let store = Reductive.Store.create(
  ~reducer=appReducer,
  ~preloadedState={counter: 0, notACounter: ""},
  ~enhancer=Middleware.logger,
  (),
)
