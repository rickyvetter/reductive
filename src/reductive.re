module Store = {
  type t('action, 'state) = {
    mutable state: 'state,
    mutable reducer: ('state, 'action) => 'state,
    mutable listeners: list(unit => unit),
    customDispatcher:
      option((t('action, 'state), 'action => unit, 'action) => unit),
  };
  let create = (~reducer, ~preloadedState, ~enhancer=?, ()) =>
    switch (preloadedState, enhancer, reducer) {
    | (preloadedState, None, reducer) => {
        state: preloadedState,
        listeners: [],
        reducer,
        customDispatcher: None,
      }
    | (preloadedState, Some(enhancer), reducer) => {
        state: preloadedState,
        listeners: [],
        reducer,
        customDispatcher: Some(enhancer),
      }
    };
  let unsubscribe = (store, listener, ()) =>
    store.listeners = List.filter(l => listener !== l, store.listeners);
  let subscribe = (store, listener) => {
    store.listeners = [listener, ...store.listeners];
    unsubscribe(store, listener);
  };
  let nativeDispatch = (store, action) => {
    store.state = store.reducer(store.state, action);
    List.iter(listener => listener(), store.listeners);
  };
  let dispatch = (store, action) =>
    switch (store.customDispatcher) {
    | Some(customDispatcher) =>
      customDispatcher(store, nativeDispatch(store), action)
    | None => nativeDispatch(store, action)
    };
  let getState = store => store.state;
  let replaceReducer = (store, reducer) => store.reducer = reducer;
};

module type Config = {
  type state;
  type action;

  let store: Store.t(action, state);
};

module Make = (Config: Config) => {
  module Context = {
    type t = Store.t(Config.action, Config.state);
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

        let newSelectedState = selector(Store.getState(Config.store));
        React.Ref.setCurrent(latestSelectedState, Some(newSelectedState));
        None;
      },
      [|selector|],
    );

    React.useLayoutEffect1(
      () => {
        let checkForUpdates = () => {
          let newSelectedState = selector(Store.getState(Config.store));

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
        Some(Store.subscribe(storeFromContext, checkForUpdates));
      },
      [|storeFromContext|],
    );

    let currentStoreState = Store.getState(Config.store);
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
    Store.dispatch(storeFromContext);
  };
};

/*** These are all visible apis of Redux that aren't needed in Reason.
 * When used, build tools will provide explanation of alternatives.
 * (see .rei for those)
 */
let compose = _ => ();

let combineReducers = _ => ();

let applyMiddleware = _ => ();

let bindActionCreators = (actions, dispatch) =>
  List.map((action, ()) => dispatch(action), actions);
