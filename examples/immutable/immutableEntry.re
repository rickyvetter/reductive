module Blah =
  Reductive.MakeProvider {
    type state = AppState.appState;
    type action = ReduxThunk.thunk AppState.appState;
  };

ReactDOMRe.render
  <Blah store=TimeTravelStore.store component=ImmutableRenderer.createElement />
  (ReasonJs.Document.getElementById "index");
