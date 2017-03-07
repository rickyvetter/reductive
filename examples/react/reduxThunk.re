type thunk _ = ..;

type thunk 'state +=
  | Thunk (Reductive.Store.t (thunk 'state) 'state => unit);

let thunk store next action =>
  switch action {
  | Thunk func => func store
  | _ => next action
  };
