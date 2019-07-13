[@bs.config {jsx: 3}];
module Context = {
  module type Config = {
    type context;
    let defaultValue: context;
  };
  
  module Make = (Config: Config) => {
    let context = React.createContext(Config.defaultValue);

    module Provider = {
      let make = context->React.Context.provider;
      [@bs.obj]
      external makeProps:
        (
          ~value: Config.context,
          ~children: React.element,
          ~key: string=?,
          unit
        ) =>
        {
          .
          "value": Config.context,
          "children": React.element,
        } =
        "";
    };
  };  
}

module type Config = {
  type state;
  type action;

  let store: Reductive.Store.t(action, state);
};

module Make = (Config: Config) => {
  module Context = {
    type t = Reductive.Store.t(Config.action, Config.state);
    include Context.Make({
      type context = t;
      let defaultValue = Config.store;
    });
  };

  module Provider = {
    [@react.component]
    let make = (~children) => {
      <Context.Provider value=Config.store> children </Context.Provider>;
    };
  };

  let useSelector = selector => {
    let storeFromContext = React.useContext(Context.context);
    let (_, forceRerender) = React.useReducer((s, _) => s + 1, 0);

    // initialize ref to None to avoid reevaluating selector function on each rerender
    let latestSelectedState = React.useRef(None);
    let latestSelector = React.useRef(selector);

    // selector function might change if using components' props to select state
    React.useLayoutEffect1(
      () => {
        React.Ref.setCurrent(latestSelector, selector);

        let newSelectedState =
          selector(Reductive.Store.getState(Config.store));
        React.Ref.setCurrent(latestSelectedState, Some(newSelectedState));
        None;
      },
      [|selector|],
    );

    React.useLayoutEffect1(
      () => {
        let checkForUpdates = () => {
          let newSelectedState =
            selector(Reductive.Store.getState(Config.store));

          let hasStateChanged =
            switch (React.Ref.current(latestSelectedState)) {
            | None => true
            | Some(state) => newSelectedState !== state
            };

          if (hasStateChanged) {
            React.Ref.setCurrent(
              latestSelectedState,
              Some(newSelectedState),
            );
            forceRerender();
          };
        };
        Some(Reductive.Store.subscribe(storeFromContext, checkForUpdates));
      },
      [|storeFromContext|],
    );

    let currentStoreState = Reductive.Store.getState(Config.store);
    if (React.Ref.current(latestSelector) !== selector) {
      selector(currentStoreState);
    } else {
      switch (React.Ref.current(latestSelectedState)) {
      | None => selector(currentStoreState)
      | Some(state) => state
      };
    };
  };

  let useDispatch = () => {
    let storeFromContext = React.useContext(Context.context);
    Reductive.Store.dispatch(storeFromContext);
  };
};
