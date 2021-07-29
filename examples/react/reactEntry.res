ReactDOM.querySelector("#index")->Belt.Option.forEach(
  ReactDOM.render(
    <ThunkedStore.Provider store=ThunkedStore.appStore> <DataRenderer /> </ThunkedStore.Provider>,
  ),
)
