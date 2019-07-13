[@bs.config {jsx: 3}];
module type Config = {
  type state;
  type action;

  let store: Reductive.Store.t(action, state);
};

module Make = (Config: Config) => {
  let storeContext = React.createContext(Config.store);

  module ContextProvider = {
    let make = React.Context.provider(storeContext);
    let makeProps = (~value, ~children, ()) => {
      "value": value,
      "children": children,
    };
  };

  module Provider = {
    [@react.component]
    let make = (~children) => {
      <ContextProvider value=Config.store> children </ContextProvider>;
    };
  };

  let useSelector = selector => {
    let storeFromContext = React.useContext(storeContext);
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
    let storeFromContext = React.useContext(storeContext);
    Reductive.Store.dispatch(storeFromContext);
  };
};
