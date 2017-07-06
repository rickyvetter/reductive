let component = ReasonReact.statelessComponent "DataRenderer";

let make state::(state: ThunkedStore.appState) ::dispatch _children => {
  let incrementIfOdd
      (store: Reductive.Store.t (ReduxThunk.thunk ThunkedStore.appState) ThunkedStore.appState) =>
    switch (Reductive.Store.getState store) {
    | {counter} when counter mod 2 === 1 =>
      Reductive.Store.dispatch store (ThunkedStore.CounterAction SimpleStore.Increment)
    | _ => ()
    };
  let incrementAsync store =>
    ignore (
      ReasonJs.setTimeout
        (
          fun () =>
            Reductive.Store.dispatch store (ThunkedStore.CounterAction SimpleStore.Increment)
        )
        1000
    );
  {
    ...component,
    render: fun _self =>
      <div>
        <div> (ReasonReact.stringToElement ("string: " ^ state.notACounter)) </div>
        <div> (ReasonReact.stringToElement ("counter: " ^ string_of_int state.counter)) </div>
        <button onClick=(fun _ => dispatch (ThunkedStore.CounterAction SimpleStore.Increment))>
          (ReasonReact.stringToElement "Increment")
        </button>
        <button onClick=(fun _ => dispatch (ThunkedStore.CounterAction SimpleStore.Decrement))>
          (ReasonReact.stringToElement "Decrement")
        </button>
        <button onClick=(fun _ => dispatch (ThunkedStore.StringAction ThunkedStore.A))>
          (ReasonReact.stringToElement "add a")
        </button>
        <button onClick=(fun _ => dispatch (ReduxThunk.Thunk incrementAsync))>
          (ReasonReact.stringToElement "Increment Async")
        </button>
        <button onClick=(fun _ => dispatch (ReduxThunk.Thunk incrementIfOdd))>
          (ReasonReact.stringToElement "Increment if Odd")
        </button>
      </div>
  }
};
