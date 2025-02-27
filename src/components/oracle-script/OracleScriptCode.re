module Styles = {
  open Css;

  let tableLowerContainer = style([position(`relative)]);
  let tableWrapper =
    style([padding(`px(24)), Media.mobile([padding2(~v=`px(20), ~h=`zero)])]);
  let codeImage = style([width(`px(20)), marginRight(`px(10))]);

  let scriptContainer =
    style([
      fontSize(`px(12)),
      lineHeight(`px(20)),
      fontFamilies([`custom("Roboto Mono"), `monospace]),
    ]);

  let padding = style([padding(`px(20))]);
  let titleSpacing = style([marginBottom(`px(8))]);
};

let renderCode = content => {
  <div className=Styles.scriptContainer>
    <ReactHighlight className=Styles.padding> {content |> React.string} </ReactHighlight>
  </div>;
};

[@react.component]
let make = (~url: string) =>
  {
    let ({ThemeContext.theme}, _) = React.useContext(ThemeContext.context);
    let%Opt code = CodeHook.getCode(url);
    Some(
      <div className=Styles.tableWrapper>
        <Row marginBottom=24>
          <Col col=Col.Six colSm=Col.Six>
            <div className={Css.merge([CssHelper.flexBox(), Styles.titleSpacing])}>
              <Heading size=Heading.H5 value="Platform" />
              <HSpacing size=Spacing.xs />
              <CTooltip
                tooltipPlacementSm=CTooltip.BottomLeft
                tooltipText="The platform to which to generate the code for">
                <Icon name="fal fa-info-circle" size=10 color={theme.textPrimary} />
              </CTooltip>
            </div>
            <Text value="OWASM v0.1" weight=Text.Regular size=Text.Lg block=true />
          </Col>
          <Col col=Col.Six colSm=Col.Six>
            <div className={Css.merge([CssHelper.flexBox(), Styles.titleSpacing])}>
              <Heading size=Heading.H5 value="Language" />
              <HSpacing size=Spacing.xs />
              <CTooltip tooltipText="The programming language">
                <Icon name="fal fa-info-circle" size=10 color={theme.textPrimary} />
              </CTooltip>
            </div>
            <Text value="Rust 1.40.0" weight=Text.Regular size=Text.Lg block=true />
          </Col>
        </Row>
        <Row marginBottom=24 marginBottomSm=12>
          <Col>
            <div className={CssHelper.flexBox(~justify=`spaceBetween, ())}>
              <div className={CssHelper.flexBox()}>
                <Icon name="fal fa-file" size=16 color={theme.textSecondary} />
                <HSpacing size=Spacing.sm />
                <Text value="src/logic.rs" weight=Text.Semibold size=Text.Lg block=true />
              </div>
              <CopyButton data=code title="Copy Code" />
            </div>
          </Col>
        </Row>
        <div className=Styles.tableLowerContainer> {code |> renderCode} </div>
      </div>,
    );
  }
  |> Belt.Option.getWithDefault(_, React.null);
