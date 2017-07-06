module ImmutableStoreProvider = {
  let make = Reductive.Provider.createMake TimeTravelStore.store;
};

ReactDOMRe.renderToElementWithId
  <ImmutableStoreProvider component=ImmutableRenderer.make /> "index";
