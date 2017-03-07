module Blah =
  Reductive.MakeProvider {
    type state = ThunkedStore.appState;
    type action = ReduxThunk.thunk ThunkedStore.appState;
  };

ReactDOMRe.render
  <Blah store=ThunkedStore.store component=DataRenderer.createElement />
  (ReasonJs.Document.getElementById "index");

Reductive.Store.subscribe
  ThunkedStore.store (fun () => Js.log (Reductive.Store.getState ThunkedStore.store));
