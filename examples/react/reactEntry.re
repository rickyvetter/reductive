module ThunkedStoreProvider = {
  let lens = Reductive.Lens.make((a: ThunkedStore.appState) => a);
  let make = Reductive.Provider.createMake(ThunkedStore.store, lens);
};

ReactDOMRe.renderToElementWithId(<ThunkedStoreProvider component=DataRenderer.make />, "index");
