module Styles = {
  open Css;

  let noDataImage = style([width(`auto), height(`px(70)), marginBottom(`px(16))]);
};

module RenderBody = {
  [@react.component]
  let make = (~requestsSub: ApolloHooks.Subscription.variant(RequestSub.t)) => {
    let ({ThemeContext.theme}, _) = React.useContext(ThemeContext.context);

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
           | Data({oracleScript: {oracleScriptID, name}}) =>
             <div className={CssHelper.flexBox()}>
               <TypeID.OracleScript id=oracleScriptID />
               <HSpacing size=Spacing.sm />
               <Text value=name ellipsis=true color={theme.textPrimary} />
             </div>
           | _ => <LoadingCensorBar width=270 height=15 />
           }}
        </Col>
        <Col col=Col.Three>
          {switch (requestsSub) {
           | Data({requestedValidators, minCount, reports}) =>
             <ProgressBar
               reportedValidators={reports |> Belt.Array.size}
               minimumValidators=minCount
               requestValidators={requestedValidators |> Belt.Array.size}
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
             | Data({transactionOpt}) =>
               switch (transactionOpt) {
               | Some(transaction) =>
                 <Timestamp time={transaction.block.timestamp} textAlign=Text.Right size=Text.Md />
               | None => <Text value="Syncing" />
               }
             | _ => <LoadingCensorBar width=80 height=15 />
             }}
          </div>
        </Col>
      </Row>
    </TBody>;
  };
};

module RenderBodyMobile = {
  [@react.component]
  let make = (~reserveIndex, ~requestsSub: ApolloHooks.Subscription.variant(RequestSub.t)) => {
    switch (requestsSub) {
    | Data({
        id,
        transactionOpt,
        oracleScript: {oracleScriptID, name},
        requestedValidators,
        minCount,
        reports,
        resolveStatus,
      }) =>
      <MobileCard
        values=InfoMobileCard.[
          ("Request ID", RequestID(id)),
          ("Oracle Script", OracleScript(oracleScriptID, name)),
          (
            "Report Status",
            ProgressBar({
              reportedValidators: {
                reports |> Belt_Array.size;
              },
              minimumValidators: minCount,
              requestValidators: {
                requestedValidators |> Belt_Array.size;
              },
            }),
          ),
          (
            "Timestamp",
            switch (transactionOpt) {
            | Some(transaction) => Timestamp(transaction.block.timestamp)
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
let make = () => {
  let isMobile = Media.isMobile();

  let (page, setPage) = React.useState(_ => 1);
  let pageSize = 10;

  let latestRequestSub = RequestSub.getList(~pageSize=1, ~page=1, ());
  let requestsSub = RequestSub.getList(~pageSize, ~page, ());

  let allSub = Sub.all2(requestsSub, latestRequestSub);

  let ({ThemeContext.theme, isDarkMode}, _) = React.useContext(ThemeContext.context);

  <Section>
    <div className=CssHelper.container id="requestsSection">
      <Row alignItems=Row.Center marginBottom=40 marginBottomSm=24>
        <Col col=Col.Twelve>
          <Heading value="All Requests" size=Heading.H2 marginBottom=16 marginBottomSm=8 />
          {switch (allSub) {
           | Data((_, latestRequest)) =>
             <Heading
               value={
                 latestRequest
                 ->Belt.Array.get(0)
                 ->Belt.Option.mapWithDefault(0, ({id}) => id |> ID.Request.toInt)
                 ->Format.iPretty
                 ++ " In total"
               }
               size=Heading.H3
             />
           | _ => <LoadingCensorBar width=65 height=21 />
           }}
        </Col>
      </Row>
      <Table>
        {isMobile
           ? React.null
           : <THead>
               <Row alignItems=Row.Center>
                 <Col col=Col.Two>
                   <Text
                     block=true
                     value="Request ID"
                     size=Text.Sm
                     weight=Text.Semibold
                     transform=Text.Uppercase
                   />
                 </Col>
                 <Col col=Col.Four>
                   <Text
                     block=true
                     value="Oracle Script"
                     size=Text.Sm
                     weight=Text.Semibold
                     transform=Text.Uppercase
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
                     size=Text.Sm
                     weight=Text.Semibold
                     align=Text.Right
                     transform=Text.Uppercase
                   />
                 </Col>
               </Row>
             </THead>}
        {switch (allSub) {
         | Data((requests, latestRequest)) =>
           let requestsCount =
             latestRequest
             ->Belt.Array.get(0)
             ->Belt.Option.mapWithDefault(0, ({id}) => id |> ID.Request.toInt);
           let pageCount = Page.getPageCount(requestsCount, pageSize);
           <>
             {requestsCount > 0
                ? requests
                  ->Belt_Array.mapWithIndex((i, e) =>
                      isMobile
                        ? <RenderBodyMobile
                            reserveIndex=i
                            key={e.id |> ID.Request.toString}
                            requestsSub={Sub.resolve(e)}
                          />
                        : <RenderBody
                            key={e.id |> ID.Request.toString}
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
                 ? <RenderBodyMobile reserveIndex=i key={i |> string_of_int} requestsSub=noData />
                 : <RenderBody key={i |> string_of_int} requestsSub=noData />
             )
           ->React.array
         }}
      </Table>
    </div>
  </Section>;
};
