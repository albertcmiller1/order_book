Feature: Order Book Happy Path 

  Scenario: Order Book Can Run Swiftly 
        Given an empty order book
        When we randomly add bids and asks for XX seconds
        Then the order book will process them swiftly