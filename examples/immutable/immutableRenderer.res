@@bs.config({jsx: 3})

let stateSelector = state => state
@react.component
let make = () => {
  let state = TimeTravelStore.useSelector(stateSelector)
  let dispatch = TimeTravelStore.useDispatch()

  let incrementIfOdd = (
    store: Reductive.Store.t<ReduxThunk.thunk<AppState.appState>, AppState.appState>,
  ) =>
    switch Reductive.Store.getState(store) {
    | {counter} if mod(counter, 2) === 1 =>
      Reductive.Store.dispatch(store, TimeTravelStore.CounterAction(SimpleStore.Increment))
    | _ => ()
    }
  let incrementAsync = store =>
    ignore(
      Js.Global.setTimeout(
        () => Reductive.Store.dispatch(store, TimeTravelStore.CounterAction(SimpleStore.Increment)),
        1000,
      ),
    )

  <div>
    <div> {ReasonReact.string("string: " ++ state.notACounter)} </div>
    <div> {ReasonReact.string("counter: " ++ string_of_int(state.counter))} </div>
    <button onClick={_ => dispatch(TimeTravelStore.CounterAction(SimpleStore.Increment))}>
      {ReasonReact.string("Increment")}
    </button>
    <button onClick={_ => dispatch(TimeTravelStore.CounterAction(SimpleStore.Decrement))}>
      {ReasonReact.string("Decrement")}
    </button>
    <button onClick={_ => dispatch(TimeTravelStore.StringAction(TimeTravelStore.A))}>
      {ReasonReact.string("add a")}
    </button>
    <button onClick={_ => dispatch(ReduxThunk.Thunk(incrementAsync))}>
      {ReasonReact.string("Increment Async")}
    </button>
    <button onClick={_ => dispatch(ReduxThunk.Thunk(incrementIfOdd))}>
      {ReasonReact.string("Increment if Odd")}
    </button>
    <button onClick={_ => dispatch(TimeTravelStore.TravelBackward)}>
      {ReasonReact.string("Undo")}
    </button>
    <button onClick={_ => dispatch(TimeTravelStore.TravelForward)}>
      {ReasonReact.string("Redo")}
    </button>
  </div>
}
