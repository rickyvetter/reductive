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

  type source('a) = {
    subscribe: (unit => unit, unit) => unit,
    getCurrentValue: unit => 'a,
    value: 'a,
  };

  let useSubscription =
      (subscribe: (unit => unit, unit) => unit, getCurrentValue: unit => 'a)
      : 'a => {
    let hasSubscriptionChanged = prevState =>
      prevState.getCurrentValue !== getCurrentValue
      || prevState.subscribe !== subscribe;

    let (state, setState) =
      React.useState(() =>
        {getCurrentValue, subscribe, value: getCurrentValue()}
      );

    React.useEffect2(
      () => {
        let didUnsubscribe = ref(false);

        let updateState = () =>
          setState(prevState =>
            if (hasSubscriptionChanged(prevState)) {
              prevState;
            } else {
              let newValue = getCurrentValue();

              prevState.value === newValue
                ? prevState : {...prevState, value: newValue};
            }
          );

        let checkForUpdates = () =>
          if (! didUnsubscribe^) {
            updateState();
          };

        // state value might have changed between unscribing and subscribing again due
        // to a new selector function (e.g. because of a change in props used inside of it).
        let newValue = getCurrentValue();
        if (state.value !== newValue) {
          setState(prevState => {...prevState, value: newValue});
        };

        let unsubscribe = subscribe(checkForUpdates);

        Some(
          () => {
            didUnsubscribe := true;
            unsubscribe();
          },
        );
      },
      (getCurrentValue, subscribe),
    );

    // evaluate the return value
    if (hasSubscriptionChanged(state)) {
      let newValue = getCurrentValue();
      setState(_ => {getCurrentValue, subscribe, value: newValue});

      newValue;
    } else {
      state.value;
    };
  };

  let useSelector = selector => {
    let storeFromContext = React.useContext(storeContext);
    let subscribe =
      React.useCallback1(
        Reductive.Store.subscribe(storeFromContext),
        [|storeFromContext|],
      );

    let getCurrentValue =
      React.useCallback2(
        () => selector(Reductive.Store.getState(storeFromContext)),
        (selector, storeFromContext),
      );

    let selectedState = useSubscription(subscribe, getCurrentValue);

    selectedState;
  };

  let useDispatch = () => {
    let storeFromContext = React.useContext(storeContext);
    Reductive.Store.dispatch(storeFromContext);
  };
};
