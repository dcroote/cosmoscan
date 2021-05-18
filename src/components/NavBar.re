module RenderDesktop = {
  module Styles = {
    open Css;

    let nav = (isActive, theme: Theme.t) =>
      style([
        padding3(~top=`px(16), ~h=`zero, ~bottom=`px(12)),
        cursor(`pointer),
        fontSize(`px(12)),
        hover([color(theme.textPrimary)]),
        active([color(theme.textPrimary)]),
        transition(~duration=400, "all"),
        color(isActive ? theme.textPrimary : theme.textSecondary),
        borderBottom(`px(4), `solid, isActive ? theme.baseBlue : `transparent),
      ]);
  };

  [@react.component]
  let make = (~routes) => {
    let currentRoute = ReasonReactRouter.useUrl() |> Route.fromUrl;

    let ({ThemeContext.theme}, _) = React.useContext(ThemeContext.context);

    <div className={CssHelper.flexBox(~justify=`spaceBetween, ())} id="navigationBar">
      {routes
       ->Belt.List.map(((v, route)) =>
           <div key=v className={CssHelper.flexBox(~justify=`spaceBetween, ())}>
             <Link className={Styles.nav(currentRoute == route, theme)} route>
               {v |> React.string}
             </Link>
           </div>
         )
       ->Array.of_list
       ->React.array}
    </div>;
  };
};

module RenderMobile = {
  module Styles = {
    open Css;

    let navContainer = (show, theme: Theme.t) =>
      style([
        display(`flex),
        flexDirection(`column),
        opacity(show ? 1. : 0.),
        zIndex(2),
        pointerEvents(show ? `auto : `none),
        width(`percent(100.)),
        position(`absolute),
        top(`px(62)),
        left(`zero),
        right(`zero),
        transition(~duration=400, "all"),
        backgroundColor(theme.secondaryBg),
        padding4(~top=`zero, ~left=`px(24), ~right=`px(24), ~bottom=`px(24)),
        boxShadow(
          Shadow.box(~x=`zero, ~y=`px(2), ~blur=`px(4), Css.rgba(0, 0, 0, `num(0.08))),
        ),
      ]);

    let nav = (theme: Theme.t) =>
      style([color(theme.textPrimary), padding2(~v=`px(16), ~h=`zero)]);
    let menuContainer =
      style([
        marginLeft(`px(10)),
        flexBasis(`px(24)),
        flexGrow(0.),
        flexShrink(0.),
        height(`px(24)),
        textAlign(`center),
      ]);
    let menu = style([width(`px(20)), display(`block)]);
    let cmcLogo = style([width(`px(19)), height(`px(19))]);
    let socialContainer = style([display(`flex), flexDirection(`row), marginTop(`px(10))]);
    let socialLink =
      style([
        display(`flex),
        flexDirection(`row),
        justifyContent(`center),
        alignItems(`center),
      ]);
    let backdropContainer = show =>
      style([
        width(`percent(100.)),
        height(`percent(100.)),
        backgroundColor(`rgba((0, 0, 0, `num(0.5)))),
        position(`fixed),
        opacity(show ? 1. : 0.),
        pointerEvents(show ? `auto : `none),
        left(`zero),
        top(`px(62)),
        transition(~duration=400, "all"),
      ]);
  };

  [@react.component]
  let make = (~routes) => {
    let (show, setShow) = React.useState(_ => false);

    let ({ThemeContext.theme}, _) = React.useContext(ThemeContext.context);

    <>
      <div className=Styles.menuContainer onClick={_ => setShow(prev => !prev)}>
        {show
           ? <Icon name="fal fa-times" color={theme.textPrimary} size=24 />
           : <Icon name="fal fa-bars" color={theme.textPrimary} size=24 />}
      </div>
      <div className={Styles.navContainer(show, theme)}>
        {routes
         ->Belt.List.map(((v, route)) =>
             <Link key=v className={Styles.nav(theme)} route onClick={_ => setShow(_ => false)}>
               <Text value=v size=Text.Lg />
             </Link>
           )
         ->Array.of_list
         ->React.array}
        <div className=Styles.socialContainer>
          <div className=Styles.socialLink>
            <AbsoluteLink href="https://twitter.com/bandprotocol">
              <Icon name="fab fa-twitter" color={theme.baseBlue} size=20 />
            </AbsoluteLink>
          </div>
          <HSpacing size={`px(24)} />
          <div className=Styles.socialLink>
            <AbsoluteLink href="https://t.me/bandprotocol">
              <Icon name="fab fa-telegram-plane" color={theme.baseBlue} size=21 />
            </AbsoluteLink>
          </div>
          <HSpacing size={`px(24)} />
          <div className=Styles.socialLink>
            <AbsoluteLink href="https://coinmarketcap.com/currencies/band-protocol">
              <img src=Images.cmcLogo className=Styles.cmcLogo />
            </AbsoluteLink>
          </div>
        </div>
      </div>
      <div onClick={_ => setShow(prev => !prev)} className={Styles.backdropContainer(show)} />
    </>;
  };
};

[@react.component]
let make = () => {
  let wenchangRoutes = [
    ("Home", Route.HomePage),
    ("Validators", ValidatorHomePage),
    ("Blocks", BlockHomePage),
    ("Transactions", TxHomePage),
    ("Proposals", ProposalHomePage),
  ];

  exception WrongNetwork(string);
  let routes =
    switch (Env.network) {
    | "WENCHANG" => wenchangRoutes
    | "GUANYU38"
    | "GUANYU" =>
      wenchangRoutes->Belt.List.concat([
        ("Data Sources", DataSourceHomePage),
        ("Oracle Scripts", OracleScriptHomePage),
        ("Requests", RequestHomePage),
      ])
    | _ => raise(WrongNetwork("Incorrect or unspecified NETWORK environment variable"))
    };

  Media.isMobile() ? <RenderMobile routes /> : <RenderDesktop routes />;
};
