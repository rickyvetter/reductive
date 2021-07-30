ReactDOM.querySelector("#index")->Belt.Option.forEach(
  ReactDOM.render(
    <TimeTravelStore.Provider store=TimeTravelStore.timeTravelStore>
      <ImmutableRenderer />
    </TimeTravelStore.Provider>,
  ),
)
