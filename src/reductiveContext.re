[@bs.config {jsx: 3}];
module type Config = {
  type state;
  type action;

  let store: Reductive.Store.t(action, state);
};

module Make = (Config: Config) => {
  open Subscription;
  let storeContext = React.createContext(None);

  module ContextProvider = {
    let make = React.Context.provider(storeContext);
    let makeProps =
        (
          ~value: option(Reductive.Store.t(Config.action, Config.state)),
          ~children,
          (),
        ) => {
      "value": value,
      "children": children,
    };
  };

  module Provider = {
    [@react.component]
    let make = (~children, ~store) => {
      <ContextProvider value={Some(store)}> children </ContextProvider>;
    };
  };

  let getStoreFromContext = () => {
    switch (React.useContext(storeContext)) {
    | None =>
      failwith(
        "could not find reactive context value; please ensure the component is wrapped in a <Provider>",
      )
    | Some(storeContext) => storeContext
    };
  };

  let useSelector = selector => {
    let store = getStoreFromContext();

    let source =
      React.useMemo2(
        () =>
          {
            subscribe: Reductive.Store.subscribe(store),
            getCurrentValue: () =>
              selector(Reductive.Store.getState(store)),
          },
        (selector, store),
      );

    let selectedState = useSubscription(source);

    selectedState;
  };

  let useDispatch = () => {
    Reductive.Store.dispatch(getStoreFromContext());
  };
  };
};
