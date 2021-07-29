@@bs.config({jsx: 3})
@ocaml.doc("
  * Example of a todo list, loosely based on the redux example, mixed with
  * some code from Jared Forsyth's ReasonReact tutorial, and Cheng Lou's
  * todomvc ReasonReact example.
  *
  * Borrowed CSS from https://github.com/tastejs/todomvc-app-css
  ")
type visibility =
  | ALL
  | ACTIVE
  | COMPLETED

type appAction =
  | AddItem(string)
  | Show(visibility)
  | UpdateItem(int, string)
  | ToggleItem(int)
  | ToggleAll(bool)
  | ClearItem(int)
  | ClearCompleted

type todo = {
  id: int,
  text: string,
  completed: bool,
}

type appState = {
  todos: list<todo>,
  filter: visibility,
}

let lastId = ref(0)
let newItem = text => {
  lastId := lastId.contents + 1
  {id: lastId.contents, text: text, completed: false}
}

let appReducer = (state, action) =>
  switch action {
  | AddItem(text) => {...state, todos: list{newItem(text), ...state.todos}}
  | Show(vis) => {...state, filter: vis}
  | UpdateItem(id, text) => {
      ...state,
      todos: List.map(todo => todo.id == id ? {...todo, text: text} : todo, state.todos),
    }
  | ToggleItem(id) => {
      ...state,
      todos: List.map(
        todo => todo.id == id ? {...todo, completed: !todo.completed} : todo,
        state.todos,
      ),
    }
  | ToggleAll(checked) => {
      ...state,
      todos: List.map(todo => {...todo, completed: checked}, state.todos),
    }
  | ClearItem(id) => {
      ...state,
      todos: List.filter(todo => todo.id != id, state.todos),
    }
  | ClearCompleted => {
      ...state,
      todos: List.filter(todo => !todo.completed, state.todos),
    }
  }

let logger = (store, next) => \"@@"(Middleware.logger(store), next)

let appStore = Reductive.Store.create(
  ~reducer=appReducer,
  ~preloadedState={todos: list{}, filter: ALL},
  ~enhancer=logger,
  (),
)

module AppStore = {
  include ReductiveContext.Make({
    type action = appAction
    type state = appState
  })
}

let valueFromEvent = (evt): string => ReactEvent.Form.target(evt)["value"]

module TodoItem = {
  type state = {
    editText: string,
    editing: bool,
  }
  type action =
    | Edit
    | FinishEdit(string)
    | CancelEdit
    | Change(string)

  let getInitialState = todo => {editText: todo.text, editing: false}

  @react.component
  let make = (~todo, ~dispatch) => {
    let editFieldRef = React.useRef(Js.Nullable.null)

    let reducer = (state, action) =>
      switch action {
      | Edit => {editing: true, editText: todo.text}
      | FinishEdit(editText) => {editing: false, editText: editText}
      | Change(text) => state.editing ? {...state, editText: text} : state
      | CancelEdit => {editing: false, editText: todo.text}
      }
    let (state, todoItemDispatch) = React.useReducer(reducer, getInitialState(todo))

    React.useEffect1(() => {
      Js.log("Inside effect ")
      let optionRef = editFieldRef->React.Ref.current->Js.Nullable.toOption
      switch (state.editing, optionRef) {
      | (true, Some(field)) =>
        let node = ReactDOMRe.domElementToObj(field)
        Js.log(node)

        ignore(node["focus"]())
        ignore(node["setSelectionRange"](node["value"]["length"], node["value"]["length"]))
      | _ => ()
      }

      None
    }, [state.editing])

    let submit = () =>
      switch String.trim(state.editText) {
      | "" => dispatch(ClearItem(todo.id))
      | nonEmptyValue =>
        todoItemDispatch(FinishEdit(nonEmptyValue))
        dispatch(UpdateItem(todo.id, nonEmptyValue))
      }

    let handleKeyDown = key =>
      switch key {
      | 27 => todoItemDispatch(CancelEdit)
      | 13 => submit()
      | _ => ()
      }

    let className =
      list{todo.completed ? "completed" : "", state.editing ? "editing" : ""} |> String.concat(" ")

    <li className>
      <div className="view">
        <input
          type_="checkbox"
          className="toggle"
          checked=todo.completed
          onChange={_ => dispatch(ToggleItem(todo.id))}
        />
        <label onDoubleClick={_ => todoItemDispatch(Edit)}> {ReasonReact.string(todo.text)} </label>
        <button className="destroy" onClick={_ => dispatch(ClearItem(todo.id))} />
      </div>
      <input
        ref={ReactDOMRe.Ref.domRef(editFieldRef)}
        className="edit"
        value=state.editText
        onBlur={_ => submit()}
        onChange={event => {
          let value = valueFromEvent(event)
          todoItemDispatch(Change(value))
        }}
        onKeyDown={event => handleKeyDown(ReactEvent.Keyboard.which(event))}
      />
    </li>
  }
}

module TodoList = {
  @react.component
  let make = (~todos, ~dispatch) => {
    let todoItems = List.map(todo => <TodoItem key={string_of_int(todo.id)} todo dispatch />, todos)
    let todoCount = List.length(todos)
    let completedCount = List.length(List.filter(todo => todo.completed, todos))
    let activeTodoCount = todoCount - completedCount

    <section className="main">
      <input
        type_="checkbox"
        id="toggle-all"
        className="toggle-all"
        onChange={_ => dispatch(ToggleAll(activeTodoCount > 0))}
        checked={todoCount > 0 && activeTodoCount === 0}
      />
      <label htmlFor="toggle-all"> {ReasonReact.string("Mark all as complete")} </label>
      <ul className="todo-list"> {ReasonReact.array(Belt.List.toArray(todoItems))} </ul>
    </section>
  }
}

module TodoInput = {
  @react.component
  let make = (~onSubmit) => {
    let (value, setValue) = React.useState(() => "")
    let handleChange = event => {
      let value = valueFromEvent(event)
      setValue(_ => value)
    }
    let handleKeyDown = event =>
      if ReactEvent.Keyboard.key(event) == "Enter" {
        onSubmit(value)
        setValue(_ => "")
      }

    <input
      className="new-todo"
      type_="text"
      value
      placeholder="What needs to be done?"
      onChange=handleChange
      onKeyDown=handleKeyDown
      autoFocus=true
    />
  }
}

module AddTodo = {
  @react.component
  let make = (~dispatch) => <TodoInput onSubmit={text => dispatch(AddItem(text))} />
}

module VisibleTodoList = {
  @react.component
  let make = (~state: appState, ~dispatch) => {
    let todos = switch state.filter {
    | ALL => state.todos
    | ACTIVE => List.filter(t => !t.completed, state.todos)
    | COMPLETED => List.filter(t => t.completed, state.todos)
    }
    <TodoList todos dispatch />
  }
}

module FilterLink = {
  @react.component
  let make = (~filter, ~label, ~state: appState, ~dispatch) => {
    let className = filter == state.filter ? "selected" : ""
    <li> <a className onClick={_ => dispatch(Show(filter))}> {ReasonReact.string(label)} </a> </li>
  }
}

module Footer = {
  @react.component
  let make = (~state: appState, ~dispatch) => {
    let completedCount = List.length(List.filter(todo => todo.completed, state.todos))
    let activeTodoCount = List.length(state.todos) - completedCount
    let activeTodoWord = activeTodoCount === 1 ? "item" : "items"
    let clearButton =
      completedCount > 0
        ? <button className="clear-completed" onClick={_ => dispatch(ClearCompleted)}>
            {ReasonReact.string("Clear completed")}
          </button>
        : ReasonReact.null
    <footer className="footer">
      <span className="todo-count">
        <strong> {ReasonReact.string(string_of_int(activeTodoCount))} </strong>
        {ReasonReact.string(" " ++ (activeTodoWord ++ " left"))}
      </span>
      <ul className="filters">
        <FilterLink filter=ALL label="All" state dispatch />
        <FilterLink filter=ACTIVE label="Active" state dispatch />
        <FilterLink filter=COMPLETED label="Completed" state dispatch />
      </ul>
      clearButton
    </footer>
  }
}

let stateSelector = state => state

module App = {
  @react.component
  let make = () => {
    let dispatch = AppStore.useDispatch()
    let state = AppStore.useSelector(stateSelector)

    <div>
      <header className="header">
        <h1> {ReasonReact.string("todos")} </h1> <AddTodo dispatch />
      </header>
      <VisibleTodoList state dispatch />
      <Footer state dispatch />
    </div>
  }
}

ReactDOMRe.renderToElementWithId(
  <AppStore.Provider store=appStore> <App /> </AppStore.Provider>,
  "TodoApp",
)
