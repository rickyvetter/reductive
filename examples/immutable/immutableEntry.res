@@bs.config({jsx: 3})
ReactDOMRe.renderToElementWithId(
  <TimeTravelStore.Provider store=TimeTravelStore.timeTravelStore>
    <ImmutableRenderer />
  </TimeTravelStore.Provider>,
  "index",
)
