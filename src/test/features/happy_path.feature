Feature: Order Book Happy Path 

  Scenario: Order Book Can Process Matches
     Given we have access to the order book
      When we add bids and asks, then process matches
      Then the order book will correctly process those matches