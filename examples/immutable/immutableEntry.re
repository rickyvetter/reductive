module ImmutableStoreProvider =
  Reductive.MakeProvider {
    type state = AppState.appState;
    type action = ReduxThunk.thunk AppState.appState;
  };

ReactDOMRe.renderToElementWithId
  <ImmutableStoreProvider store=TimeTravelStore.store component=ImmutableRenderer.createElement />
  "index";
