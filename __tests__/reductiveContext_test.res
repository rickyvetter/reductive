@@bs.config({jsx: 3})
open Jest
open Expect
open ReasonHooksTestingLibrary

module TestStoreContext = {
  type testState = {
    counter: int,
    content: string,
  }

  type testAction =
    | Increment
    | Decrement
    | AppendA
    | AppendB
    | Reset

  let testReducer = (state, action) =>
    switch action {
    | Increment => {...state, counter: state.counter + 1}
    | Decrement => {...state, counter: state.counter - 1}
    | AppendA => {...state, content: state.content ++ "A"}
    | AppendB => {...state, content: state.content ++ "B"}
    | Reset => {counter: 0, content: ""}
    }

  let appStore = Reductive.Store.create(
    ~reducer=testReducer,
    ~preloadedState={counter: 0, content: ""},
    (),
  )

  include ReductiveContext.Make({
    type action = testAction
    type state = testState
  })
}

module App = {
  @react.component
  let make = (~children) =>
    <TestStoreContext.Provider store=TestStoreContext.appStore>
      children
    </TestStoreContext.Provider>
}

type selectorMockTest = {"selector": TestStoreContext.testState => int}
type spyReturnMockValue = string

let getResult = container => container->Testing.Result.result->Testing.Result.current

describe("reductiveContext", () => {
  open Testing
  open ReactTestingLibrary

  let options = Options.t(~wrapper=App.make, ())
  beforeEach(() => Reductive.Store.dispatch(TestStoreContext.appStore, Reset))

  describe("useSelector", () => {
    test("sets initial state correctly", () => {
      let selector = (state: TestStoreContext.testState) => state.counter

      let container = renderHook(() => TestStoreContext.useSelector(selector), ~options, ())

      expect(getResult(container)) |> toEqual(0)
    })

    test("selects the state and re-renders component on store updates", () => {
      let selector = (state: TestStoreContext.testState) => state.counter

      let container = renderHook(() => TestStoreContext.useSelector(selector), ~options, ())

      act(() => {
        Reductive.Store.dispatch(TestStoreContext.appStore, Increment)
        ()
      })

      expect(getResult(container)) |> toEqual(1)
    })

    test("always gives back the latest state", () => {
      let selector = (state: TestStoreContext.testState) => state.counter
      let renderedStates: ref<array<int>> = ref([])

      module Comp = {
        @react.component
        let make = () => {
          let counter = TestStoreContext.useSelector(selector)
          renderedStates := Belt.Array.concat(renderedStates.contents, [counter])
          <div />
        }
      }

      let element = <App> <Comp /> </App>
      render(element) |> ignore

      act(() => {
        Reductive.Store.dispatch(TestStoreContext.appStore, Increment)
        Reductive.Store.dispatch(TestStoreContext.appStore, Decrement)
        ()
      })

      expect(renderedStates.contents) |> toEqual([0, 1, 0])
    })

    test("prevents re-render if selected state is referentially equal", () => {
      let renderedStates: ref<array<int>> = ref([])
      let selector = (state: TestStoreContext.testState) => state.counter

      module Comp = {
        @react.component
        let make = () => {
          let counter = TestStoreContext.useSelector(selector)
          renderedStates := Belt.Array.concat(renderedStates.contents, [counter])
          <div />
        }
      }

      let element = <App> <Comp /> </App>
      render(element) |> ignore

      act(() => {
        Reductive.Store.dispatch(TestStoreContext.appStore, AppendA)
        Reductive.Store.dispatch(TestStoreContext.appStore, AppendB)
        ()
      })

      expect(renderedStates.contents) |> toEqual([0])
    })

    test("correctly updates selected state if selector depends on props", () => {
      let renderedStates: ref<array<int>> = ref([])

      module Comp = {
        @react.component
        let make = (~prop) => {
          let selector = React.useCallback1(
            (s: TestStoreContext.testState) => s.counter + prop,
            [prop],
          )

          let counter = TestStoreContext.useSelector(selector)

          renderedStates := Belt.Array.concat(renderedStates.contents, [counter])
          <div />
        }
      }

      let updateProp = ref(() => ())
      module Parent = {
        @react.component
        let make = () => {
          let (prop, dispatch) = React.useReducer((s, _) => s + 1, 0)
          updateProp := dispatch
          <Comp prop />
        }
      }
      let element = <App> <Parent /> </App>

      render(element) |> ignore

      act(() => {
        Reductive.Store.dispatch(TestStoreContext.appStore, Increment) // state.counter - 1, prop - 0, total - 1
        updateProp.contents() // state.counter - 1, prop - 1, total - 2
        Reductive.Store.dispatch(TestStoreContext.appStore, Increment) // state.counter - 2, prop - 1, total - 3
        updateProp.contents() // state.counter - 2, prop - 2, total - 4
        Reductive.Store.dispatch(TestStoreContext.appStore, Increment) // state.counter - 3, prop - 2, total - 5
        ()
      })

      // changing selector function that depends on props leads to double re-render and duplicated state values
      let distinctRenderState =
        renderedStates.contents->Belt.Array.keepWithIndex((value, index) =>
          renderedStates.contents
          ->Belt.Array.getIndexBy(v => v === value)
          ->Belt.Option.getWithDefault(-1) === index
        )
      expect(distinctRenderState) |> toEqual([0, 1, 2, 3, 4, 5])
    })

    test("removes subscription on unmount", () => {
      let setupSpy: selectorMockTest => spyReturnMockValue = %raw(`
       function (obj) {
         return jest.spyOn(obj, "selector");
       }
     `)

      let selector = (state: TestStoreContext.testState) => state.counter
      let selectorSpyObject = ref({"selector": selector})

      let selectStateFromSpy = state => {
        let spySelector = selectorSpyObject.contents["selector"]
        spySelector(state)
      }

      module Comp = {
        @react.component
        let make = () => {
          let _ = TestStoreContext.useSelector(selectStateFromSpy)
          <div />
        }
      }

      let element = <App> <Comp /> </App>
      render(element) |> unmount() |> ignore

      // start spying after unmount
      let spy = setupSpy(selectorSpyObject.contents)

      act(() => {
        Reductive.Store.dispatch(TestStoreContext.appStore, Increment)
        Reductive.Store.dispatch(TestStoreContext.appStore, Decrement)
        Reductive.Store.dispatch(TestStoreContext.appStore, Increment)
        ()
      })

      let expectSelectorToHaveBeenCalled: spyReturnMockValue => unit = %raw(`
         function (spy) {
           console.log(spy)
           return expect(spy).toHaveBeenCalledTimes(0)
         }
       `)

      expectSelectorToHaveBeenCalled(spy)
      expect(true) |> toEqual(true)
    })
  })

  describe("useStore", () => {
    test("gets the store", () => {
      let container = renderHook(() => TestStoreContext.useStore(), ~options, ())

      expect(getResult(container)) |> toEqual(TestStoreContext.appStore)
    })

    test("re-renders component on store updates", () => {
      let container = renderHook(() => TestStoreContext.useStore(), ~options, ())

      act(() => {
        Reductive.Store.dispatch(TestStoreContext.appStore, Increment)
        ()
      })

      let state = container->getResult->Reductive.Store.getState
      expect(state.counter) |> toEqual(1)
    })
  })

  describe("useDispatch", () =>
    test("has stable reference", () => {
      let dispatchRerenders: ref<int> = ref(0)
      let forceRender = ref(() => ())

      module Comp = {
        @react.component
        let make = () => {
          let (_, setState) = React.useState(() => 0)
          forceRender := (() => setState(prev => prev + 1))

          let dispatch = TestStoreContext.useDispatch()
          React.useEffect1(() => {
            dispatchRerenders := dispatchRerenders.contents + 1
            None
          }, [dispatch])
          <div />
        }
      }

      let element = <App> <Comp /> </App>
      render(element) |> ignore

      act(() => {
        forceRender.contents()
        forceRender.contents()
        forceRender.contents()
        ()
      })
      expect(dispatchRerenders.contents) |> toEqual(1)
    })
  )
})
