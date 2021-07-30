let stateSelector = state => state

@react.component
let make = () => {
  let state = ThunkedStore.useSelector(stateSelector)
  let dispatch = ThunkedStore.useDispatch()

  let incrementIfOdd = (
    store: Reductive.Store.t<ReduxThunk.thunk<ThunkedStore.appState>, ThunkedStore.appState>,
  ) =>
    switch Reductive.Store.getState(store) {
    | {counter} if mod(counter, 2) === 1 =>
      Reductive.Store.dispatch(store, ThunkedStore.CounterAction(SimpleStore.Increment))
    | _ => ()
    }
  let incrementAsync = store =>
    ignore(
      Js.Global.setTimeout(
        () => Reductive.Store.dispatch(store, ThunkedStore.CounterAction(SimpleStore.Increment)),
        1000,
      ),
    )

  <div>
    <div> {React.string("string: " ++ state.notACounter)} </div>
    <div> {React.string("counter: " ++ string_of_int(state.counter))} </div>
    <button onClick={_ => dispatch(ThunkedStore.CounterAction(SimpleStore.Increment))}>
      {React.string("Increment")}
    </button>
    <button onClick={_ => dispatch(ThunkedStore.CounterAction(SimpleStore.Decrement))}>
      {React.string("Decrement")}
    </button>
    <button onClick={_ => dispatch(ThunkedStore.StringAction(ThunkedStore.A))}>
      {React.string("add a")}
    </button>
    <button onClick={_ => dispatch(ReduxThunk.Thunk(incrementAsync))}>
      {React.string("Increment Async")}
    </button>
    <button onClick={_ => dispatch(ReduxThunk.Thunk(incrementIfOdd))}>
      {React.string("Increment if Odd")}
    </button>
  </div>
}
