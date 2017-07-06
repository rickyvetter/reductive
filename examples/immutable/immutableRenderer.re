let component = ReasonReact.statelessComponent "ImmutableRenderer";

let make state::(state: AppState.appState) ::dispatch _children => {
  let incrementIfOdd
      (store: Reductive.Store.t (ReduxThunk.thunk AppState.appState) AppState.appState) =>
    switch (Reductive.Store.getState store) {
    | {counter} when counter mod 2 === 1 =>
      Reductive.Store.dispatch store (TimeTravelStore.CounterAction SimpleStore.Increment)
    | _ => ()
    };
  let incrementAsync store =>
    ignore (
      ReasonJs.setTimeout
        (
          fun () =>
            Reductive.Store.dispatch store (TimeTravelStore.CounterAction SimpleStore.Increment)
        )
        1000
    );
  {
    ...component,
    render: fun _self =>
      <div>
        <div> (ReasonReact.stringToElement ("string: " ^ state.notACounter)) </div>
        <div> (ReasonReact.stringToElement ("counter: " ^ string_of_int state.counter)) </div>
        <button onClick=(fun _ => dispatch (TimeTravelStore.CounterAction SimpleStore.Increment))>
          (ReasonReact.stringToElement "Increment")
        </button>
        <button onClick=(fun _ => dispatch (TimeTravelStore.CounterAction SimpleStore.Decrement))>
          (ReasonReact.stringToElement "Decrement")
        </button>
        <button onClick=(fun _ => dispatch (TimeTravelStore.StringAction TimeTravelStore.A))>
          (ReasonReact.stringToElement "add a")
        </button>
        <button onClick=(fun _ => dispatch (ReduxThunk.Thunk incrementAsync))>
          (ReasonReact.stringToElement "Increment Async")
        </button>
        <button onClick=(fun _ => dispatch (ReduxThunk.Thunk incrementIfOdd))>
          (ReasonReact.stringToElement "Increment if Odd")
        </button>
        <button onClick=(fun _ => dispatch TimeTravelStore.TravelBackward)>
          (ReasonReact.stringToElement "Undo")
        </button>
        <button onClick=(fun _ => dispatch TimeTravelStore.TravelForward)>
          (ReasonReact.stringToElement "Redo")
        </button>
      </div>
  }
};
