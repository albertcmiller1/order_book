Feature: Order Book Happy Path 

  Scenario: Order Book Can Process Matches
     Given access to an empty order book
      When bids and asks are added
      Then the order book can corectly process matches