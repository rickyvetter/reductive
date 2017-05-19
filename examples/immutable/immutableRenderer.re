module ImmutableRenderer = {
  include ReactRe.Component;
  type props = {state: AppState.appState, dispatch: ReduxThunk.thunk AppState.appState => unit};
  let name = "ImmutableRenderer";
  let incrementAsync store =>
    ignore (
      ReasonJs.setTimeout
        (
          fun () =>
            Reductive.Store.dispatch store (TimeTravelStore.CounterAction SimpleStore.Increment)
        )
        1000
    );
  let incrementIfOdd
      (store: Reductive.Store.t (ReduxThunk.thunk AppState.appState) AppState.appState) =>
    switch (Reductive.Store.getState store) {
    | {counter} when counter mod 2 === 1 =>
      Reductive.Store.dispatch store (TimeTravelStore.CounterAction SimpleStore.Increment)
    | _ => ()
    };
  let render {props} =>
    <div>
      <div> (ReactRe.stringToElement ("string: " ^ props.state.notACounter)) </div>
      <div> (ReactRe.stringToElement ("counter: " ^ string_of_int props.state.counter)) </div>
      <button
        onClick=(fun _ => props.dispatch (TimeTravelStore.CounterAction SimpleStore.Increment))>
        (ReactRe.stringToElement "Increment")
      </button>
      <button
        onClick=(fun _ => props.dispatch (TimeTravelStore.CounterAction SimpleStore.Decrement))>
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
      <button onClick=(fun _ => props.dispatch TimeTravelStore.TravelBackward)>
        (ReactRe.stringToElement "Undo")
      </button>
      <button onClick=(fun _ => props.dispatch TimeTravelStore.TravelForward)>
        (ReactRe.stringToElement "Redo")
      </button>
    </div>;
};

include ReactRe.CreateComponent ImmutableRenderer;

let createElement ::state ::dispatch => wrapProps {state, dispatch};
