module ThunkedStoreProvider = {
  let make = Reductive.Provider.createMake(ThunkedStore.store);
};

ReactDOMRe.renderToElementWithId(<ThunkedStoreProvider component=DataRenderer.make />, "index");
