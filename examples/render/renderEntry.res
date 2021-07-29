@@bs.config({jsx: 3})
/*
 * Example using multiple components to represent different slices of state.
 * Updating the state exposed by one component should not cause the other
 * components to also update (visually). Use the React Devtools "highlight
 * updates" feature to see this in action. If that proves difficult, then
 * try the Chrome devtools Rendering options, enabling "Paint flashing".
 */
type counterAction =
  | Increment
  | Decrement

let counterReduce = (state, action) =>
  switch action {
  | Increment => state + 1
  | Decrement => state - 1
  }

type stringAction =
  | AppendA
  | AppendB

let stringReduce = (state, action) =>
  switch action {
  | AppendA => state ++ "A"
  | AppendB => state ++ "B"
  }

type ReduxThunk.thunk<_> +=
  | StringAction(stringAction)
  | CounterAction(counterAction)

type appState = {
  counter: int,
  content: string,
}

let appReducer = (state, action) =>
  switch action {
  | StringAction(action) => {
      ...state,
      content: stringReduce(state.content, action),
    }
  | CounterAction(action) => {
      ...state,
      counter: counterReduce(state.counter, action),
    }
  | _ => state
  }

let thunkedLogger = (store, next) =>
  \"@@"(Middleware.thunk(store), \"@@"(Middleware.logger(store), next))

let appStore = Reductive.Store.create(
  ~reducer=appReducer,
  ~preloadedState={counter: 0, content: ""},
  ~enhancer=thunkedLogger,
  (),
)

module AppStore = {
  include ReductiveContext.Make({
    type state = appState
    type action = ReduxThunk.thunk<appState>
  })
}

let contentSelector = state => state.content
module StringComponent = {
  @react.component
  let make = () => {
    let dispatch = AppStore.useDispatch()
    let state = AppStore.useSelector(contentSelector)

    <div>
      <div> {ReasonReact.string("Content: " ++ state)} </div>
      <button onClick={_ => dispatch(StringAction(AppendA))}> {ReasonReact.string("+A")} </button>
      <button onClick={_ => dispatch(StringAction(AppendB))}> {ReasonReact.string("+B")} </button>
    </div>
  }
}

let counterSelector = state => state.counter

module CounterComponent = {
  @react.component
  let make = () => {
    let dispatch = AppStore.useDispatch()
    let state = AppStore.useSelector(counterSelector)

    <div>
      <div> {ReasonReact.string("Counter: " ++ string_of_int(state))} </div>
      <button onClick={_ => dispatch(CounterAction(Increment))}>
        {ReasonReact.string("++")}
      </button>
      <button onClick={_ => dispatch(CounterAction(Decrement))}>
        {ReasonReact.string("--")}
      </button>
    </div>
  }
}

module RenderApp = {
  @react.component
  let make = () => <div> <CounterComponent /> <StringComponent /> </div>
}

ReactDOMRe.renderToElementWithId(<RenderApp />, "index")
