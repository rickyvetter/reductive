# Reductive

Reductive, like [redux](https://github.com/reactjs/redux), is a predictable
state container for [Reason](https://github.com/facebook/reason) applications.
Its scope goes beyond that of managing state for a single component, and
concerns the application as a whole.

## Hooks

`ReasonReact` version 0.7.0 has added [support](https://reasonml.github.io/reason-react/blog/2019/04/10/react-hooks) for react hooks and `reductive` now includes a set of hooks to subscribe to the store and dispatch actions. With the new hooks there is no need to use `Lense`s that wrap components, which results in simpler and cleaner code with a flat component tree.

The good ol' `Lense` is still available, since there is support for the old `jsx` and reducer style components, but is considered to be deprecated, since the old `jsx` syntax is also marked as [deprecated](https://reasonml.github.io/reason-react/docs/en/jsx-2). The preferred way of using `reductive` is via the new hooks API.

### Requirements

The new hooks API is built on top of the existing `react` hooks. In order to use hooks in `ReasonReact`, you have to use the [new jsx](https://reasonml.github.io/reason-react/blog/2019/04/10/react-hooks) syntax for `ReasonReact` components and `^5.0.4` or `^6.0.1` of `bs-platform`.

New projects will use the latest `jsx` version at the [application level](https://reasonml.github.io/reason-react/docs/en/jsx#application-level) by specifying `"react-jsx": 3` in `bsconfig.json`, which allows to use hooks. Existing projects can be gradually converted using `[@bs.config {jsx: 3}]` to enable the new version at the [file level](https://reasonml.github.io/reason-react/docs/en/jsx#file-level).

### useSelector

### useDispatch

## Use case

For simpler use cases, it might be sufficient with the [`useReducer`](https://reactjs.org/docs/hooks-reference.html#usereducer) hook to manage state on a component level, or combining this approach with react [context](https://reactjs.org/docs/context.html) to allow components deeper in the tree receive updates from the global state via `useContext` hook.

The latter approach, however, doesn't allow to subscribe to only part of the global state, resulting in all subscribed components re-render any time something in the state changes (even if they are not interested in particular updates). This is a known issue and happens since there is no bail-out mechanism inside `useContext`,
see [long github thread](https://github.com/facebook/react/issues/14110) for a really deep insight.

This might not be a problem for many applications, or might become a one for the others. `reductive` exposes `useSelector` hook, that makes sure only the components interested in a particular update are re-rendered, and not the rest.

## Requirements

A recent release of [Node](https://nodejs.org/en/) LTS should be sufficient.

## Installation

Installation of this module can be achieved using the `npm` command like so:

```shell
$ npm install reasonml-community/reductive
```

## Examples

See the `examples` directory for several working examples using reductive. The
`basic` example is console logging only. The `immutable` example may be broken
due to an API incompatibility. The `render` example demonstrates the
effectiveness of the `Lens`, in that only the components whose state has changed
will be re-rendered; turn on the "highlight updates" option in React Devtools to
see the effect. The `todomvc` example shows the use of reducer components along
with reductive. While the todomvc example looks a lot like those of the [todomvc
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
