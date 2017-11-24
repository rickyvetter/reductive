module ImmutableStoreProvider = {
  let make = Reductive.Provider.createMake(~name="ImmutableStoreProvider", TimeTravelStore.store);
};

ReactDOMRe.renderToElementWithId(
  <ImmutableStoreProvider component=ImmutableRenderer.make />,
  "index"
);
