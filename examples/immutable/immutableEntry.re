module ImmutableStoreProvider = {
  let lens = Reductive.Lens.make((a: AppState.appState) => a);
  let make = Reductive.Provider.createMake(~name="ImmutableStoreProvider", TimeTravelStore.store, lens);
};

ReactDOMRe.renderToElementWithId(
  <ImmutableStoreProvider component=ImmutableRenderer.make />,
  "index"
);
