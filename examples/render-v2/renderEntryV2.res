/*
 * Example using multiple components to represent different slices of state.
 * Updating the state exposed by one component should not cause the other
 * components to also update (visually). Use the React Devtools "highlight
 * updates" feature to see this in action. If that proves difficult, then
 * try the Chrome devtools Rendering options, enabling "Paint flashing".
 */

module Counter = {
  type counterAction =
    | Increment
    | Decrement

  let counterReduce = (state, action) =>
    switch action {
    | Increment => state + 1
    | Decrement => state - 1
    }
}

module Content = {
  type contentAction =
    | AppendA
    | AppendB

  let contentReduce = (state, action) =>
    switch action {
    | AppendA => state ++ "A"
    | AppendB => state ++ "B"
    }
}

module App = {
  type appState = {
    counter: int,
    content: string,
  }

  type appAction =
    | CounterAction(Counter.counterAction)
    | StringAction(Content.contentAction)

  let appReducer = (state, action) =>
    switch action {
    | StringAction(action) => {
        ...state,
        content: Content.contentReduce(state.content, action),
      }
    | CounterAction(action) => {
        ...state,
        counter: Counter.counterReduce(state.counter, action),
      }
    }
}

module Store = {
  include ReductiveContext.Make({
    type action = App.appAction
    type state = App.appState
  })

  let logger = (store, next) => \"@@"(Middleware.logger(store), next)

  let store = Reductive.Store.create(
    ~reducer=App.appReducer,
    ~preloadedState={counter: 0, content: ""},
    ~enhancer=logger,
    (),
  )

  module Selectors = {
    open App
    let state = state => state
    let contentState = state => state.content
    let counterState = state => state.counter
  }
}

module ContentComponent = {
  @react.component
  let make = () => {
    let dispatch = Store.useDispatch()
    let state = Store.useSelector(Store.Selectors.contentState)
    <div>
      <div> {ReasonReact.string("Content: " ++ state)} </div>
      <button onClick={_ => dispatch(StringAction(AppendA))}> {ReasonReact.string("+A")} </button>
      <button onClick={_ => dispatch(StringAction(AppendB))}> {ReasonReact.string("+B")} </button>
    </div>
  }
}

module CounterComponent = {
  @react.component
  let make = () => {
    let dispatch = Store.useDispatch()
    let state = Store.useSelector(Store.Selectors.counterState)
    <div>
      <div> {React.string("Counter: " ++ string_of_int(state))} </div>
      <button onClick={_ => dispatch(CounterAction(Increment))}> {React.string("++")} </button>
      <button onClick={_ => dispatch(CounterAction(Decrement))}> {React.string("--")} </button>
    </div>
  }
}

module RenderApp = {
  @react.component
  let make = () =>
    <div>
      <Store.Provider store=Store.store> <CounterComponent /> <ContentComponent /> </Store.Provider>
    </div>
}

ReactDOMRe.renderToElementWithId(<RenderApp />, "index")
