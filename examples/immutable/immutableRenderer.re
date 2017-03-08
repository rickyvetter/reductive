module ImmutableRenderer = {
  include ReactRe.Component;
  type props = {
    state: TimeTravelStore.appState,
    dispatch: ReduxThunk.thunk TimeTravelStore.appState => unit
  };
  let name = "ImmutableRenderer";
  let handleClick _ _ => {
    Js.log "clicked!";
    None
  };
  let incrementAsync store =>
    ignore (
      ReasonJs.setTimeout
        (
          fun () =>
            Reductive.Store.dispatch store (TimeTravelStore.CounterAction CounterStore.Increment)
        )
        1000
    );
  let incrementIfOdd
      (store: Reductive.Store.t (ReduxThunk.thunk TimeTravelStore.appState) TimeTravelStore.appState) =>
    switch (Reductive.Store.getState store) {
    | {counter} when counter mod 2 === 1 =>
      Reductive.Store.dispatch store (TimeTravelStore.CounterAction CounterStore.Increment)
    | _ => ()
    };
  let render {props, updater} =>
    <div onClick=(updater handleClick)>
      <div> (ReactRe.stringToElement ("string: " ^ props.state.notACounter)) </div>
      <div> (ReactRe.stringToElement ("counter: " ^ string_of_int props.state.counter)) </div>
      <button
        onClick=(fun _ => props.dispatch (TimeTravelStore.CounterAction CounterStore.Increment))>
        (ReactRe.stringToElement "Increment")
      </button>
      <button
        onClick=(fun _ => props.dispatch (TimeTravelStore.CounterAction CounterStore.Decrement))>
        (ReactRe.stringToElement "Decrement")
      </button>
      <button onClick=(fun _ => props.dispatch (TimeTravelStore.StringAction TimeTravelStore.A))>
        (ReactRe.stringToElement "add a")
      </button>
      <button onClick=(fun _ => props.dispatch (ReduxThunk.Thunk incrementAsync))>
        (ReactRe.stringToElement "Increment Async")
      </button>
      <button onClick=(fun _ => props.dispatch (ReduxThunk.Thunk incrementIfOdd))>
        (ReactRe.stringToElement "Increment if Odd")
      </button>
    </div>;
};

include ReactRe.CreateComponent ImmutableRenderer;

let createElement ::state ::dispatch => wrapProps {state, dispatch};
