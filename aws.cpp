#include <aws/core/Aws.h>
#include <aws/dynamodb/DynamoDBClient.h>
#include <aws/dynamodb/model/AttributeDefinition.h>
#include <aws/dynamodb/model/PutItemRequest.h>
#include <aws/dynamodb/model/PutItemResult.h>
#include <iostream>
#include "aws.hpp"
// #include "include/dynamodb_samples.h"

bool AwsDoc::DynamoDB::putItem(const Aws::String &table_name,
                               const Aws::String &transaction_id_key,
                               const Aws::String &transaction_id_value,
                               const Aws::String &date_key,
                               const Aws::String &date_value,
                               const Aws::String &price_key,
                               const Aws::String &price_value,
                               const Aws::String &quantity_key,
                               const Aws::String &quantity_value,
                               const Aws::String &ticker_key,
                               const Aws::String &ticker_value,
                               const Aws::String &transaction_type_key,
                               const Aws::String &transaction_type_value,
                               const Aws::String &user_id_key,
                               const Aws::String &user_id_value,
                               const Aws::Client::ClientConfiguration &clientConfiguration) {
    Aws::DynamoDB::DynamoDBClient dynamoClient(clientConfiguration);

    Aws::DynamoDB::Model::PutItemRequest putItemRequest;
    putItemRequest.SetTableName(table_name);

    putItemRequest.AddItem(transaction_id_key,      Aws::DynamoDB::Model::AttributeValue().SetS(transaction_id_value));   
    putItemRequest.AddItem(date_key,                Aws::DynamoDB::Model::AttributeValue().SetS(date_value));
    putItemRequest.AddItem(price_key,               Aws::DynamoDB::Model::AttributeValue().SetS(price_value));
    putItemRequest.AddItem(quantity_key,            Aws::DynamoDB::Model::AttributeValue().SetS(quantity_value));
    
    putItemRequest.AddItem(ticker_key,              Aws::DynamoDB::Model::AttributeValue().SetS(ticker_value));
    putItemRequest.AddItem(transaction_type_key,    Aws::DynamoDB::Model::AttributeValue().SetS(transaction_type_value));
    putItemRequest.AddItem(user_id_key,             Aws::DynamoDB::Model::AttributeValue().SetS(user_id_value));

    const Aws::DynamoDB::Model::PutItemOutcome outcome = dynamoClient.PutItem(putItemRequest);
    if (outcome.IsSuccess()) {
        std::cout << "Successfully added Item!" << std::endl;
    }
    else {
        std::cerr << outcome.GetError().GetMessage() << std::endl;
    }

    return outcome.IsSuccess();
}


#ifndef TESTING_BUILD

int main(int argc, char **argv) {
    Aws::SDKOptions options;

    Aws::InitAPI(options);
    {

        // transaction_id_key, date, price, quantity, ticker, transaction_type, user_id
        const Aws::String table_name = "paper-trader-transactions";
        
        const Aws::String transaction_id_key = "productId";
        const Aws::String transaction_id_value = "3458uadsf3ggggg4ot34055345";

        const Aws::String date_key = "date";
        const Aws::String date_value = "some_date";

        const Aws::String price_key = "price";
        const Aws::String price_value = "some_price";

        const Aws::String quantity_key = "quantity";
        const Aws::String quantity_value = "4";

        const Aws::String ticker_key = "ticker";
        const Aws::String ticker_value = "TSLA";

        const Aws::String transaction_type_key = "transaction_type";
        const Aws::String transaction_type_value = "buy";

        const Aws::String user_id_key = "user_id";
        const Aws::String user_id_value = "albert";

        Aws::Client::ClientConfiguration clientConfig;

        AwsDoc::DynamoDB::putItem(table_name, 
                                    transaction_id_key, transaction_id_value, 
                                    date_key, date_value,
                                    price_key, price_value, 
                                    quantity_key, quantity_value,
                                    ticker_key, ticker_value,
                                    transaction_type_key, transaction_type_value,
                                    user_id_key, user_id_value,
                                    clientConfig);
    }
    Aws::ShutdownAPI(options);
    return 0;
}

#endif // TESTING_BUILD