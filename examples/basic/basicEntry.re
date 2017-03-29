let unsubscribe =
  Reductive.Store.subscribe
    SimpleStore.store (fun () => Js.log (Reductive.Store.getState SimpleStore.store));

let unsubscribe2 =
  Reductive.Store.subscribe
    SimpleStore.store (fun () => Js.log "Reductive.Store.getState SimpleStore.store");

let dispatch = Reductive.Store.dispatch SimpleStore.store;

dispatch Increment;

dispatch Increment;

dispatch Decrement;

dispatch Increment;

Reductive.Store.replaceReducer SimpleStore.store SimpleStore.doubleCounter;

/* this won't work because types are different :) */
/* Reductive.Store.replaceReducer store notACounter; */
dispatch Increment;

unsubscribe ();

dispatch Increment;

dispatch Increment;

dispatch Increment;

dispatch Increment;

dispatch Increment;

dispatch Increment;

Reductive.Store.subscribe
  ComplexStore.store (fun () => Js.log (Reductive.Store.getState ComplexStore.store));

Reductive.Store.dispatch ComplexStore.store (ComplexStore.StringAction A);

Reductive.Store.dispatch ComplexStore.store (ComplexStore.StringAction B);

Reductive.Store.dispatch ComplexStore.store (ComplexStore.CounterAction Increment);
