type thunk('state) = ..;

type thunk('state) +=
  | Thunk ((Reductive.Store.t(thunk('state), 'state) => unit));
