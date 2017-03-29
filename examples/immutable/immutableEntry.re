module Blah =
  Reductive.MakeProvider {
    type state = AppState.appState;
    type action = ReduxThunk.thunk AppState.appState;
  };

ReactDOMRe.renderToElementWithId
  <Blah store=TimeTravelStore.store component=ImmutableRenderer.createElement /> "index";
