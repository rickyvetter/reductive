/**
 * Example of a todo list, loosely based on the redux example, mixed with
 * some code from Jared Forsyth's ReasonReact tutorial, and Cheng Lou's
 * todomvc ReasonReact example.
 *
 * Borrowed CSS from https://github.com/tastejs/todomvc-app-css
 */
type visibility =
  | ALL
  | ACTIVE
  | COMPLETED;

type action =
  | AddItem(string)
  | Show(visibility)
  | UpdateItem(int, string)
  | ToggleItem(int)
  | ToggleAll(bool)
  | ClearItem(int)
  | ClearCompleted;

type todo = {
  id: int,
  text: string,
  completed: bool,
};

type appState = {
  todos: list(todo),
  filter: visibility,
};

let lastId = ref(0);
let newItem = text => {
  lastId := lastId^ + 1;
  {id: lastId^, text, completed: false};
};

let appReducer = (state, action) =>
  switch (action) {
  | AddItem(text) => {...state, todos: [newItem(text), ...state.todos]}
  | Show(vis) => {...state, filter: vis}
  | UpdateItem(id, text) => {
      ...state,
      todos:
        List.map(
          todo => todo.id == id ? {...todo, text} : todo,
          state.todos,
        ),
    }
  | ToggleItem(id) => {
      ...state,
      todos:
        List.map(
          todo =>
            todo.id == id ? {...todo, completed: !todo.completed} : todo,
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
  };

let logger = (store, next) => Middleware.logger(store) @@ next;

let store =
  Reductive.Store.create(
    ~reducer=appReducer,
    ~preloadedState={todos: [], filter: ALL},
    ~enhancer=logger,
    (),
  );

module StoreProvider = {
  let make = Reductive.Provider.createMake(store);
};

/*
 * For a more recent version of reason-react, use this:
 *
 * let valueFromEvent = evt: string => ReactEvent.Form.target(evt)##value;
 */
let valueFromEvent = evt: string =>
  (evt |> ReactEventRe.Form.target |> ReactDOMRe.domElementToObj)##value;

module TodoItem = {
  type state = {
    editText: string,
    editing: bool,
    editField: ref(option(Dom.element)),
  };
  type action =
    | Edit
    | Submit
    | KeyDown(int)
    | Change(string);
  let component = ReasonReact.reducerComponent("TodoItem");
  let setEditFieldRef = (r, {ReasonReact.state}) =>
    state.editField := Js.Nullable.toOption(r);
  let make = (~todo, ~dispatch, _children) => {
    let submitHelper = state =>
      switch (String.trim(state.editText)) {
      | "" =>
        ReasonReact.SideEffects((_self => dispatch(ClearItem(todo.id))))
      | nonEmptyValue =>
        ReasonReact.UpdateWithSideEffects(
          {...state, editing: false, editText: nonEmptyValue},
          (_self => dispatch(UpdateItem(todo.id, nonEmptyValue))),
        )
      };
    {
      ...component,
      initialState: () => {
        editText: todo.text,
        editing: false,
        editField: ref(None),
      },
      reducer: action =>
        switch (action) {
        | Edit => (
            state =>
              ReasonReact.Update({
                ...state,
                editing: true,
                editText: todo.text,
              })
          )
        | Submit => submitHelper
        | Change(text) => (
            state =>
              state.editing ?
                ReasonReact.Update({...state, editText: text}) :
                ReasonReact.NoUpdate
          )
        | KeyDown(27) => (
            state =>
              ReasonReact.Update({
                ...state,
                editing: false,
                editText: todo.text,
              })
          )
        | KeyDown(13) => submitHelper
        | KeyDown(_) => (_state => ReasonReact.NoUpdate)
        },
      didUpdate: ({oldSelf, newSelf}) =>
        switch (
          oldSelf.state.editing,
          newSelf.state.editing,
          newSelf.state.editField^,
        ) {
        | (false, true, Some(field)) =>
          let node = ReactDOMRe.domElementToObj(field);
          ignore(node##focus());
          ignore(
            node##setSelectionRange(node##value##length, node##value##length),
          );
        | _ => ()
        },
      render: self => {
        let className =
          [
            todo.completed ? "completed" : "",
            self.state.editing ? "editing" : "",
          ]
          |> String.concat(" ");
        <li className>
          <div className="view">
            {
              /* cannot set type_ with this version of reason-react? */
              ReasonReact.createDomElement(
                "input",
                ~props={
                  "className": "toggle",
                  "type": "checkbox",
                  "checked": todo.completed,
                  "onChange": _ => dispatch(ToggleItem(todo.id)),
                },
                [||],
              )
            }
            <label onDoubleClick={_event => self.send(Edit)}>
              {ReasonReact.string(todo.text)}
            </label>
            <button
              className="destroy"
              onClick={_ => dispatch(ClearItem(todo.id))}
            />
          </div>
          <input
            ref={self.handle(setEditFieldRef)}
            className="edit"
            value={self.state.editText}
            onBlur={_event => self.send(Submit)}
            onChange={event => self.send(Change(valueFromEvent(event)))}
            onKeyDown={
              event =>
                self.send(KeyDown(ReactEventRe.Keyboard.which(event)))
            }
          />
        </li>;
      },
    };
  };
};

module TodoList = {
  let component = ReasonReact.statelessComponent("TodoList");
  let make = (~todos, ~dispatch, _children) => {
    ...component,
    render: _self => {
      let todoItems =
        List.map(
          todo => <TodoItem key={string_of_int(todo.id)} todo dispatch />,
          todos,
        );
      let todoCount = List.length(todos);
      let completedCount =
        List.length(List.filter(todo => todo.completed, todos));
      let activeTodoCount = todoCount - completedCount;
      <section className="main">
        {
          /* cannot set type_ with this version of reason-react? */
          ReasonReact.createDomElement(
            "input",
            ~props={
              "className": "toggle-all",
              "id": "toggle-all",
              "type": "checkbox",
              "onChange": _ => dispatch(ToggleAll(activeTodoCount > 0)),
              "checked": todoCount > 0 && activeTodoCount === 0,
            },
            [||],
          )
        }
        <label htmlFor="toggle-all">
          {ReasonReact.string("Mark all as complete")}
        </label>
        <ul className="todo-list">
          {ReasonReact.array(Belt.List.toArray(todoItems))}
        </ul>
      </section>;
    },
  };
};

module TodoInput = {
  type state = string;
  let component = ReasonReact.reducerComponent("TodoInput");
  let make = (~onSubmit, _children) => {
    ...component,
    initialState: () => "",
    reducer: (newText, _text) => ReasonReact.Update(newText),
    render: self =>
      /* cannot set type_ with this version of reason-react? */
      ReasonReact.createDomElement(
        "input",
        ~props={
          "className": "new-todo",
          "type": "text",
          "value": self.state,
          "placeholder": "What needs to be done?",
          "onChange": evt => self.send(valueFromEvent(evt)),
          "onKeyDown": evt =>
            if (ReactEventRe.Keyboard.key(evt) == "Enter") {
              onSubmit(self.state);
              self.send("");
            },
          "autoFocus": true,
        },
        [||],
      ),
  };
};

module AddTodo = {
  let component = ReasonReact.statelessComponent("AddTodo");
  let make = (~state: appState, ~dispatch, _children) => {
    ...component,
    render: _self => {
      ignore(state);
      <TodoInput onSubmit={text => dispatch(AddItem(text))} />;
    },
  };
};

module VisibleTodoList = {
  let component = ReasonReact.statelessComponent("VisibleTodoList");
  let make = (~state: appState, ~dispatch, _children) => {
    ...component,
    render: _self => {
      let todos =
        switch (state.filter) {
        | ALL => state.todos
        | ACTIVE => List.filter(t => !t.completed, state.todos)
        | COMPLETED => List.filter(t => t.completed, state.todos)
        };
      <TodoList todos dispatch />;
    },
  };
};

module FilterLink = {
  let component = ReasonReact.statelessComponent("FilterLink");
  let make = (~filter, ~label, ~state: appState, ~dispatch, _children) => {
    ...component,
    render: _self => {
      let className = filter == state.filter ? "selected" : "";
      <li>
        <a className onClick={_ => dispatch(Show(filter))}>
          {ReasonReact.string(label)}
        </a>
      </li>;
    },
  };
};

module Footer = {
  let component = ReasonReact.statelessComponent("Footer");
  let make = (~state: appState, ~dispatch, _children) => {
    ...component,
    render: _self => {
      let completedCount =
        List.length(List.filter(todo => todo.completed, state.todos));
      let activeTodoCount = List.length(state.todos) - completedCount;
      let activeTodoWord = activeTodoCount === 1 ? "item" : "items";
      let clearButton =
        completedCount > 0 ?
          <button
            className="clear-completed"
            onClick={_ => dispatch(ClearCompleted)}>
            {ReasonReact.string("Clear completed")}
          </button> :
          ReasonReact.null;
      <footer className="footer">
        <span className="todo-count">
          <strong>
            {ReasonReact.string(string_of_int(activeTodoCount))}
          </strong>
          {ReasonReact.string(" " ++ activeTodoWord ++ " left")}
        </span>
        <ul className="filters">
          <StoreProvider
            component={FilterLink.make(~filter=ALL, ~label="All")}
          />
          <StoreProvider
            component={FilterLink.make(~filter=ACTIVE, ~label="Active")}
          />
          <StoreProvider
            component={FilterLink.make(~filter=COMPLETED, ~label="Completed")}
          />
        </ul>
        clearButton
      </footer>;
    },
  };
};

module App = {
  let component = ReasonReact.statelessComponent("App");
  let make = _children => {
    ...component,
    render: _self =>
      <div>
        <header className="header">
          <h1> {ReasonReact.string("todos")} </h1>
          <StoreProvider component=AddTodo.make />
        </header>
        <StoreProvider component=VisibleTodoList.make />
        <StoreProvider component=Footer.make />
      </div>,
  };
};

ReactDOMRe.renderToElementWithId(<App />, "TodoApp");