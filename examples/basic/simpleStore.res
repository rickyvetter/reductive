type action =
  | Increment
  | Decrement

let counter = (state, action) =>
  switch action {
  | Increment => state + 1
  | Decrement => state - 1
  }

let doubleCounter = (state, action) =>
  switch action {
  | Increment => state + 2
  | Decrement => state - 2
  }

let store = Reductive.Store.create(~reducer=counter, ~preloadedState=0, ())
