[@bs.config {jsx: 3}];
module type Config = {
  type state;
  type action;

  let store: Reductive.Store.t(action, state);
};

module Make = (Config: Config) => {
  open Subscription;
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

    let source =
      React.useMemo2(
        () =>
          {
            subscribe: Reductive.Store.subscribe(storeFromContext),
            getCurrentValue: () =>
              selector(Reductive.Store.getState(storeFromContext)),
          },
        (selector, storeFromContext),
      );

    let selectedState = useSubscription(source);

    selectedState;
  };

  let useDispatch = () => {
    let storeFromContext = React.useContext(storeContext);
    Reductive.Store.dispatch(storeFromContext);
  };
};
