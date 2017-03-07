module Store = {
  type t 'action 'state = {
    mutable state: 'state,
    mutable reducer: 'state => 'action => 'state,
    mutable listeners: list (unit => unit),
    customDispatcher: option (t 'action 'state => ('action => unit) => 'action => unit)
  };
  let create ::reducer ::preloadedState ::enhancer=? () =>
    switch (preloadedState, enhancer, reducer) {
    | (preloadedState, None, reducer) => {
        state: preloadedState,
        listeners: [],
        reducer,
        customDispatcher: None
      }
    | (preloadedState, Some enhancer, reducer) => {
        state: preloadedState,
        listeners: [],
        reducer,
        customDispatcher: Some enhancer
      }
    };
  let unsubscribe store listener () =>
    store.listeners = List.filter (fun l => listener !== l) store.listeners;
  let subscribe store listener => {
    store.listeners = [listener, ...store.listeners];
    unsubscribe store listener
  };
  let nativeDispatch store action => {
    store.state = store.reducer store.state action;
    List.iter (fun listener => listener ()) store.listeners
  };
  let dispatch store action =>
    switch store.customDispatcher {
    | Some customDispatcher => customDispatcher store (nativeDispatch store) action
    | None => nativeDispatch store action
    };
  let getState store => store.state;
  let replaceReducer store reducer => store.reducer = reducer;
};

module MakeProvider (StoreTypes: {type state; type action;}) => {
  module Provider = {
    include ReactRe.Component.Stateful.InstanceVars;
    type props = {
      store: Store.t StoreTypes.action StoreTypes.state,
      component:
        state::StoreTypes.state =>
        dispatch::(StoreTypes.action => unit) =>
        children::list ReactRe.reactElement =>
        ref::(ReactRe.reactRef => unit)? =>
        key::string? =>
        unit =>
        ReactRe.reactElement
    };
    type state = option StoreTypes.state;
    type instanceVars = {mutable unsubscribe: option (unit => unit)};
    let name = "Provider";
    let getInstanceVars _ => {unsubscribe: None};
    let getInitialState _ => None;
    let update {props} () => Some (Some (Store.getState props.store));
    let componentDidMount {props, instanceVars, updater} => {
      instanceVars.unsubscribe = Some (Store.subscribe props.store (updater update));
      Some (Some (Store.getState props.store))
    };
    let componentWillUnmount {instanceVars} =>
      switch instanceVars.unsubscribe {
      | Some unsubscribe => unsubscribe ()
      | None => ()
      };
    let dispatch {props} action => {
      Store.dispatch props.store action;
      None
    };
    let render {props, state, updater} =>
      switch state {
      | None => ReactRe.nullElement
      | Some state => props.component ::state dispatch::(updater dispatch) children::[] ()
      };
  };
  include ReactRe.CreateComponent Provider;
  let createElement ::store ::component => wrapProps {store, component};
};


/** These are all visible apis of Redux with explainations about how to achieve the same thing in Reason */
let compose _ => ();

let combineReducers _ => ();

let applyMiddleware _ => ();

let bindActionCreators actions dispatch => List.map (fun action () => dispatch action) actions;
