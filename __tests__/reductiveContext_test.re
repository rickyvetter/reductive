[@bs.config {jsx: 3}];
open Jest;
open Expect;
open ReasonHooksTestingLibrary;

module TestStoreContext = {
  type testState = {
    counter: int,
    content: string,
  };

  type testAction =
    | Increment
    | Decrement
    | AppendA
    | AppendB
    | Reset;

  let testReducer = (state, action) =>
    switch (action) {
    | Increment => {...state, counter: state.counter + 1}
    | Decrement => {...state, counter: state.counter - 1}
    | AppendA => {...state, content: state.content ++ "A"}
    | AppendB => {...state, content: state.content ++ "B"}
    | Reset => {counter: 0, content: ""}
    };

  let appStore =
    Reductive.Store.create(
      ~reducer=testReducer,
      ~preloadedState={counter: 0, content: ""},
      (),
    );

  include ReductiveContext.Make({
    type action = testAction;
    type state = testState;
    let store = appStore;
  });
};

module App = {
  [@react.component]
  let make = (~children) => {
    <TestStoreContext.Provider> children </TestStoreContext.Provider>;
  };
};

type selectorMockTest = {. "selector": TestStoreContext.testState => int};
type spyReturnMockValue = string;

let getResult = container =>
  container->Testing.Result.result->Testing.Result.current;

describe("useSelector", () => {
  open Testing;
  open ReactTestingLibrary;

  let options = Options.t(~wrapper=App.make, ());
  beforeEach(() =>
    Reductive.Store.dispatch(TestStoreContext.appStore, Reset)
  );

  test("sets initial state correctly", () => {
    let container =
      renderHook(
        () => TestStoreContext.useSelector(state => state.counter),
        ~options,
        (),
      );

    expect(getResult(container)) |> toEqual(0);
  });

  test("selects the state and re-renders component on store updates", () => {
    let container =
      renderHook(
        () => TestStoreContext.useSelector(state => state.counter),
        ~options,
        (),
      );

    act(() => {
      Reductive.Store.dispatch(TestStoreContext.appStore, Increment);
      ();
    });

    expect(getResult(container)) |> toEqual(1);
  });

  test("always gives back the latest state", () => {
    let renderedStates: ref(array(int)) = ref([||]);

    module Comp = {
      [@react.component]
      let make = () => {
        let counter = TestStoreContext.useSelector(s => s.counter);
        renderedStates := Belt.Array.concat(renderedStates^, [|counter|]);
        <div />;
      };
    };

    let element = <App> <Comp /> </App>;
    render(element) |> ignore;

    act(() => {
      Reductive.Store.dispatch(TestStoreContext.appStore, Increment);
      Reductive.Store.dispatch(TestStoreContext.appStore, Decrement);
      ();
    });

    expect(renderedStates^) |> toEqual([|0, 1, 0|]);
  });

  test("prevents re-render if selected state is referentially equal", () => {
    let renderedStates: ref(array(int)) = ref([||]);

    module Comp = {
      [@react.component]
      let make = () => {
        let counter = TestStoreContext.useSelector(s => s.counter);
        renderedStates := Belt.Array.concat(renderedStates^, [|counter|]);
        <div />;
      };
    };

    let element = <App> <Comp /> </App>;
    render(element) |> ignore;

    act(() => {
      Reductive.Store.dispatch(TestStoreContext.appStore, AppendA);
      Reductive.Store.dispatch(TestStoreContext.appStore, AppendB);
      ();
    });

    expect(renderedStates^) |> toEqual([|0|]);
  });

  test("correctly updates selected state if selector depends on props", () => {
    let renderedStates: ref(array(int)) = ref([||]);

    module Comp = {
      [@react.component]
      let make = (~prop) => {
        let counter = TestStoreContext.useSelector(s => s.counter + prop);
        renderedStates := Belt.Array.concat(renderedStates^, [|counter|]);
        <div />;
      };
    };

    let updateProp = ref(() => ());
    module Parent = {
      [@react.component]
      let make = () => {
        let (prop, dispatch) = React.useReducer((s, _) => s + 1, 0);
        updateProp := dispatch;
        <Comp prop />;
      };
    };
    let element = <App> <Parent /> </App>;

    render(element) |> ignore;

    act(() => {
      Reductive.Store.dispatch(TestStoreContext.appStore, Increment); // state.counter - 1, prop - 0
      updateProp^(); // state.counter - 1, prop - 1
      updateProp^(); // state.counter - 1, prop - 2
      Reductive.Store.dispatch(TestStoreContext.appStore, Increment); // state.counter - 2, prop - 2
      ();
    });
    expect(renderedStates^) |> toEqual([|0, 1, 2, 3, 4|]);
  });

  test("removes subscription on unmount", () => {
    let setupSpy: selectorMockTest => spyReturnMockValue = [%bs.raw
      {|
      function (obj) {
        return jest.spyOn(obj, "selector");
      }
    |}
    ];

    let selector = (state: TestStoreContext.testState) => state.counter;
    let selectorSpyObject = ref({"selector": selector});

    let selectStateFromSpy = state => {
      let spySelector = (selectorSpyObject^)##selector;
      spySelector(state);
    };

    module Comp = {
      [@react.component]
      let make = () => {
        let _ = TestStoreContext.useSelector(selectStateFromSpy);
        <div />;
      };
    };

    let element = <App> <Comp /> </App>;
    render(element) |> unmount() |> ignore;

    // start spying after unmount
    let spy = setupSpy(selectorSpyObject^);

    act(() => {
      Reductive.Store.dispatch(TestStoreContext.appStore, Increment);
      Reductive.Store.dispatch(TestStoreContext.appStore, Decrement);
      Reductive.Store.dispatch(TestStoreContext.appStore, Increment);
      ();
    });

    let expectSelectorToHaveBeenCalled: spyReturnMockValue => unit = [%bs.raw
      {|
        function (spy) {
          console.log(spy)
          return expect(spy).toHaveBeenCalledTimes(0)
        }
      |}
    ];

    expectSelectorToHaveBeenCalled(spy);
    expect(true) |> toEqual(true);
  });
});
