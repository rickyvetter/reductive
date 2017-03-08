module Blah =
  Reductive.MakeProvider {
    type state = TimeTravelStore.appState;
    type action = ReduxThunk.thunk TimeTravelStore.appState;
  };

ReactDOMRe.render
  <Blah store=TimeTravelStore.store component=ImmutableRenderer.createElement />
  (ReasonJs.Document.getElementById "index");

Reductive.Store.subscribe
  TimeTravelStore.store (fun () => Js.log (Reductive.Store.getState TimeTravelStore.store));
