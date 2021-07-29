let unsubscribe = Reductive.Store.subscribe(SimpleStore.store, () => Js.log("store has updated"))

let dispatch = Reductive.Store.dispatch(SimpleStore.store)

dispatch(Increment)

dispatch(Increment)

dispatch(Increment)

dispatch(Decrement)

dispatch(Increment)

Reductive.Store.subscribe(SimpleStore.store, () =>
  Js.log(Reductive.Store.getState(SimpleStore.store))
)

/* when replacing reducers, the action and state types must match */
Reductive.Store.replaceReducer(SimpleStore.store, SimpleStore.doubleCounter)

dispatch(Increment)

unsubscribe()

dispatch(Increment)

dispatch(Increment)

dispatch(Increment)

dispatch(Increment)

dispatch(Increment)

dispatch(Increment)

Reductive.Store.subscribe(ComplexStore.store, () =>
  Js.log(Reductive.Store.getState(ComplexStore.store))
)

Reductive.Store.dispatch(ComplexStore.store, ComplexStore.StringAction(A))

Reductive.Store.dispatch(ComplexStore.store, ComplexStore.StringAction(B))

Reductive.Store.dispatch(ComplexStore.store, ComplexStore.CounterAction(Increment))
