module Styles = {
  open Css;

  let tableWrapper = style([Media.mobile([padding2(~v=`px(16), ~h=`zero)])]);
  let icon = style([width(`px(80)), height(`px(80))]);
  let iconWrapper =
    style([
      width(`percent(100.)),
      display(`flex),
      flexDirection(`column),
      alignItems(`center),
    ]);
  let noDataImage = style([width(`auto), height(`px(70)), marginBottom(`px(16))]);
};

module RenderBody = {
  [@react.component]
  let make = (~requestsSub: ApolloHooks.Subscription.variant(RequestSub.Mini.t), ~theme: Theme.t) => {
    <TBody>
      <Row alignItems=Row.Center>
        <Col col=Col.Two>
          {switch (requestsSub) {
           | Data({id}) => <TypeID.Request id />
           | _ => <LoadingCensorBar width=135 height=15 />
           }}
        </Col>
        <Col col=Col.Four>
          {switch (requestsSub) {
           | Data({oracleScriptID, oracleScriptName}) =>
             <div className={CssHelper.flexBox()}>
               <TypeID.OracleScript id=oracleScriptID />
               <HSpacing size=Spacing.sm />
               <Text value=oracleScriptName ellipsis=true color={theme.textPrimary} />
             </div>
           | _ => <LoadingCensorBar width=270 height=15 />
           }}
        </Col>
        <Col col=Col.Three>
          {switch (requestsSub) {
           | Data({minCount, askCount, reportsCount}) =>
             <ProgressBar
               reportedValidators=reportsCount
               minimumValidators=minCount
               requestValidators=askCount
             />
           | _ => <LoadingCensorBar width=212 height=15 />
           }}
        </Col>
        <Col col=Col.One>
          <div className={CssHelper.flexBox(~justify=`flexEnd, ())}>
            {switch (requestsSub) {
             | Data({resolveStatus}) => <RequestStatus resolveStatus />
             | _ => <LoadingCensorBar width=100 height=15 />
             }}
          </div>
        </Col>
        <Col col=Col.Two>
          <div className={CssHelper.flexBox(~justify=`flexEnd, ())}>
            {switch (requestsSub) {
             | Data({txTimestamp}) =>
               switch (txTimestamp) {
               | Some(txTimestamp') =>
                 <Timestamp
                   time=txTimestamp'
                   size=Text.Md
                   weight=Text.Regular
                   textAlign=Text.Right
                 />
               | None => <Text value="Syncing" />
               }
             | _ => <LoadingCensorBar width=120 height=15 mt=5 />
             }}
          </div>
        </Col>
      </Row>
    </TBody>;
  };
};

module RenderBodyMobile = {
  [@react.component]
  let make = (~reserveIndex, ~requestsSub: ApolloHooks.Subscription.variant(RequestSub.Mini.t)) => {
    switch (requestsSub) {
    | Data({
        id,
        txTimestamp,
        oracleScriptID,
        oracleScriptName,
        minCount,
        askCount,
        reportsCount,
        resolveStatus,
      }) =>
      <MobileCard
        values=InfoMobileCard.[
          ("Request ID", RequestID(id)),
          ("Oracle Script", OracleScript(oracleScriptID, oracleScriptName)),
          (
            "Report Status",
            ProgressBar({
              reportedValidators: reportsCount,
              minimumValidators: minCount,
              requestValidators: askCount,
            }),
          ),
          (
            "Timestamp",
            switch (txTimestamp) {
            | Some(txTimestamp') => Timestamp(txTimestamp')
            | None => Text("Syncing")
            },
          ),
        ]
        key={id |> ID.Request.toString}
        idx={id |> ID.Request.toString}
        requestStatus=resolveStatus
      />
    | _ =>
      <MobileCard
        values=InfoMobileCard.[
          ("Request ID", Loading(70)),
          ("Oracle Script", Loading(136)),
          ("Report Status", Loading(20)),
          ("Timestamp", Loading(166)),
        ]
        key={reserveIndex |> string_of_int}
        idx={reserveIndex |> string_of_int}
      />
    };
  };
};

[@react.component]
let make = (~dataSourceID: ID.DataSource.t) => {
  let (page, setPage) = React.useState(_ => 1);
  let pageSize = 5;

  let requestsSub = RequestSub.Mini.getListByDataSource(dataSourceID, ~pageSize, ~page, ());
  let totalRequestCountSub = RequestSub.countByDataSource(dataSourceID);

  let allSub = Sub.all2(requestsSub, totalRequestCountSub);

  let isMobile = Media.isMobile();

  let ({ThemeContext.theme, isDarkMode}, _) = React.useContext(ThemeContext.context);

  <div className=Styles.tableWrapper>
    {isMobile
       ? <Row marginBottom=16>
           <Col>
             {switch (allSub) {
              | Data((_, totalRequestCount)) =>
                <div className={CssHelper.flexBox()}>
                  <Text
                    block=true
                    value={totalRequestCount |> Format.iPretty}
                    weight=Text.Semibold
                    size=Text.Sm
                  />
                  <HSpacing size=Spacing.xs />
                  <Text
                    block=true
                    value="Requests"
                    weight=Text.Semibold
                    size=Text.Sm
                    transform=Text.Uppercase
                  />
                </div>
              | _ => <LoadingCensorBar width=100 height=15 />
              }}
           </Col>
         </Row>
       : <THead>
           <Row alignItems=Row.Center>
             <Col col=Col.Two>
               {switch (allSub) {
                | Data((_, totalRequestCount)) =>
                  <div className={CssHelper.flexBox()}>
                    <Text
                      block=true
                      value={totalRequestCount |> Format.iPretty}
                      weight=Text.Semibold
                      transform=Text.Uppercase
                      size=Text.Sm
                    />
                    <HSpacing size=Spacing.xs />
                    <Text
                      block=true
                      value="Requests"
                      weight=Text.Semibold
                      transform=Text.Uppercase
                      size=Text.Sm
                    />
                  </div>
                | _ => <LoadingCensorBar width=100 height=15 />
                }}
             </Col>
             <Col col=Col.Four>
               <Text
                 block=true
                 value="Oracle Script"
                 weight=Text.Semibold
                 transform=Text.Uppercase
                 size=Text.Sm
               />
             </Col>
             <Col col=Col.Four>
               <Text
                 block=true
                 value="Report Status"
                 size=Text.Sm
                 weight=Text.Semibold
                 transform=Text.Uppercase
               />
             </Col>
             <Col col=Col.Two>
               <Text
                 block=true
                 value="Timestamp"
                 weight=Text.Semibold
                 size=Text.Sm
                 align=Text.Right
                 transform=Text.Uppercase
               />
             </Col>
           </Row>
         </THead>}
    {switch (allSub) {
     | Data((requests, requestsCount)) =>
       let pageCount = Page.getPageCount(requestsCount, pageSize);
       <>
         {requestsCount > 0
            ? requests
              ->Belt_Array.mapWithIndex((i, e) =>
                  isMobile
                    ? <RenderBodyMobile
                        key={e.id |> ID.Request.toString}
                        reserveIndex=i
                        requestsSub={Sub.resolve(e)}
                      />
                    : <RenderBody
                        key={e.id |> ID.Request.toString}
                        theme
                        requestsSub={Sub.resolve(e)}
                      />
                )
              ->React.array
            : <EmptyContainer>
                <img
                  src={isDarkMode ? Images.noDataDark : Images.noDataLight}
                  className=Styles.noDataImage
                />
                <Heading
                  size=Heading.H4
                  value="No Request"
                  align=Heading.Center
                  weight=Heading.Regular
                  color={theme.textSecondary}
                />
              </EmptyContainer>}
         {isMobile
            ? React.null
            : <Pagination
                currentPage=page
                pageCount
                onPageChange={newPage => setPage(_ => newPage)}
              />}
       </>;
     | _ =>
       Belt_Array.make(pageSize, ApolloHooks.Subscription.NoData)
       ->Belt_Array.mapWithIndex((i, noData) =>
           isMobile
             ? <RenderBodyMobile key={i |> string_of_int} reserveIndex=i requestsSub=noData />
             : <RenderBody key={i |> string_of_int} theme requestsSub=noData />
         )
       ->React.array
     }}
  </div>;
};
