module type Config = {
  type context;
  let defaultValue: context;
};

module Make = (Config: Config) => {
  let context = React.createContext(Config.defaultValue);

  module Provider = {
    let make = context->React.Context.provider;

    [@bs.obj]
    external makeProps:
      (
        ~value: Config.context,
        ~children: React.element,
        ~key: string=?,
        unit
      ) =>
      {
        .
        "value": Config.context,
        "children": React.element,
      } =
      "";
  };
};
