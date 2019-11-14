# Reductive

Reductive, like [redux](https://github.com/reactjs/redux), is a predictable
state container for [Reason](https://github.com/facebook/reason) applications.
Its scope goes beyond that of managing state for a single component, and
concerns the application as a whole.

## Use case

For simpler use cases, it might be sufficient with the [`useReducer`](https://reactjs.org/docs/hooks-reference.html#usereducer) hook to manage state on a component level, or combining this approach with react [context](https://reactjs.org/docs/context.html) to allow components deeper in the tree receive updates via `useContext` hook. For more, see [you might not need redux](https://medium.com/@dan_abramov/you-might-not-need-redux-be46360cf367), especially so in a language which provides good enough construction blocks out of the box.

The mentioned approach, however, doesn't allow to subscribe to only part of the global state, resulting in all subscribed components re-render any time something in the state changes (even if they are not interested in particular updates). This is a known issue and occurs since there is no bail-out mechanism inside `useContext`,
see [long github thread](https://github.com/facebook/react/issues/14110) for a really deep insight.

This might not be a problem for many applications, or might become a one for the others. `reductive` exposes `useSelector` hook, that makes sure only the components interested in a particular update are re-rendered, and not the rest.

## Installation

Install via:

```shell
$ npm install --save reductive
```

and add it to `bsconfig.json`:

```sh
"bs-dependencies": ["reductive"]
```

## Hooks

`ReasonReact` version 0.7.0 has added [support](https://reasonml.github.io/reason-react/blog/2019/04/10/react-hooks) for react hooks and `reductive` now includes a set of hooks to subscribe to the store and dispatch actions. With the new hooks there is no need to use `Lense`s that wrap components, which results in simpler and cleaner code with a flat component tree. Moreover, the new hooks API is [safe to use in concurrent mode](https://github.com/facebook/react/tree/master/packages/use-subscription#use-subscription).

The `Lense` API is still available, since there is support for the old `jsx` and reducer style components, but is marked as deprecated, since the old `jsx` syntax is also [deprecated](https://reasonml.github.io/reason-react/docs/en/jsx-2) in the `reason-react` docs. The preferred way of using `reductive` is via the new hooks API.

### Requirements

The new hooks API is built on top of the existing `react` hooks. In order to use hooks in `ReasonReact`, you have to use the [new jsx](https://reasonml.github.io/reason-react/blog/2019/04/10/react-hooks) syntax for `ReasonReact` components and `^5.0.4` or `^6.0.1` of `bs-platform`.

New projects will use the latest `jsx` version by default at the [application level](https://reasonml.github.io/reason-react/docs/en/jsx#application-level) by having `"react-jsx": 3` in `bsconfig.json`. Existing projects can be gradually converted using `[@bs.config {jsx: 3}]` to enable the new version at the [file level](https://reasonml.github.io/reason-react/docs/en/jsx#file-level).

### Setup store and context provider

The new hooks API makes use of react `context` to make the store available to all nested components. You will need to create a store, implement a module with
context provider and hooks, and render the provider with the created store at the top of the component tree.

First, define the state and action types and reducer for your application, and create a store:

```reason
type appState = { counter: int };
type appAction =
  | Increment
  | Decrement;

let appReducer = (state, action) => ...;
let appStore =
  Reductive.Store.create(
    ~reducer=appReducer,
    ~preloadedState={counter: 0},
    (),
  );
```

Then create a customized version of the context and hooks for your application:

```reason
module AppStore = {
  include ReductiveContext.Make({
    type action = appAction;
    type state = appState;
  });
};
```

This will create a "typed" version of the store context and hooks with the `action` and `state` types specific to your application. If you are curious, `ReductiveContext.Make` is called a [functor](https://reasonml.github.io/docs/en/module#module-functions-functors), which is a module that acts as a function, and can be used to make custom versions of a module for different data structures.

Finally, use the provider from `AppStore` when rendering your root component passing in the created `store`:

```reason
ReactDOMRe.renderToElementWithId(
  <AppStore.Provider store=appStore> <Root /> </AppStore.Provider>,
  "root",
);
```

From now on you will access the hooks from your `AppStore` module, like `AppStore.useSelector` and `AppStore.useDispatch`.

### useSelector

Subscribes to changes to a selected portion of the store state, specified by a selector function. The selector function accepts the whole store state and runs whenever an action is dispatched or the component renders (for some other reason than store updates).

`useSelector` is built on top of the [`useSubscription`](https://github.com/facebook/react/tree/master/packages/use-subscription) hook, which is [safe to use](https://github.com/facebook/react/tree/master/packages/use-subscription#limitations-in-concurrent-mode) in the concurrent mode.

#### Selector function

The selector function is required to have a **stable reference** in order to avoid infinite re-renders. The easiest to achieve this is to declare it outside of the component,

```reason
// declare selector outside of the component
let userSelector = state => state.user;

// in the component
let user = AppStore.useSelector(userSelector);
```

or memoize with `useCallback` if it depends on `props`, `state` or anything else accessible only inside of the component:

```reason
[@react.component]
let make = (~id) => {
  let productSelector =
    React.useCallback1(
      state => state.products->Belt.List.keep(p => p.id === id),
      [|id|],
    );
  let product = AppStore.useSelector(productSelector);
  ...
};
```

#### Re-renders

`useSelector` relies on `useState` under the hood and therefore allows to bail-out of re-render similar to how `useState` [works](https://reactjs.org/docs/hooks-reference.html#bailing-out-of-a-state-update), which will compare by value for primitive types, and by reference for objects.

If the selector function returns an object, it won't cause a re-render only if the new object has the same **reference** as the previous one, and returning a new object every time will **always** cause a re-render. For example,

```reason
let selector = state => {email: state.user.email, cart: state.shop.cart};

// in the component
let selectedState = AppStore.useSelector(selector);
```

will cause the component to re-render every time an action is dispatched, regardless of whether `user` or `shop` have changed, since running the selector function will create a new object every time it is called. To prevent those re-renders, it is recommended to have multiple calls to `useSelector`, one per each individual field:

```reason
let emailSelector = state => state.user.email;
let cartSelector = state => cart: state.shop.cart;

// in the component
let email = AppStore.useSelector(emailSelector);
let cart = AppStore.useSelector(cartSelector);
```

This helps to make sure the component re-renders only when either `email` or `cart` on the store state changes.

This is different from how `mapStateToProps`, if you are used to dealing with the traditional `redux` flow, since `mapStateToProps` will compare individual fields of the returned object.

### useDispatch

Returns the dispatch function from the store:

```reason
let dispatch = AppStore.useDispatch();
...
dispatch(Increment);
```

### useStore

This hook returns a reference to the store that was passed in to the `<Provider>` component.

This hook should probably not be used frequently. Prefer `useSelector()` as your primary choice. However, this may be useful for less common scenarios that do require access to the store, such as replacing reducers.

## Requirements

A recent release of [Node](https://nodejs.org/en/) LTS should be sufficient.

## Examples

See the `examples` directory for several working examples using reductive. The
`basic` example is console logging only. The `immutable` example may be broken
due to an API incompatibility. The `render` example demonstrates the
effectiveness of the hooks, in that only the components whose state has changed
will be re-rendered; turn on the "highlight updates" option in React DevTools to
see the effect. The `todomvc` example shows the use of `useReducer` along
with `reductive`. While the todomvc example looks a lot like those of the [todomvc
project](http://todomvc.com/), it does not conform to the todomvc application
specification, instead focusing on demonstrating the usefulness of reductive.

### Running the Examples

Start by cloning this repository, then get everything installed and built:

```shell
$ npm install
$ npm run build
$ npx webpack
```

You can then open any of the HTML files in the `test` folder within your browser.

## Comparisons with Redux

### Actions

Redux [actions](http://redux.js.org/docs/basics/Actions.html) are implemented as plain JS objects. JS objects can be a bit too flexible, and many Redux users rely on standardized shapes for their actions to make sure that middlewares and third party libraries work with them. Reason has language-level support for composing a set of data types which many functions can operate over. They are called [variants](https://reasonml.github.io/docs/en/variant.html) and you can see how they are used [here](https://github.com/reasonml-community/reductive/blob/master/examples/basic/simpleStore.re).

### Action Creators

Redux [action creators](http://redux.js.org/docs/basics/Actions.html#action-creators) are used as composable ways of generating plain JS objects that match the schema of a specific action. In Reason, the type system does a lot of this work for you, so you can reliably put action types directly in your code and know that things will Just Work™. This has an added performance advantage of shifting that function call to compile time.

### Enhancer Composition

Much of the power of redux comes from having a unified API for writing and using middleware or enhancers. They're typically wired together behind the scenes in the [applyMiddlewares function](http://redux.js.org/docs/api/applyMiddleware.html). This function doesn't exist in Reductive, because Reason has language-level support for function composition with operators like [`|>` and `@@`](https://caml.inria.fr/pub/docs/manual-ocaml/libref/Pervasives.html#6_Compositionoperators). You can see some [example usage here](https://github.com/reasonml-community/reductive/blob/master/examples/immutable/timeTravelStore.re#L86-L87) and [some of the deprecation messages](https://github.com/reasonml-community/reductive/blob/master/src/reductive.rei#L39-L112) to get a better understanding of typical usage.

### The Source

The code behind Reductive is incredibly simple. The [first 40 lines](https://github.com/reasonml-community/reductive/blob/master/src/reductive.re#L1-L40) include the entire re-implementation of [redux](https://github.com/reactjs/redux). The [next ~40 lines](https://github.com/reasonml-community/reductive/blob/master/src/reductive.re#L42-L83) are a re-implementation of the [react-redux](https://github.com/reactjs/react-redux) library (without the higher-order component connect style implementation). The code is short and relatively simple to follow.
