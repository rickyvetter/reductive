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
    <div> {React.string("string: " ++ state.notACounter)} </div>
    <div> {React.string("counter: " ++ string_of_int(state.counter))} </div>
    <button onClick={_ => dispatch(TimeTravelStore.CounterAction(SimpleStore.Increment))}>
      {React.string("Increment")}
    </button>
    <button onClick={_ => dispatch(TimeTravelStore.CounterAction(SimpleStore.Decrement))}>
      {React.string("Decrement")}
    </button>
    <button onClick={_ => dispatch(TimeTravelStore.StringAction(TimeTravelStore.A))}>
      {React.string("add a")}
    </button>
    <button onClick={_ => dispatch(ReduxThunk.Thunk(incrementAsync))}>
      {React.string("Increment Async")}
    </button>
    <button onClick={_ => dispatch(ReduxThunk.Thunk(incrementIfOdd))}>
      {React.string("Increment if Odd")}
    </button>
    <button onClick={_ => dispatch(TimeTravelStore.TravelBackward)}>
      {React.string("Undo")}
    </button>
    <button onClick={_ => dispatch(TimeTravelStore.TravelForward)}> {React.string("Redo")} </button>
  </div>
}
